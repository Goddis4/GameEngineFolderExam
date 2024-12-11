#include "CollideSpheres.h"
#include <functional>


CollideSpheres::CollideSpheres(const glm::vec3& boxPosition, const glm::vec3& boxSize)
    : mBoxPosition(boxPosition), mBoxSize(boxSize) {
    mWorldBounds.min = mBoxPosition - mBoxSize / 2.0f;
    mWorldBounds.max = mBoxPosition + mBoxSize / 2.0f;
}


uint32_t CollideSpheres::addSphere(const glm::vec3& position, const glm::vec3& velocity, float radius, const glm::vec3& color) {
    uint32_t entity = mEntityManager.createEntity();
    if (entity >= mComponents.transforms.size()) {
        mComponents.resize(entity + 1);
    }

    size_t vertexCount;
    uint32_t vao = createSphereVAO(radius, vertexCount);

    mComponents.transforms[entity] = { position, {}, glm::vec3(1.0f) };
    mComponents.physics[entity] = { velocity, 1.0f, radius };

    mComponents.renders[entity] = {
        vao,        // VAO
        0,          // VBO
        0,          // EBO
        color,      // Color
        radius,     // Radius
        vertexCount // Vertex Count
    };

    mSphereEntities.push_back(entity);
    return entity;
}

void CollideSpheres::printAllEntities() {
    std::cout << "Current Entities:" << std::endl;
    for (uint32_t entity : mSphereEntities) {
        // Retrieve the components for this entity
        const TransformComponent& transform = mComponents.transforms[entity];
        const PhysicsComponent& physics = mComponents.physics[entity];
        const RenderComponent& render = mComponents.renders[entity];

        std::cout << "Entity ID: " << entity << std::endl;
        std::cout << "  Position: ("
            << transform.position.x << ", "
            << transform.position.y << ", "
            << transform.position.z << ")" << std::endl;
        std::cout << "  Velocity: ("
            << physics.velocity.x << ", "
            << physics.velocity.y << ", "
            << physics.velocity.z << ")" << std::endl;
        std::cout << "  Radius: " << physics.radius << std::endl;
        std::cout << "  Color: ("
            << render.color.x << ", "
            << render.color.y << ", "
            << render.color.z << ")" << std::endl;
        std::cout << std::endl;
    }
    std::cout << "Total Entities: " << mSphereEntities.size() << std::endl;
}


void CollideSpheres::update(float deltaTime) {
    // Update physics for all entities in this box
    PhysicsSystem::update(mComponents, deltaTime);

    // Handle world bounds collisions
    CollisionSystem::updateWorldBoundCollisions(mComponents, mWorldBounds);

    // Handle inter-entity collisions
    CollisionSystem::updateInterEntityCollisions(mComponents);
}

void CollideSpheres::render(Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
    RenderSystem renderSystem;
    renderSystem.render(mComponents, shader, view, projection);
}

void CollideSpheres::removeEntity(uint32_t entity) {
    mEntityManager.destroyEntity(entity);
    auto it = std::find(mSphereEntities.begin(), mSphereEntities.end(), entity);
    if (it != mSphereEntities.end()) {
        mSphereEntities.erase(it);
    }
}

uint32_t CollideSpheres::createSphereVAO(float radius, size_t& outVertexCount)
{
    std::vector<glm::vec3> vertices;

    glm::vec3 v0{ 0.0f, 0.0f, 1.0f };
    glm::vec3 v1{ 1.0f, 0.0f, 0.0f };
    glm::vec3 v2{ 0.0f, 1.0f, 0.0f };
    glm::vec3 v3{ -1.0f, 0.0f, 0.0f };
    glm::vec3 v4{ 0.0f, -1.0f, 0.0f };
    glm::vec3 v5{ 0.0f, 0.0f, -1.0f };

    // Subdivision function
    std::function<void(const glm::vec3&, const glm::vec3&, const glm::vec3&, int)> subDivide;
    subDivide = [&](const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, int n) {
        if (n > 0) {
            glm::vec3 v1 = glm::normalize(a + b);
            glm::vec3 v2 = glm::normalize(a + c);
            glm::vec3 v3 = glm::normalize(c + b);
            subDivide(a, v1, v2, n - 1);
            subDivide(c, v2, v3, n - 1);
            subDivide(b, v3, v1, n - 1);
            subDivide(v3, v2, v1, n - 1);
        }
        else {
            vertices.push_back(glm::normalize(a) * radius);
            vertices.push_back(glm::normalize(b) * radius);
            vertices.push_back(glm::normalize(c) * radius);
        }
        };

    // Perform subdivisions
    subDivide(v0, v1, v2, 3);
    subDivide(v0, v2, v3, 3);
    subDivide(v0, v3, v4, 3);
    subDivide(v0, v4, v1, 3);
    subDivide(v5, v2, v1, 3);
    subDivide(v5, v3, v2, 3);
    subDivide(v5, v4, v3, 3);
    subDivide(v5, v1, v4, 3);

    // Prepare data for VAO
    std::vector<float> interleavedVertices;
    for (const auto& vertex : vertices) {
        // Position
        interleavedVertices.push_back(vertex.x);
        interleavedVertices.push_back(vertex.y);
        interleavedVertices.push_back(vertex.z);

        // Normal (same as position for unit sphere)
        glm::vec3 normal = glm::normalize(vertex);
        interleavedVertices.push_back(normal.x);
        interleavedVertices.push_back(normal.y);
        interleavedVertices.push_back(normal.z);
    }



    // Create VAO and buffers
    uint32_t vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    // Vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, interleavedVertices.size() * sizeof(float), interleavedVertices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // Set indices count for rendering
    outVertexCount = vertices.size();

    return vao;
}



