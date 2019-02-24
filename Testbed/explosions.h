#ifndef EXPLOSIONS_H
#define EXPLOSIONS_H

#ifndef DEGTORAD
#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#endif

#include <vector>

#include "Framework/Test.h"
#include "Framework/Render.h"
#include <GL/glut.h>
#include <cstring>
#include <ctime>
#include <random>
#include <cmath>

#include "bomb.h"

namespace
{
const float HEIGHT = 70.0;
bool g_pauseAfterBlast = false;

}

class iforce2d_Explosions: public Test{
public:
    iforce2d_Explosions()
    {
        srand(static_cast<unsigned int>(time(nullptr)));
        b2Vec2 g(0.0f, -10.0f);
        m_world->SetGravity(g);
        b2Body* bodies{};
        {
          b2BodyDef bd;
          bd.type = b2BodyType(0);
          bd.position.Set(0.0f, 0.0f);
          bd.angle = 0.0f;
          bd.linearVelocity.Set(0.0f, 0.0f);
          bd.angularVelocity = 0.0f;
          bd.linearDamping = 0.0f;
          bd.angularDamping = 0.0f;
          bd.allowSleep = bool(4);
          bd.awake = bool(2);
          bd.fixedRotation = bool(0);
          bd.bullet = bool(0);
          bd.active = bool(32);
          bodies = m_world->CreateBody(&bd);
        }
        {
          b2FixtureDef fd;
          fd.friction = 0.2f;
          fd.restitution = 0.0f;
          fd.density = 1.0f;
          fd.isSensor = bool(0);
          fd.filter.categoryBits = uint16(1);
          fd.filter.maskBits = uint16(65535);
          fd.filter.groupIndex = int16(0);
          b2PolygonShape shape;
          b2Vec2 vs[8];
          vs[0].Set(30.e+01f, -1.e+01f);
          vs[1].Set(30.e+01f, 0.0e+01f);
          vs[2].Set(-30.e+01f, 0.0e+01f);
          vs[3].Set(-30.e+01f, -1.e+01f);
          shape.Set(vs, 4);

          fd.shape = &shape;

          bodies->CreateFixture(&fd);
        }
        b2Vec2 velosity(0.0,100.0);
        b2Vec2 position(0.0,0.0);

        bombs.push_back(bomb (m_world,velosity,position));

        m_shouldPause = false;

        glEnable(GL_POINT_SMOOTH);
    }
    ~iforce2d_Explosions(){
        for(auto& i: bombs){
            i.clear();
        }
    }

    void Keyboard(unsigned char key)
    {
        switch (key) {
        case 'b' :  {
            b2Vec2 velosity(0.0,100.0);
            b2Vec2 position(10.0f - static_cast<float>(20 & rand()), 0.0);
            bombs.push_back(bomb (m_world, velosity,position));
            break;
        }
        default: Test::Keyboard(key);
        }
    }

    void Step(Settings* settings){
        m_debugDraw.DrawString(5, m_textLine, "Press b to go bang");

        if ( m_shouldPause ) {
            settings->pause = true;
            m_shouldPause = false;
        }
        Test::Step(settings);
        for(auto& i : bombs){
            if(i.body->GetPosition().y > HEIGHT && i.explode){
                i.explosion();
                m_world->DestroyBody(i.body);
            }
            i.update();
            i.display_particle();
            if(settings->showTrajectory == 0){
                i.display_trajectory();
            }
        }
    }

    static Test* Create()
    {
        return new iforce2d_Explosions;
    }
    b2Body* m_grenadeBody{};
    bool m_shouldPause;

    std::vector<bomb> bombs;
};

#endif
