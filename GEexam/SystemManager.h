#pragma once
#include <vector>
#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "EntityManager.h"
#include "ComponentManager.h"


class CollisionSystem {
public:
    static void updateWorldBoundCollisions(
        ComponentArrays& components,
        const WorldBoundsComponent& bounds
    ) {
        for (size_t i = 0; i < components.physics.size(); ++i) {
            auto& transform = components.transforms[i];
            auto& physics = components.physics[i];

            for (int axis = 0; axis < 3; ++axis) {
                if (transform.position[axis] - physics.radius < bounds.min[axis]) {
                    transform.position[axis] = bounds.min[axis] + physics.radius;
                    physics.velocity[axis] = std::abs(physics.velocity[axis]);
                }
                else if (transform.position[axis] + physics.radius > bounds.max[axis]) {
                    transform.position[axis] = bounds.max[axis] - physics.radius;
                    physics.velocity[axis] = -std::abs(physics.velocity[axis]);
                }
            }
        }
    }
    static void updateInterEntityCollisions(ComponentArrays& components) {
        for (size_t i = 0; i < components.physics.size(); ++i) {
            for (size_t j = i + 1; j < components.physics.size(); ++j) {
                if (detectCollision(components.transforms[i], components.physics[i],
                    components.transforms[j], components.physics[j])) {
                    resolveCollision(components.transforms[i], components.physics[i],
                        components.transforms[j], components.physics[j]);
                }
            }
        }
    }

private:
    static bool detectCollision(
        const TransformComponent& transform1, const PhysicsComponent& physics1,
        const TransformComponent& transform2, const PhysicsComponent& physics2
    ) {
        glm::vec3 deltaPos = transform1.position - transform2.position;
        float distance = glm::length(deltaPos);
        return distance < (physics1.radius + physics2.radius);
    }

    static void resolveCollision(
        TransformComponent& transform1, PhysicsComponent& physics1,
        TransformComponent& transform2, PhysicsComponent& physics2
    ) {
        glm::vec3 normal = glm::normalize(transform1.position - transform2.position);
        float m1 = physics1.mass, m2 = physics2.mass;
        glm::vec3 v1 = physics1.velocity, v2 = physics2.velocity;

        float v1n = glm::dot(v1, normal);
        float v2n = glm::dot(v2, normal);

        float v1nPrime = (m1 - m2) / (m1 + m2) * v1n + (2 * m2) / (m1 + m2) * v2n;
        float v2nPrime = (m2 - m1) / (m1 + m2) * v2n + (2 * m1) / (m1 + m2) * v1n;

        physics1.velocity += (v1nPrime - v1n) * normal;
        physics2.velocity += (v2nPrime - v2n) * normal;
    }
};



class PhysicsSystem {
public:
    static void update(ComponentArrays& components, float deltaTime) {
        for (size_t i = 0; i < components.physics.size(); ++i) {
            auto& transform = components.transforms[i];
            const auto& physics = components.physics[i];

            transform.position += physics.velocity * deltaTime;
        }
    }
};



class RenderSystem {
public:
    void render(ComponentArrays& components, Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
        for (size_t i = 0; i < components.renders.size(); ++i) {
            const auto& transform = components.transforms[i];
            const auto& render = components.renders[i];

            shader.use();
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, transform.position);
            model = glm::scale(model, glm::vec3(transform.scale) * render.radius);  // Add radius scaling

            shader.setMat4("model", model);
            shader.setMat4("view", view);
            shader.setMat4("projection", projection);
            shader.setVec3("objectColor", render.color);

            glBindVertexArray(render.vao);
            // Use correct number of indices for sphere
            glDrawArrays(GL_TRIANGLES, 0, render.vertexCount);
            glBindVertexArray(0);
        }
    }
};




