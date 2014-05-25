//
//  MyNode.cpp
//  SoftPhysics
//
//  Created by Akihiro Matsuura on 5/15/14.
//  Copyright (c) 2014 Syuhari, Inc. All rights reserved.
//
//

#include "MyNode.h"

#define PTM_RATIO 32.f

using namespace cocos2d;

MyNode::MyNode()
:bodies(NULL)
,innerCircleBody(NULL)
{

}

MyNode::~MyNode() {
    bodies.clear();
    innerCircleBody = NULL;
}

bool MyNode::init() {
    
    if (!CCNode::init()) {
        return false;
    }
    
    texture = CCTextureCache::sharedTextureCache()->addImage("football.png");
    
    return true;
}

void MyNode::createPhysicsObject(b2World *world) {
    // Center is the position of the circle that is in the center (inner circle)
    b2Vec2 center = b2Vec2(this->getPositionX() / PTM_RATIO, this->getPositionY() / PTM_RATIO);
    b2CircleShape circleShape;
    circleShape.m_radius = 0.1f;
    
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circleShape;
    fixtureDef.density = 0.1;
    fixtureDef.restitution = 0.05;
    fixtureDef.friction = 1.0;
    
    // Delta angle to step by
    deltaAngle = (2.f * M_PI) / NUM_SEGMENTS;

    // Radius of the wheel
    float radius = 50;
    
    // Need to store the bodies so that we can refer back
    // to it when we connect the joints
    //bodies = CCArray::create();
    //bodies->retain();
    bodies.clear();
    
    for (int i = 0; i < NUM_SEGMENTS; i++) {
        // Current angle
        float theta = deltaAngle * i;
        
        // Calculate x and y based on theta
        float x = radius * cosf(theta);
        float y = radius * sinf(theta);
        
        // Remember to divide by PTM_RATIO to convert to Box2d coordinate
        b2Vec2 circlePosition = b2Vec2(x / PTM_RATIO, y / PTM_RATIO);
        
        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        // Position should be relative to the center
        bodyDef.position = (center + circlePosition);
        
        // Create the body and fixture
        b2Body *body = world->CreateBody(&bodyDef);
        body->CreateFixture(&fixtureDef);
        
        // Add the body to the array to connect joints to it
        // later. b2Body is a C++ object, so must wrap it
        // in NSValue when inserting into it NSMutableArray
        bodies.push_back(body);
    }
    
    // Circle at the center (inner circle)
    b2BodyDef innerCircleBodyDef;
    // Make the inner circle larger
    circleShape.m_radius = 0.8f;
    innerCircleBodyDef.type = b2_dynamicBody;

    // Position is at the center
    innerCircleBodyDef.position = center;
    innerCircleBody = world->CreateBody(&innerCircleBodyDef);
    innerCircleBody->CreateFixture(&fixtureDef);
    
    // Connect the joints
    b2DistanceJointDef jointDef;
    for (int i = 0; i < NUM_SEGMENTS; i++) {
        // The neighbor
        int neighborIndex = (i + 1) % NUM_SEGMENTS;
        
        // Get current body and neighbor
        b2Body* currentBody = bodies.operator[](i);
        b2Body* neighborBody = bodies.operator[](neighborIndex);
        
        // Connect the outer circles to each other
        jointDef.Initialize(currentBody, neighborBody,
                            currentBody->GetWorldCenter(),
                            neighborBody->GetWorldCenter() );
        // Specifies whether the two connected bodies should collide with each other
        jointDef.collideConnected = true;
        jointDef.frequencyHz = 0.0f;
        jointDef.dampingRatio = 0.0f;
        
        world->CreateJoint(&jointDef);
        
        // Connect the center circle with other circles
        jointDef.Initialize(currentBody, innerCircleBody, currentBody->GetWorldCenter(), center);
        jointDef.collideConnected = true;
        jointDef.frequencyHz = 8.0f;
        jointDef.dampingRatio = 0.5f;

        world->CreateJoint(&jointDef);
        
    }

}