//#include "CollideSpheres.h"
//
//CollideSpheres::CollideSpheres(const glm::vec3& boxPosition, const glm::vec3& boxSize)
//    : mBoxPosition(boxPosition), mBoxSize(boxSize) {
//}
//
//void CollideSpheres::addSphere(const Spheres& sphere) {
//    mSpheres.push_back(sphere);
//}
//
//
//void CollideSpheres::update(float deltaTime) {
//   
//    glm::vec3 boxMin = mBoxPosition - mBoxSize / 2.0f;
//    glm::vec3 boxMax = mBoxPosition + mBoxSize / 2.0f;
//
//
//
//    for (auto& sphere : mSpheres) {
//        sphere.update(deltaTime); // Update sphere position
//        checkWallCollision(sphere, boxMin, boxMax); // Check for wall collisions
//
//        const glm::vec3& pos = sphere.getPosition();
//    }
//    checkBallCollision();
//}
//
//void CollideSpheres::render(Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
//    // Render each sphere
//    for (auto& sphere : mSpheres) {
//        sphere.renderSphere(shader, view, projection);
//    }
//}
//
//void CollideSpheres::checkBallCollision() {
//    for (size_t i = 0; i < mSpheres.size(); ++i) {
//        for (size_t j = i + 1; j < mSpheres.size(); ++j) {
//            float collisionTime = detectCollision(mSpheres[i], mSpheres[j], 1.0f);
//            if (collisionTime < 1.0f) {
//                // Update positions to collision point
//                mSpheres[i].mPosition += mSpheres[i].mVelocity * collisionTime;
//                mSpheres[j].mPosition += mSpheres[j].mVelocity * collisionTime;
//
//                // Resolve collision
//                resolveCollision(mSpheres[i], mSpheres[j]);
//
//                // Use post-collision velocities for the remaining time
//                float remainingTime = 1.0f - collisionTime;
//                mSpheres[i].mPosition += mSpheres[i].mVelocity * remainingTime;
//                mSpheres[j].mPosition += mSpheres[j].mVelocity * remainingTime;
//            }
//        }
//    }
//}
//
//float CollideSpheres::detectCollision(const Spheres& s1, const Spheres& s2, float deltaTime) {
//    glm::vec3 A = s1.mPosition - s2.mPosition;
//    glm::vec3 B = s1.mVelocity - s2.mVelocity;
//    float A2 = glm::dot(A, A);
//    float B2 = glm::dot(B, B);
//    float AB = glm::dot(A, B);
//    float d = s1.mRadius + s2.mRadius;
//    float discriminant = AB * AB - B2 * (A2 - d * d);
//
//    if (discriminant < 0.0f) return 1.0f; // No collision
//    float t1 = (-AB - sqrt(discriminant)) / B2;
//    float t2 = (-AB + sqrt(discriminant)) / B2;
//
//    float t = (t1 >= 0.0f && t1 <= deltaTime) ? t1 : t2;
//    return (t >= 0.0f && t <= deltaTime) ? t : 1.0f; // Return collision time or 1.0f for no collision
//}
//
//void CollideSpheres::resolveCollision(Spheres& s1, Spheres& s2) {
//    glm::vec3 normal = glm::normalize(s1.mPosition - s2.mPosition);
//    float m1 = s1.mass, m2 = s2.mass;
//    glm::vec3 v1 = s1.mVelocity, v2 = s2.mVelocity;
//
//    float v1n = glm::dot(v1, normal);
//    float v2n = glm::dot(v2, normal);
//
//    float v1nPrime = (m1 - m2) / (m1 + m2) * v1n + (2 * m2) / (m1 + m2) * v2n;
//    float v2nPrime = (m2 - m1) / (m1 + m2) * v2n + (2 * m1) / (m1 + m2) * v1n;
//
//    glm::vec3 v1Prime = v1 + (v1nPrime - v1n) * normal;
//    glm::vec3 v2Prime = v2 + (v2nPrime - v2n) * normal;
//
//    s1.mVelocity = v1Prime;
//    s2.mVelocity = v2Prime;
//}
//
//void CollideSpheres::checkWallCollision(Spheres& sphere, const glm::vec3& boxMin, const glm::vec3& boxMax) {
//    glm::vec3 position = sphere.getPosition();
//    glm::vec3 velocity = sphere.getVelocity();
//    float radius = sphere.getRadius();
//
//    for (int i = 0; i < 3; ++i) { // Iterate over X, Y, Z axes
//        if (position[i] - radius < boxMin[i]) {
//            position[i] = boxMin[i] + radius; // Adjust position
//            velocity[i] = std::abs(velocity[i]); // Ensure velocity points inward
//        }
//        else if (position[i] + radius > boxMax[i]) {
//            position[i] = boxMax[i] - radius; // Adjust position
//            velocity[i] = -std::abs(velocity[i]); // Ensure velocity points inward
//        }
//    }
//
//    sphere.setVelocity(velocity); // Update sphere velocity
//}
//
//
