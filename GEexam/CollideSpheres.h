//#pragma once
//#include "Shader.h"
//#include "Spheres.h"
//#include <vector>
//#include <glm/glm.hpp>
//#include <glm/gtx/string_cast.hpp>
//
//class CollideSpheres {
//public:
//    CollideSpheres(const glm::vec3& boxPosition, const glm::vec3& boxSize);
//
//    void addSphere(const Spheres& sphere);
//    
//    void update(float deltaTime);
//    void render(Shader& shader, const glm::mat4& view, const glm::mat4& projection);
//    void checkBallCollision();
//    float detectCollision(const Spheres& s1, const Spheres& s2, float deltaTime); // Detects collision time
//    void resolveCollision(Spheres& s1, Spheres& s2);
//    void checkWallCollision(Spheres& sphere, const glm::vec3& boxMin, const glm::vec3& boxMax);
//
//private:
//    glm::vec3 mBoxPosition; // Position of the containing box
//    glm::vec3 mBoxSize;     // Size of the containing box
//    std::vector<Spheres> mSpheres; // List of spheres within this object
//
//    
//};

#pragma once
#include "Shader.h"
#include "EntityManager.h"
#include "SystemManager.h"
#include <glm/glm.hpp>

class CollideSpheres {
public:
    CollideSpheres(const glm::vec3& boxPosition, const glm::vec3& boxSize);

    uint32_t addSphere(const glm::vec3& position, const glm::vec3& velocity, float radius, const glm::vec3& color);

    void printAllEntities();
    void update(float deltaTime);
    void render(Shader& shader, const glm::mat4& view, const glm::mat4& projection);
    void removeEntity(uint32_t entity);
    uint32_t createSphereVAO(float radius, size_t& outVertexCount);


    std::vector<uint32_t> mSphereEntities;
private:
    glm::vec3 mBoxPosition; 
    glm::vec3 mBoxSize;     

    EntityManager mEntityManager;       // Manages entity IDs
    ComponentArrays mComponents;       // Stores components for entities in this box

    WorldBoundsComponent mWorldBounds; 

     
};
