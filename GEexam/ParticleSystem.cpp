#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(int maxParticles, const glm::vec3& boxMin, const glm::vec3& boxMax)
    : mMaxParticles(maxParticles), mBoxMin(boxMin), mBoxMax(boxMax) {
    // Seed random number generator
    std::srand(static_cast<unsigned>(std::time(0)));

    // Initialize particle data
    mPositions.resize(maxParticles);
    mVelocities.resize(maxParticles);
    mLifetimes.resize(maxParticles);
    mActive.resize(maxParticles, true);

    for (int i = 0; i < maxParticles; ++i) {
        respawnParticle(i);
    }

    // OpenGL setup for particle rendering
    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);

    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mPositions.size() * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void ParticleSystem::update(float deltaTime) {
    for (int i = 0; i < mMaxParticles; ++i) {
        if (!mActive[i]) continue;

        // Update position
        mPositions[i] += mVelocities[i] * deltaTime;

        // Check if the particle has reached the floor of the box
        if (mPositions[i].y <= mBoxMin.y) {
            respawnParticle(i);
        }
    }

    // Update OpenGL buffer
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, mPositions.size() * sizeof(glm::vec3), mPositions.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleSystem::render(Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
    shader.use();

    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    shader.setVec3("objectColor", glm::vec3(0.0f, 0.5f, 1.0f)); 
    shader.setBool("useFlatColor", true);

    glPointSize(2.0f);
    glBindVertexArray(mVAO);
    glDrawArrays(GL_POINTS, 0, mMaxParticles);
    glBindVertexArray(0);

    shader.setBool("useFlatColor", false);
}

void ParticleSystem::respawnParticle(int index) {
    // Random x and z within the box bounds
    float x = mBoxMin.x + static_cast<float>(std::rand()) / RAND_MAX * (mBoxMax.x - mBoxMin.x);
    float z = mBoxMin.z + static_cast<float>(std::rand()) / RAND_MAX * (mBoxMax.z - mBoxMin.z);

    // Spawn at random height slightly above the box
    float y = mBoxMax.y + static_cast<float>(std::rand()) / RAND_MAX * 2.0f;

    // Random downward velocity
    float velocityY = -1.0f - static_cast<float>(std::rand()) / RAND_MAX * 2.0f;


    mPositions[index] = glm::vec3(x, y, z);
    mVelocities[index] = glm::vec3(0.0f, velocityY, 0.0f);
    mLifetimes[index] = 5.0f; // lifetime
    mActive[index] = true;
}

void ParticleSystem::setBounds(const glm::vec3& boxMin, const glm::vec3& boxMax) {
    mBoxMin = boxMin;
    mBoxMax = boxMax;
}
