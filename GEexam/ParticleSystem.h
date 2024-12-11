#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <cstdlib> 
#include <ctime>
#include "Shader.h"

class ParticleSystem {
public:
    ParticleSystem(int maxParticles, const glm::vec3& boxMin, const glm::vec3& boxMax);

    void update(float deltaTime);
    void render(Shader& shader, const glm::mat4& view, const glm::mat4& projection);
    void setBounds(const glm::vec3& boxMin, const glm::vec3& boxMax);

private:
    void respawnParticle(int index);

    int mMaxParticles;
    glm::vec3 mBoxMin;
    glm::vec3 mBoxMax;

    // Structure of Arrays (SoA) for particle data
    std::vector<glm::vec3> mPositions; 
    std::vector<glm::vec3> mVelocities; 
    std::vector<float> mLifetimes;     
    std::vector<bool> mActive;         

    unsigned int mVAO, mVBO; 
};

