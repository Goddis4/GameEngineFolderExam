#include "Box.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

Box::Box(const glm::vec3& position, const glm::vec3& size)
    : mPosition(position),
    mSize(size),
    mCollideSpheres(position, size), 
    mParticleSystem(1000, position - size / 2.0f, position + size / 2.0f) {
    makingBox(); 
}

uint32_t Box::addSphereEntity(const glm::vec3& position, const glm::vec3& velocity, float radius, const glm::vec3& color) {
    // Delegate entity creation to CollideSpheres
    return mCollideSpheres.addSphere(position, velocity, radius, color);
}

void Box::update(float deltaTime) {
    glm::vec3 boxMin = mPosition - mSize * 0.5f;
    glm::vec3 boxMax = mPosition + mSize * 0.5f;

    // Update particles
    mParticleSystem.setBounds(boxMin, boxMax);
    mParticleSystem.update(deltaTime);

    
    mCollideSpheres.update(deltaTime);
    //mCollideSpheres.printAllEntities();
}

void Box::render(Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
    shader.use();

    // Render the box
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, mPosition);
    model = glm::scale(model, glm::vec3(1.0f));

    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    // Lighting and material properties
    shader.setVec3("lightPos", glm::vec3(0.0f, 20.0f, 0.0f));
    shader.setVec3("viewPos", glm::vec3(0.0f, 5.0f, 10.0f));
    shader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    shader.setVec3("objectColor", glm::vec3(1.0f, 0.0f, 0.0f));
    shader.setBool("useFlatColor", false);

    glBindVertexArray(mVAO);
    glDrawElements(GL_TRIANGLES, 6 * 5, GL_UNSIGNED_INT, 0); 
    glBindVertexArray(0);

    mParticleSystem.render(shader, view, projection);
    mCollideSpheres.render(shader, view, projection);
}

