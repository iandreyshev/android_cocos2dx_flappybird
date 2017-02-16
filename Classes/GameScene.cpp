#include "GameScene.h"

USING_NS_CC;

const float WORLD_GRAVITY = 2400;

Scene* GameScene::createScene()
{
	auto scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setGravity(Vect(0, -WORLD_GRAVITY));

	auto layer = GameScene::create();
	scene->addChild(layer);

	return scene;
}

bool GameScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	this->isKeypadEnabled();

	auto winSize = Director::getInstance()->getVisibleSize();
	Vec2 center = Vec2(winSize * 0.5f);

	m_bird = new Bird();
	m_map = new GameMap();
	m_interface = new GameInterface();

	m_bird->init();
	m_map->init();
	m_interface->init();

	AddListeners();
	scheduleUpdate();

	SetBehavoir(GameBehavior::START);

	this->addChild(m_map);
	this->addChild(m_bird);
	this->addChild(m_interface);

	return true;
}

void GameScene::AddListeners()
{
	auto collideListener = EventListenerPhysicsContact::create();
	collideListener->onContactBegin = CC_CALLBACK_1(GameScene::IsBirdCollideAny, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(collideListener, this);

	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->setSwallowTouches(true);
	touchListener->onTouchBegan = CC_CALLBACK_2(GameScene::HandleTouch, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

	auto pKeybackListener = EventListenerKeyboard::create();
	pKeybackListener->onKeyReleased = CC_CALLBACK_2(GameScene::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(pKeybackListener, this);
}

void GameScene::update(float elapsedTime)
{
	float unreachableHeight = Director::getInstance()->getVisibleSize().height;
	float birdHeight = m_bird->GetPosition().y;

	m_interface->Update(m_bird->GetPosition());

	if (m_interface->GetPointsCount() == POINTS_MAX ||
		birdHeight > unreachableHeight ||
		birdHeight < 0)
	{
		SetBehavoir(GameBehavior::GAMEOVER);
	}
}

bool GameScene::HandleTouch(Touch* touch, Event* event)
{
	switch (m_behavior)
	{
	case GameBehavior::START:
		m_bird->Jump();
		SetBehavoir(GameBehavior::GAMEPLAY);
		break;

	case GameBehavior::GAMEPLAY:
		m_bird->Jump();
		break;

	case GameBehavior::GAMEOVER:
		if (m_interface->IsGameoverInit())
		{
			SetBehavoir(GameBehavior::START);
		}
		break;
	default:
		break;
	}

	return true;
}

void GameScene::SetBehavoir(GameBehavior newBehavior)
{
	m_behavior = newBehavior;

	switch (newBehavior)
	{
	case GameBehavior::START:
		m_bird->Reset();
		m_map->Reset();
		m_interface->Reset();
		break;

	case GameBehavior::GAMEPLAY:
		m_interface->SetGameplayUI();
		m_map->StartMotion();
		break;

	case GameBehavior::GAMEOVER:
		m_interface->SetGameoverUI();
		m_bird->Death();
		m_map->StopMotion();
		break;
	default:
		break;
	}
}

bool GameScene::IsBirdCollideAny(PhysicsContact& contact)
{
	auto bitMaskA = contact.getShapeA()->getCollisionBitmask();
	auto bitMaskB = contact.getShapeB()->getCollisionBitmask();

	if ((bitMaskA == MAP_BITMASK && bitMaskB == BIRD_BITMASK) ||
		(bitMaskA == BIRD_BITMASK && bitMaskB == MAP_BITMASK))
	{
		SetBehavoir(GameBehavior::GAMEOVER);
		return true;
	}
	else if ((bitMaskA == BIRD_BITMASK && bitMaskB == POINT_BITMASK) ||
			(bitMaskA == POINT_BITMASK && bitMaskB == BIRD_BITMASK))
	{
		m_interface->AddPoint();
		return false;
	}

	return false;
}

void GameScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event *pEvent)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
	{
		Director::getInstance()->end();
	}
}