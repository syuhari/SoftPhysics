#include "HelloWorldScene.h"

#define PTM_RATIO 32.f

USING_NS_CC;

CCScene* HelloWorld::scene()
{
    CCScene *scene = CCScene::create();
    HelloWorld *layer = HelloWorld::create();
    scene->addChild(layer);
    return scene;
}

HelloWorld::HelloWorld()
{
    
}

HelloWorld::~HelloWorld()
{
    delete world;
    world = NULL;
}

bool HelloWorld::init()
{
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    b2Vec2 gravity;
    gravity.Set(0.0f, -10.0f);
    world = new b2World(gravity);
    world->SetAllowSleeping(true);
    
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0, 0);
    b2Body* groundBody = world->CreateBody(&groundBodyDef);
    b2EdgeShape groundBox;
    
    // top
    groundBox.Set(b2Vec2(0,s.height/PTM_RATIO), b2Vec2(s.width/PTM_RATIO,s.height/PTM_RATIO));
    groundBody->CreateFixture(&groundBox,0);
    
    // left
    groundBox.Set(b2Vec2(0,s.height/PTM_RATIO), b2Vec2(0,0));
    groundBody->CreateFixture(&groundBox,0);
    
    // right
    groundBox.Set(b2Vec2(s.width/PTM_RATIO,s.height/PTM_RATIO), b2Vec2(s.width/PTM_RATIO,0));
    groundBody->CreateFixture(&groundBox,0);
    
    // bottom ground
    groundBox.Set(b2Vec2(0,0), b2Vec2(s.width/PTM_RATIO,0));
    groundBody->CreateFixture(&groundBox,0);
    
    setTouchEnabled(true);
    
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    node = MyNode::create();
    node->retain();
    node->setPosition(ccp(size.width/2, size.height/2));
    node->createPhysicsObject(world);
    addChild(node);
    
    this->scheduleUpdate();
    
    return true;
}

void HelloWorld::update(float dt) {
    world->Step(dt, 8, 1);
}

void HelloWorld::ccTouchesEnded(cocos2d::CCSet *touches, cocos2d::CCEvent *event) {
    node->bounce();
}
