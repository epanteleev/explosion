#pragma once
#include <Box2D/Box2D.h>
#include "Testbed/Framework/Render.h"
#include <GL/glut.h>
#include <cstdlib>
#include "Testbed/Framework/Test.h"
#include <vector>

#define MAX_BLAST_RAYS 1200

namespace {
//const float m_blastRadius = 10000;
const float m_blastPower = 100;
const size_t g_numRays = 400;
}
class bomb{
public:
    bomb(b2World* m_world, b2Vec2& velosity, b2Vec2& position):world(m_world){
        b2BodyDef bd;
        bd.type = b2BodyType(2);
        bd.position = position;
        bd.linearVelocity = velosity;
        bd.allowSleep = bool(4);
        bd.awake = bool(2);
        bd.fixedRotation = bool(0);
        bd.bullet = bool(0);
        bd.active = bool(32);
        body = m_world->CreateBody(&bd);
        b2FixtureDef fd;
        fd.friction = 0.5;
        fd.restitution = 0.95f;
        fd.density = 1.0;
        fd.isSensor = bool(0);
        fd.filter.categoryBits = uint16(1);
        fd.filter.maskBits = uint16(65535);
        fd.filter.groupIndex = int16(-1);
        b2CircleShape shape;
        shape.m_radius = 0.25f;
        shape.m_p.Set(0.0, 0.0);

        fd.shape = &shape;

        body->CreateFixture(&fd);
     }
    void clear(){
        for (int i = 0; i < g_numRays; i++) {
            if ( m_blastParticleBodies[i] != nullptr) {
                world->DestroyBody(m_blastParticleBodies[i]);
                m_blastParticleBodies[i] = nullptr;
            }
        }
        for (size_t k = 0; k < m_previousParticlePositions.size(); k++){
            delete[] m_previousParticlePositions[k];
        }
        m_previousParticlePositions.clear();
    }
    void display_particle(){
        glEnable(GL_BLEND);
        if ( m_previousParticlePositions.empty() ) {
            return;
        }
        for (size_t i = 0; i <  g_numRays; i++) {
            if (not m_blastParticleBodies[i] ) {
                continue;
            }
            glBegin(GL_LINES);
            size_t k = m_previousParticlePositions.size() - 1;
            float alpha = static_cast<float>(k) / m_previousParticlePositions.size();
            glColor4f(1,0,0, alpha);
            glVertex2fv( (GLfloat*)&(m_previousParticlePositions[k][i]) );
            glColor4f(1,1,0,alpha);
            b2Vec2 currentPos = m_blastParticleBodies[i]->GetPosition();
            glVertex2fv( (GLfloat*)&currentPos );
            glEnd();
        }
    }
    void display_trajectory(){
        glBegin(GL_LINES);
        for (size_t i = 0; i <  g_numRays; i++) {
            for (size_t k = 1; k < m_previousParticlePositions.size(); k++) {
                float alpha = static_cast<float>(k-1) / m_previousParticlePositions.size();
                glColor4f(1,0,0, alpha);
                glVertex2fv( (GLfloat*)&(m_previousParticlePositions[k-1][i]) );
                alpha = static_cast<float>(k) / m_previousParticlePositions.size();
                glColor4f(1,1,0, alpha);
                glVertex2fv( (GLfloat*)&(m_previousParticlePositions[k][i]) );
            }
        }
        glEnd();
    }
    void explosion(){
        b2Vec2 center = body->GetPosition();
        b2BodyDef bd;
        bd.fixedRotation = true;
        bd.bullet = true;
        bd.linearDamping = 1.0;
        bd.position = center;

        bd.type = b2_dynamicBody;
        for (size_t i = 0; i < g_numRays; i++) {
            float angle = static_cast<float>(rand());
            float k = sinf(static_cast<float>(rand()));
            b2Vec2 rayDir( k * sinf(angle), k * cosf(angle) );

            bd.linearVelocity = m_blastPower * rayDir;
            b2Body* body = world->CreateBody(&bd);

            b2CircleShape circleShape;
            if(i < g_numRays / 2){
                circleShape.m_radius = 0.05f;
            }else{
                circleShape.m_radius = 0.2f;
            }
            b2FixtureDef fd;
            fd.shape = &circleShape;
            fd.density = 60.f / static_cast<float>(g_numRays);
            fd.restitution = 0.0f;
            fd.friction = 0.0;
            fd.filter.groupIndex = -1;
            body->CreateFixture(&fd);

            m_blastParticleBodies[i] = body;
        }
        explode = false;
    }
    void update(){
        if (explode) {
            return;
        }
        b2Vec2* prevPositions = new b2Vec2[g_numRays];
        for (int i = 0; i < g_numRays; i++) {
            if (m_blastParticleBodies[i] != nullptr){
                prevPositions[i] = m_blastParticleBodies[i]->GetPosition();
            }
        }
        m_previousParticlePositions.push_back(prevPositions);

    }

    b2Body* m_blastParticleBodies[MAX_BLAST_RAYS]{};
    std::vector<b2Vec2*> m_previousParticlePositions;
    bool explode = true;
    b2Body* body{};
    b2World* world{};
};
