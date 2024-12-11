#pragma once
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include "EntityManager.h"

struct TransformComponent {
    glm::vec3 position{ 0.0f };
    glm::vec3 rotation{ 0.0f };
    glm::vec3 scale{ 1.0f };
};

struct PhysicsComponent {
    glm::vec3 velocity{ 0.0f };
    float mass = 1.0f;
    float radius = 1.0f;
};

struct RenderComponent {
    uint32_t vao = 0;
    uint32_t vbo = 0;
    uint32_t ebo = 0;
    glm::vec3 color{ 1.0f, 0.0f, 0.0f }; 
    float radius = 1.0f;
    size_t vertexCount = 0;
};

struct WorldBoundsComponent {
    glm::vec3 min{ -25.0f };
    glm::vec3 max{ 25.0f };
};

// Component Arrays (using Struct of Arrays approach)
struct ComponentArrays {
    // Aligned storage for components
    std::vector<TransformComponent> transforms;
    std::vector<PhysicsComponent> physics;
    std::vector<RenderComponent> renders;

    // Mapping between entity IDs and component indices
    std::vector<uint32_t> entityToTransformIndex;
    std::vector<uint32_t> entityToPhysicsIndex;
    std::vector<uint32_t> entityToRenderIndex;

    void resize(size_t newSize) {
        transforms.resize(newSize);
        physics.resize(newSize);
        renders.resize(newSize);
        entityToTransformIndex.resize(newSize);
        entityToPhysicsIndex.resize(newSize);
        entityToRenderIndex.resize(newSize);
    }
};