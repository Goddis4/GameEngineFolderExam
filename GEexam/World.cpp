#include "World.h"

//World::World() : mBox(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(50.0f, 10.0f, 50.0f)) // Box at position (0, 0, 0) with size (, , )
//{
// 
//}
//
//
//
//void World::update(float deltaTime) {
//    mBox.update(deltaTime); 
//}
//
//void World::render(Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
//    mBox.render(shader, view, projection); 
//}
//
//void World::addSphereToBox(const Spheres& sphere) {
//    mBox.addSphere(sphere); 
//}


World::World() {
    mWorldBounds.min = glm::vec3(-25.0f, -25.0f, -25.0f);
    mWorldBounds.max = glm::vec3(25.0f, 25.0f, 25.0f);
}

void World::addBox(const glm::vec3& position, const glm::vec3& size) {
    mBox.emplace_back(position, size); // Add a new Box to the world
}

uint32_t World::createSphereEntity(const glm::vec3& position, const glm::vec3& velocity, float radius, glm::vec3 color) {
    // Choose a box to add the sphere to (basic example: first box)
    if (!mBox.empty()) {
        return mBox[0].addSphereEntity(position, velocity, radius, color);
    }

    // If no boxes exist, create the sphere globally (for testing)
    uint32_t entity = mEntityManager.createEntity();

    // Resize components if necessary
    if (entity >= mComponents.transforms.size()) {
        mComponents.resize(entity + 1);
    }

    // Assign components
    mComponents.transforms[entity] = { position, {}, glm::vec3(1.0f) };
    mComponents.physics[entity] = { velocity, 1.0f, radius };
    mComponents.renders[entity] = { 0, 0, 0, color };

    return entity;
}

void World::update(float deltaTime) {
    for (auto& box : mBox) {
        box.update(deltaTime);
    }
}

void World::render(Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
    for (auto& box : mBox) {
        box.render(shader, view, projection);
    }
}


