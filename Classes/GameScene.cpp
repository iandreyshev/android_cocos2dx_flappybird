#include "GameScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* GameScene::createScene()
{
	auto scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	scene->getPhysicsWorld()->setGravity(Vect(0, -1200));

	auto layer = GameScene::create();
	layer->SetPhysicsWorld(scene->getPhysicsWorld());

	scene->addChild(layer);

	return scene;
}

bool GameScene::init()
{
	if (!Layer::init())
	{
		return false;
	}
	
	m_log.open("out.txt");

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	Vec2 center = Vec2(visibleSize.width / 2.0f, visibleSize.height / 2.0f);

	m_background.Init(this);
	m_bird.Init(this);

	m_tubesBodies = m_background.GetTubesBodies();
	SetBehavoir(GameBehavior::START);

	auto mapEdge = PhysicsBody::createEdgeBox(visibleSize);
	auto edgeNode = Node::create();
	edgeNode->addComponent(mapEdge);
	edgeNode->setPosition(center);
	this->addChild(edgeNode);

	auto collideListener = EventListenerPhysicsContact::create();
	collideListener->onContactBegin = CC_CALLBACK_1(GameScene::IsBirdFell, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(collideListener, this);

	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->setSwallowTouches(true);
	touchListener->onTouchBegan = CC_CALLBACK_2(GameScene::IsScreenTouched, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

	schedule(schedule_selector(GameScene::GameUpdate));

	return true;
}

void GameScene::SetPhysicsWorld(cocos2d::PhysicsWorld* world)
{
	sceneWorld = world;
}

void GameScene::SetBehavoir(GameBehavior newBehavior)
{
	m_behavior = newBehavior;

	switch (newBehavior)
	{
	case GameBehavior::START:
		m_bird.Reset();
		m_background.Reset();
		break;
	case GameBehavior::GAMEPLAY:
		break;
	case GameBehavior::GAMEOVER:
		m_bird.Freze();
		break;
	default:
		break;
	}
}

bool GameScene::IsBirdFell(PhysicsContact& contact)
{
	if (IsCollideWithGround(contact) || IsCollideWithTube(contact))
	{
		SetBehavoir(GameBehavior::GAMEOVER);
		return true;
	}

	return false;
}

bool GameScene::IsCollideWithGround(cocos2d::PhysicsContact& contact)
{
	PhysicsBody* bodyA = contact.getShapeA()->getBody();
	PhysicsBody* bodyB = contact.getShapeB()->getBody();

	auto birdBody = m_bird.GetBody();
	auto groundBody = m_background.GetGroundBody();

	if ((bodyA == birdBody && bodyB == groundBody) ||
		(bodyA == groundBody && bodyB == birdBody))
	{
		return true;
	}

	return false;
}

bool GameScene::IsCollideWithTube(cocos2d::PhysicsContact& contact)
{
	PhysicsBody* bodyA = contact.getShapeA()->getBody();
	PhysicsBody* bodyB = contact.getShapeB()->getBody();

	auto birdBody = m_bird.GetBody();

	for (auto tubeBody : m_tubesBodies)
	{
		if ((bodyA == birdBody && bodyB == tubeBody) ||
			(bodyA == tubeBody && bodyB == birdBody))
		{
			return true;
		}
	}

	return false;
}

bool GameScene::IsScreenTouched(cocos2d::Touch* touch, cocos2d::Event* event)
{
	switch (m_behavior)
	{
	case GameBehavior::START:
		SetBehavoir(GameBehavior::GAMEPLAY);
		m_bird.Jump();
		break;
	case GameBehavior::GAMEPLAY:
		m_bird.Jump();
		break;
	case GameBehavior::GAMEOVER:
		SetBehavoir(GameBehavior::START);
		break;
	default:
		break;
	}

	return true;
}

void GameScene::GameUpdate(float elapsedTime)
{
	m_bird.Update(elapsedTime);

	switch (m_behavior)
	{
	case GameBehavior::START:
		break;
	case GameBehavior::GAMEPLAY:
		m_background.Update(elapsedTime);
		break;
	case GameBehavior::GAMEOVER:
		break;
	default:
		break;
	}
}