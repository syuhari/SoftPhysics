#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Box2D/Box2D.h"
#include "MyNode.h"

class HelloWorld : public cocos2d::CCLayer
{
private:
    b2World *world;
    MyNode *node;
public:
    HelloWorld();
    virtual ~HelloWorld();
    virtual bool init();  
    static cocos2d::CCScene* scene();
    CREATE_FUNC(HelloWorld);
    virtual void update(float dt);
    virtual void ccTouchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
};

#endif // __HELLOWORLD_SCENE_H__
