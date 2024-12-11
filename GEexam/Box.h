#pragma once
#include "Shader.h"
#include "Spheres.h"
#include "CollideSpheres.h"
#include "ParticleSystem.h"
#include "EntityManager.h"
#include "SystemManager.h"
#include "ComponentManager.h"
#include <vector>
#include <glm/glm.hpp>

class Box {
public:
    Box(const glm::vec3& position, const glm::vec3& size);

   
    uint32_t addSphereEntity(const glm::vec3& position, const glm::vec3& velocity, float radius, const glm::vec3& color);
    void update(float deltaTime);
    void render(Shader& shader, const glm::mat4& view, const glm::mat4& projection);

private:
    glm::vec3 mPosition; 
    glm::vec3 mSize; 

    
    CollideSpheres mCollideSpheres; 
    ParticleSystem mParticleSystem; 

    GLuint mVAO, mVBO, mEBO;
    void makingBox(); 
};

//class Box {
//public:
//    Box(const glm::vec3& position, const glm::vec3& size);
//
//    void addSphere(const Spheres& sphere); 
//    void update(float deltaTime);
//    void render(Shader& shader, const glm::mat4& view, const glm::mat4& projection);
//
//    glm::vec3 getPosition() const { return mPosition; }
//    glm::vec3 getSize() const { return mSize; }
//
//private:
//    glm::vec3 mPosition; // Position of the box
//    glm::vec3 mSize;     // Dimensions of the box (width, height, depth)
//    
//    CollideSpheres mCollideSpheres;
//    ParticleSystem mParticleSystem;
//
//    unsigned int mVAO, mVBO, mEBO; 
//    void makingBox();  
//    
//};
