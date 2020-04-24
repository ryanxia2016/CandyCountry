#include "MenuLayer_L.h"
#include "MainScene.h"
#include "SimpleAudioEngine.h"

MenuLayer_L::MenuLayer_L()
{
}

MenuLayer_L::~MenuLayer_L()
{
}

bool MenuLayer_L::init(){
	if (!Layer::init()){
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();

	//�˳���ť
	Button* exitBtn = Button::create("menu/exitBtn.png");
	exitBtn->setPosition(Point(visibleSize.width * 0.9, visibleSize.height * 0.1));
	exitBtn->addTouchEventListener(this, toucheventselector(MenuLayer_L::exitBtn));
	this->addChild(exitBtn);

	return true;
}

void MenuLayer_L::exitBtn(Ref*, TouchEventType type){
	switch (type)
	{
	case TouchEventType::TOUCH_EVENT_ENDED:
		//������Ч
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/btnClick.wav");
		//�ر�����
		CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(0);
		CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(0);
		//�ڵ�ͼ��
		LayerColor* lc = LayerColor::create(Color4B(0, 0, 0, 230));
		this->addChild(lc, 10, 10);
		//���²�ͼ������
		auto listener = EventListenerTouchOneByOne::create();
		listener->onTouchBegan = [&](Touch* touch, Event* event){
			return true;
		};
		listener->setSwallowTouches(true);
		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, lc);
		//�Ի��򱳾�
		Size visibelSize = Director::getInstance()->getVisibleSize();
		Sprite* exitBG = Sprite::create("menu/exitBG.png");
		exitBG->setPosition(visibelSize.width * 0.5, visibelSize.height * 0.5);
		lc->addChild(exitBG);
		//�Ի���ť
		Button* exitSureBtn = Button::create("menu/exitSureBtn.png");
		exitSureBtn->setPosition(Point(exitBG->getContentSize().width * 0.3, exitBG->getContentSize().height * 0.33));
		exitSureBtn->addTouchEventListener(this, toucheventselector(MenuLayer_L::exitSureBtn));
		exitBG->addChild(exitSureBtn);

		Button* exitCancelBtn = Button::create("menu/exitCancelBtn.png");
		exitCancelBtn->setPosition(Point(exitBG->getContentSize().width * 0.7, exitBG->getContentSize().height * 0.33));
		exitCancelBtn->addTouchEventListener(this, toucheventselector(MenuLayer_L::exitCancelBtn));
		exitBG->addChild(exitCancelBtn);

		break;
	}
}

void MenuLayer_L::exitSureBtn(Ref*, TouchEventType type){
	switch (type)
	{
	case TouchEventType::TOUCH_EVENT_ENDED:
		//�ָ�����
		CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(1);
		CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(1);

		Director::getInstance()->replaceScene(MainScene::createScene());
		break;
	}
}

void MenuLayer_L::exitCancelBtn(Ref*, TouchEventType type){
	switch (type)
	{
	case TouchEventType::TOUCH_EVENT_ENDED:
		//�ָ�����
		CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(1);
		CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(1);

		this->removeChildByTag(10);
		break;
	}
}