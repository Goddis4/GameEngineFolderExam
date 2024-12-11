#ifndef SPHERES_H
#define SPHERES_H

#include <glm/glm.hpp>
#include <vector>
#include "Shader.h"

class Spheres {
public:

    //Spheres(float radius, const glm::vec3& initialPosition = glm::vec3(0.0f));
    Spheres(float radius, const glm::vec3& initialPosition, const glm::vec3& initialVelocity);


    void createSphere();
    void renderSphere(Shader& shader, const glm::mat4& view, const glm::mat4& projection);
    void update(float deltaTime);


    void setVelocity(const glm::vec3 velocity) {
        mVelocity = velocity;
    }
    

    glm::vec3 getVelocity() const { return mVelocity; }
    glm::vec3 getPosition() const { return mPosition; }
    float getRadius() const { return mRadius; }
    
    glm::vec3 mPosition;
    glm::vec3 mVelocity;
    float mRadius;
    float mass;

private:
    unsigned int mVAO, mVBO;
    std::vector<glm::vec3> mVertices;

    


    void lagTriangel(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3);
    void subDivide(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, int n);
};

#endif