void MyNode::draw() {
    CCPoint tfp[NUM_SEGMENTS + 2];
    tfp[0] = ccp(innerCircleBody->GetPosition().x * PTM_RATIO - this->getPosition().x,
                 innerCircleBody->GetPosition().y * PTM_RATIO - this->getPosition().y);
    
    for (int i = 0; i < NUM_SEGMENTS; i++) {
        b2Body* currentBody = bodies.operator[](i);
        tfp[i + 1] = ccp(currentBody->GetPosition().x * PTM_RATIO - this->getPosition().x,
                         currentBody->GetPosition().y * PTM_RATIO - this->getPosition().y);
    }
    tfp[NUM_SEGMENTS + 1] = tfp[1];
    
    // Using the wheel defined by the box2d objects, we'll be mapping a triangle on
    // top of it using a triangle fan. First, we calculate the center. The center
    // needs to be mulitplied by the PTM_RATIO (to get the pixel coordinate from box2d coordinate)
    // and also must be offset by the current position (remember, in HelloWorldLayer, we set
    // the position to the center of the screen (myNode.position = ccp(240, 160).
    triangleFanPos[0] = Vertex2DMake(innerCircleBody->GetPosition().x * PTM_RATIO - this->getPositionX(),
                                     innerCircleBody->GetPosition().y * PTM_RATIO - this->getPositionY());
    // Use each box2d body as a vertex and calculate coordinate for the triangle fan
    for (int i = 0; i < NUM_SEGMENTS; i++) {
        b2Body* currentBody = bodies.operator[](i);
        Vertex2D pos = Vertex2DMake(currentBody->GetPosition().x * PTM_RATIO - this->getPositionX(),
                                    currentBody->GetPosition().y * PTM_RATIO - this->getPositionY());
        triangleFanPos[i+1] = Vertex2DMake(pos.x, pos.y);
    }
    // Loop back to close off the triangle fan
    triangleFanPos[NUM_SEGMENTS+1] = triangleFanPos[1];
    
    // The first vertex is the center of the triangle fan.
    // So the first coordinate of the texture map should
    // map to the center of the triangle fan. The texture
    // map coordinates are normalized between 0 and 1, so
    // the center is (0.5. 0.5)
    textCoords[0] = Vertex2DMake(0.5f, 0.5f);
    for (int i = 0; i < NUM_SEGMENTS; i++) {
        
        GLfloat theta = M_PI + (deltaAngle * i);
        
        // Calculate the X and Y coordinates for texture mapping.
        // Need to normalize the cosine and sine functions so that the
        // values are between 0 and 1. Cosine and sine functions have
        // values from -1 to 1, so we divide by 2 and add 0.5 to it to
        // normalize the values between 0 and 1.
        textCoords[i+1] = Vertex2DMake(0.5+cosf(theta)*0.5,
                                       0.5+sinf(theta)*0.5);
        
    }
    // Close it off.
    textCoords[NUM_SEGMENTS+1] = textCoords[1];
    
    kmGLPushMatrix();
    //ccDrawSolidPoly(tfp, NUM_SEGMENTS + 2, ccc4f(1.0f, 0.f, 0.f, 1.0f));
    
    ccGLBindTexture2D(texture->getName());
    texture->getShaderProgram()->use();
    texture->getShaderProgram()->setUniformsForBuiltins();
    ccGLEnableVertexAttribs(kCCVertexAttribFlag_Position|kCCVertexAttribFlag_TexCoords);
    glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, triangleFanPos);
    glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, 0, textCoords);
    glDrawArrays(GL_TRIANGLE_FAN, 0, NUM_SEGMENTS+2);
    kmGLPopMatrix();

}

void MyNode::bounce() {
    b2Vec2 impulse = b2Vec2(innerCircleBody->GetMass() * 0, innerCircleBody->GetMass() * 40);
    b2Vec2 impulsePoint = innerCircleBody->GetPosition();
    innerCircleBody->ApplyLinearImpulse(impulse, impulsePoint);
}
