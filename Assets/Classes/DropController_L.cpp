#include "DropController_L.h"
#include "ItemBox_L.h"
#include "PostDate.h"
#include "SimpleAudioEngine.h"


DropController_L::DropController_L()
{
	this->scheduleUpdate();

}

DropController_L::~DropController_L()
{
	NotificationCenter::getInstance()->removeAllObservers(this);
}

bool DropController_L::init(ItemBox_L* itemBox){
	bindItemBox(itemBox);

	NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(DropController_L::dropListener), "drop", NULL);

	return true;
}

void DropController_L::dropListener(Ref* date){
	findDropList();

	//若消除后无掉落项也恢复触摸
	if (dropList.size() == 0){
		NotificationCenter::getInstance()->postNotification("moveOver", NULL);
		NotificationCenter::getInstance()->postNotification("stepEnd", NULL);
	}
}

void DropController_L::findDropList(){

	//从左下到右上遍历，获取droplist
	for (int j = 0; j < getItemBox()->getCellNum().y; j++){
		for (int i = 0; i < getItemBox()->getCellNum().x; i++){
			if (getItemBox()->getItem_lgc(i, j)->getItemType() == ItemType_CLEAN){

				//找到最上面的clean状态物体，判断上方物体
				int k = 0;
				Entity* item = NULL;
				while ((item = getItemBox()->getItem_lgc(i, j + k)) != NULL && item->getItemType() == ItemType_CLEAN){
					k++;
				}

				Entity* item_clean = getItemBox()->getItem_lgc(i, j + k - 1);

				//3.若上方不可掉落，向斜上两边搜索
				if (item == NULL){
					int q = 0;
					//若上方的clean无法掉落，则判断其下方的clean,直到有clean项可以下落或者到尾端
					while ((item_clean = getItemBox()->getItem_lgc(i, j + k - 1 - q)) != NULL && item_clean->getItemType() == ItemType_CLEAN){
						//先左后右。若有可掉落项则掉落
						if ((item = getItemBox()->getItem_lgc(i - 1, j + k - q)) != NULL && (item->getItemType() > ClearItemType_BEGIN && item->getItemType() < ClearItemType_END) && item->getActionState() == Fixed){
							//若其下方可掉则等待
							if (getItemBox()->getItem_lgc(i - 1, j + k - 1 - q) != NULL && getItemBox()->getItem_lgc(i - 1, j + k - 1 - q)->getItemType() == ItemType_CLEAN){
								break;
							}
							getItemBox()->setItem(i, j + k - 1 - q, item);
							getItemBox()->setItem(i - 1, j + k - q, item_clean);
							item->setActionState(Droping);
							dropList.pushBack(item);
							aimPos.push_back(Point(((float)i + 0.5f) * getItemBox()->getCellSize().x, ((float)(j + k - 1 - q) + 0.5f) * getItemBox()->getCellSize().y));
							break;
						}
						else if ((item = getItemBox()->getItem_lgc(i + 1, j + k - q)) != NULL && (item->getItemType() > ClearItemType_BEGIN && item->getItemType() < ClearItemType_END) && item->getActionState() == Fixed){
							//若其下方可掉则等待
							if (getItemBox()->getItem_lgc(i + 1, j + k - 1 - q) != NULL && getItemBox()->getItem_lgc(i + 1, j + k - 1 - q)->getItemType() == ItemType_CLEAN){
								break;
							}
							getItemBox()->setItem(i, j + k - 1 - q, item);
							getItemBox()->setItem(i + 1, j + k - q, item_clean);
							item->setActionState(Droping);
							dropList.pushBack(item);
							aimPos.push_back(Point(((float)i + 0.5f) * getItemBox()->getCellSize().x, ((float)(j + k - 1 - q) + 0.5f) * getItemBox()->getCellSize().y));
							break;
						}

						q++;
					}

				}
				//2.上方为可消除物体类型或可掉落的目标物体
				else if (item->getItemType() > ClearItemType_BEGIN && item->getItemType() < ClearItemType_END && item->getActionState() != Droping){
					//若掉落状态则忽略
					if (item->getActionState() == Droping){
						continue;
					}//否则可掉落，与下方交换（防止掉落冲突，提前交换）
					else{
						getItemBox()->setItem(i, j + k - 1, item);
						getItemBox()->setItem(i, j + k, item_clean);
						item->setActionState(Droping);
						dropList.pushBack(item);
						aimPos.push_back(Point(((float)i + 0.5f) * getItemBox()->getCellSize().x, ((float)(j + k - 1) + 0.5f) * getItemBox()->getCellSize().y));
					}
				}
			}
		}
	}
}

void DropController_L::update(float dt){
	if (dropList.size() > 0){
		for (int i = 0; i < dropList.size(); i++){
			Entity* item;
			Point pos;

			//已到达目标位置
			if ((item = dropList.at(i))->getPosition().y <= (pos = aimPos.at(i)).y){
				item->setActionState(Arrived);
				//修正x坐标移动过度（y坐标不修正，以免产生卡顿感）
				item->setPosition((getItemBox()->getItemPos(item).x + 0.5f) * getItemBox()->getCellSize().x, item->getPosition().y);
				//移除目标
				dropList.eraseObject(item);
				aimPos.erase(aimPos.begin() + i);
				i--;
			}//未到达目标位置，继续掉落动画，更改位置
			else{
				float x = item->getPosition().x;
				float y = item->getPosition().y;
				y -= dt * dropSpeed;
				if (x < pos.x){
					x += dt * dropSpeed;
				}
				else if (x > pos.x){
					x -= dt * dropSpeed;
				}
				item->setPosition(x, y);
			}

			//若全部掉落完毕
			if (dropList.size() == 0){
				//音效
				CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/tick.wav");
				//更新Arrived - Fixed
				updateToFixed();
				//再次扫描以发现新的掉落项
				findDropList();
				//无可掉落项，进行步数结束逻辑判断
				if (dropList.size() == 0){
					onStepEnd();
				}
			}
		}

	}

}

void DropController_L::updateToFixed(){
	for (int j = 0; j < getItemBox()->getCellNum().y; j++){
		for (int i = 0; i < getItemBox()->getCellNum().x; i++){

			//搜索Arrived状态项
			Entity* item = getItemBox()->getItem_lgc(i, j);
			if (item->getActionState() == Arrived){
				//若其下方物体为非Clean的固定态，则此项可固定
				Entity* belowItem = getItemBox()->getItem_lgc(i, j - 1);
				if (belowItem == NULL || (belowItem->getActionState() == Fixed && belowItem->getItemType() != ItemType_CLEAN)){
					item->setActionState(Fixed);
					((ItemBox_L*)getItemBox())->resetPos(item);
					//若可消除则发送消除消息
					if (((Scanner_L*)getItemBox()->getScanner())->isClearable(item)){
						Vector<Entity*> list;
						list.pushBack(item);
						PostDate* date = PostDate::create();
						date->setVector(list);
						date->retain();
						NotificationCenter::getInstance()->postNotification("clear", date);
					}
				}
			}
		}
	}
}

void DropController_L::onStepEnd(){

	//解除移动状态
	NotificationCenter::getInstance()->postNotification("moveOver", NULL);

	//发送游戏结束判断消息
	NotificationCenter::getInstance()->postNotification("stepEnd", NULL);
}