void Box::makingBox() {
    float halfWidth = mSize.x / 2.0f;
    float halfHeight = mSize.y / 2.0f;
    float halfDepth = mSize.z / 2.0f;

    
    float vertices[] = {
        // Floor (y = 0)
        -halfWidth, 0.0f, -halfDepth,  0.0f, 1.0f, 0.0f, // Bottom-left
         halfWidth, 0.0f, -halfDepth,  0.0f, 1.0f, 0.0f, // Bottom-right
         halfWidth, 0.0f,  halfDepth,  0.0f, 1.0f, 0.0f, // Top-right
        -halfWidth, 0.0f,  halfDepth,  0.0f, 1.0f, 0.0f, // Top-left

        // Back wall (z = -halfDepth)
        -halfWidth, 0.0f, -halfDepth,  0.0f, 0.0f, -1.0f, // Bottom-left
         halfWidth, 0.0f, -halfDepth,  0.0f, 0.0f, -1.0f, // Bottom-right
         halfWidth, mSize.y, -halfDepth,  0.0f, 0.0f, -1.0f, // Top-right
        -halfWidth, mSize.y, -halfDepth,  0.0f, 0.0f, -1.0f, // Top-left

        // Front wall (z = +halfDepth)
        -halfWidth, 0.0f,  halfDepth,  0.0f, 0.0f, 1.0f, // Bottom-left
         halfWidth, 0.0f,  halfDepth,  0.0f, 0.0f, 1.0f, // Bottom-right
         halfWidth, mSize.y,  halfDepth,  0.0f, 0.0f, 1.0f, // Top-right
        -halfWidth, mSize.y,  halfDepth,  0.0f, 0.0f, 1.0f, // Top-left

        // Left wall (x = -halfWidth)
        -halfWidth, 0.0f, -halfDepth,  -1.0f, 0.0f, 0.0f, // Bottom-front
        -halfWidth, 0.0f,  halfDepth,  -1.0f, 0.0f, 0.0f, // Bottom-back
        -halfWidth, mSize.y,  halfDepth,  -1.0f, 0.0f, 0.0f, // Top-back
        -halfWidth, mSize.y, -halfDepth,  -1.0f, 0.0f, 0.0f, // Top-front

        // Right wall (x = +halfWidth)
         halfWidth, 0.0f, -halfDepth,  1.0f, 0.0f, 0.0f, // Bottom-front
         halfWidth, 0.0f,  halfDepth,  1.0f, 0.0f, 0.0f, // Bottom-back
         halfWidth, mSize.y,  halfDepth,  1.0f, 0.0f, 0.0f, // Top-back
         halfWidth, mSize.y, -halfDepth,  1.0f, 0.0f, 0.0f, // Top-front
    };

    unsigned int indices[] = {
        // Floor
        0, 1, 2, 2, 3, 0,
        // Back wall
        4, 5, 6, 6, 7, 4,
        // Front wall
        8, 9, 10, 10, 11, 8,
        // Left wall
        12, 13, 14, 14, 15, 12,
        // Right wall
        16, 17, 18, 18, 19, 16,
    };


    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glGenBuffers(1, &mEBO);

    glBindVertexArray(mVAO);

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // Position
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // Normal
    glEnableVertexAttribArray(1);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

//Box::Box(const glm::vec3& position, const glm::vec3& size)
//    : mPosition(position), mSize(size), mCollideSpheres(position, size), mParticleSystem(1000, position - size / 2.0f, position + size / 2.0f) {
//    makingBox();
//}
//
//void Box::addSphere(const Spheres& sphere) {
//    mCollideSpheres.addSphere(sphere);
//}
//
//void Box::update(float deltaTime)
//{
//
//    glm::vec3 boxMin = mPosition - mSize * 0.5f;
//    glm::vec3 boxMax = mPosition + mSize * 0.5f;
//
//    mParticleSystem.setBounds(boxMin, boxMax);
//
//    mCollideSpheres.update(deltaTime);
//    mParticleSystem.update(deltaTime);
//}
//
//void Box::makingBox() {
//    float halfWidth = mSize.x / 2.0f;
//    float halfHeight = mSize.y / 2.0f;
//    float halfDepth = mSize.z / 2.0f;
//
//    // Define vertices for the floor and walls
//    float vertices[] = {
//        // Floor (y = 0)
//        -halfWidth, 0.0f, -halfDepth,  0.0f, 1.0f, 0.0f, // Bottom-left
//         halfWidth, 0.0f, -halfDepth,  0.0f, 1.0f, 0.0f, // Bottom-right
//         halfWidth, 0.0f,  halfDepth,  0.0f, 1.0f, 0.0f, // Top-right
//        -halfWidth, 0.0f,  halfDepth,  0.0f, 1.0f, 0.0f, // Top-left
//
//        // Back wall (z = -halfDepth)
//        -halfWidth, 0.0f, -halfDepth,  0.0f, 0.0f, -1.0f, // Bottom-left
//         halfWidth, 0.0f, -halfDepth,  0.0f, 0.0f, -1.0f, // Bottom-right
//         halfWidth, mSize.y, -halfDepth,  0.0f, 0.0f, -1.0f, // Top-right
//        -halfWidth, mSize.y, -halfDepth,  0.0f, 0.0f, -1.0f, // Top-left
//
//        // Front wall (z = +halfDepth)
//        -halfWidth, 0.0f,  halfDepth,  0.0f, 0.0f, 1.0f, // Bottom-left
//         halfWidth, 0.0f,  halfDepth,  0.0f, 0.0f, 1.0f, // Bottom-right
//         halfWidth, mSize.y,  halfDepth,  0.0f, 0.0f, 1.0f, // Top-right
//        -halfWidth, mSize.y,  halfDepth,  0.0f, 0.0f, 1.0f, // Top-left
//
//        // Left wall (x = -halfWidth)
//        -halfWidth, 0.0f, -halfDepth,  -1.0f, 0.0f, 0.0f, // Bottom-front
//        -halfWidth, 0.0f,  halfDepth,  -1.0f, 0.0f, 0.0f, // Bottom-back
//        -halfWidth, mSize.y,  halfDepth,  -1.0f, 0.0f, 0.0f, // Top-back
//        -halfWidth, mSize.y, -halfDepth,  -1.0f, 0.0f, 0.0f, // Top-front
//
//        // Right wall (x = +halfWidth)
//         halfWidth, 0.0f, -halfDepth,  1.0f, 0.0f, 0.0f, // Bottom-front
//         halfWidth, 0.0f,  halfDepth,  1.0f, 0.0f, 0.0f, // Bottom-back
//         halfWidth, mSize.y,  halfDepth,  1.0f, 0.0f, 0.0f, // Top-back
//         halfWidth, mSize.y, -halfDepth,  1.0f, 0.0f, 0.0f, // Top-front
//        };
//
//    unsigned int indices[] = {
//        // Floor
//        0, 1, 2, 2, 3, 0,
//        // Back wall
//        4, 5, 6, 6, 7, 4,
//        // Front wall
//        8, 9, 10, 10, 11, 8,
//        // Left wall
//        12, 13, 14, 14, 15, 12,
//        // Right wall
//        16, 17, 18, 18, 19, 16,
//    };
//
//
//    glGenVertexArrays(1, &mVAO);
//    glGenBuffers(1, &mVBO);
//    glGenBuffers(1, &mEBO);
//
//    glBindVertexArray(mVAO);
//
//    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // Position
//    glEnableVertexAttribArray(0);
//
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // Normal
//    glEnableVertexAttribArray(1);
//
//
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glBindVertexArray(0);
//
//}
//
//void Box::render(Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
//    shader.use();
//
//
//    glm::mat4 model = glm::mat4(1.0f);
//    model = glm::translate(model, mPosition);
//    model = glm::scale(model, glm::vec3(1.0f));
//
//    shader.setMat4("model", model);
//    shader.setMat4("view", view);
//    shader.setMat4("projection", projection);
//
//    // Pass lighting and material properties
//    shader.setVec3("lightPos", glm::vec3(0.0f, 20.0f, 0.0f)); // Example light position
//    shader.setVec3("viewPos", glm::vec3(0.0f, 5.0f, 10.0f)); // Example camera position
//    shader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f)); // White light
//    shader.setVec3("objectColor", glm::vec3(1.0f, 0.0f, 0.0f)); // Red color
//    shader.setBool("useFlatColor", false); // Use Phong shading
//
//    // Render the box
//    glBindVertexArray(mVAO);
//    glDrawElements(GL_TRIANGLES, 6 * 5, GL_UNSIGNED_INT, 0); // 5 sides (floor + 4 walls)
//    glBindVertexArray(0);
//
//    // Render all spheres inside the box
//    mParticleSystem.render(shader, view, projection);
//    mCollideSpheres.render(shader, view, projection);
//    
//}
