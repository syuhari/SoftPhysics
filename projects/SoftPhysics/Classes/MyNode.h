//
//  MyNode.cpp
//  SoftPhysics
//
//  Created by Akihiro Matsuura on 5/15/14.
//  Copyright (c) 2014 Syuhari, Inc. All rights reserved.
//

#ifndef __SoftBody__MyNode__
#define __SoftBody__MyNode__

#include "cocos2d.h"
#include "Box2D.h"
#include "CCGL.h"

typedef struct {
    GLfloat x;
    GLfloat y;
} Vertex2D;

static inline Vertex2D Vertex2DMake(GLfloat x, GLfloat y) {
    Vertex2D vertex;
    vertex.x = x;
    vertex.y = y;
    return vertex;
}

#define NUM_SEGMENTS 12

class MyNode : public cocos2d::CCNode {
public:
    MyNode();
    virtual ~MyNode();
    bool init();
    void createPhysicsObject(b2World *world);
    virtual void draw(void);
    void bounce();
    CREATE_FUNC(MyNode);
private:
    // Texture
    cocos2d::CCTexture2D* texture;

    // Physics bodies
    std::vector<b2Body*> bodies;
    float deltaAngle;

    // Center circle
    b2Body *innerCircleBody;

    // Polygon vertices
    Vertex2D triangleFanPos[NUM_SEGMENTS+2];

    // Texture coordinate array
    Vertex2D textCoords[NUM_SEGMENTS+2];
};

#endif /* defined(__SoftBody__MyNode__) */
