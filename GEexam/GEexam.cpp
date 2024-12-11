#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "Camera.h"
#include "Shader.h"
#include "Spheres.h"
#include "World.h"


//Lua includes
extern "C"
{
#include "lua54/include/lua.h"
#include "lua54/include/lauxlib.h"
#include "lua54/include/lualib.h"
}

// Link to lua library
#ifdef _WIN32
#pragma comment(lib, "lua54/lua54.lib")
#endif

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, Camera& camera, bool& shouldReloadScript);


ComponentArrays gComponents;

int lua_createEntity(lua_State* L) {
    uint32_t entity = gComponents.transforms.size();
    gComponents.resize(entity + 1);

    std::cout << "Lua created entity ID: " << entity << std::endl;

    lua_pushinteger(L, entity); // Return entity ID
    return 1; // Number of return values
}



int lua_setPosition(lua_State* L) {
    uint32_t entity = lua_tointeger(L, 1); // Get entity ID
    float x = lua_tonumber(L, 2);          // Get x coordinate
    float y = lua_tonumber(L, 3);          // Get y coordinate
    float z = lua_tonumber(L, 4);          // Get z coordinate


    if (entity < gComponents.transforms.size()) {
        gComponents.transforms[entity].position = { x, y, z };
    }
    else {
        std::cerr << "Invalid entity ID: " << entity << std::endl;
    }

    return 0; // No return values
}

int lua_setVelocity(lua_State* L) {
    uint32_t entity = lua_tointeger(L, 1);
    float vx = lua_tonumber(L, 2);
    float vy = lua_tonumber(L, 3);
    float vz = lua_tonumber(L, 4);

    if (entity < gComponents.physics.size()) {
        gComponents.physics[entity].velocity = { vx, vy, vz };
    }
    else {
        std::cerr << "Invalid entity ID: " << entity << std::endl;
    }

    return 0;
}

int lua_setColor(lua_State* L) {
    uint32_t entity = lua_tointeger(L, 1);
    float r = lua_tonumber(L, 2);
    float g = lua_tonumber(L, 3);
    float b = lua_tonumber(L, 4);

    if (entity < gComponents.renders.size()) {
        gComponents.renders[entity].color = { r, g, b };
    }
    else {
        std::cerr << "Invalid entity ID: " << entity << std::endl;
    }

    return 0;
}


// Register functions in Lua
void registerLuaFunctions(lua_State* L) {

    int lua_setVelocity(lua_State * L);
    int lua_setPosition(lua_State * L);
    int lua_setColor(lua_State * L);

    lua_register(L, "createEntity", lua_createEntity);
    lua_register(L, "setPosition", lua_setPosition);
    lua_register(L, "setVelocity", lua_setVelocity);
    lua_register(L, "setColor", lua_setColor);
}

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

int main()
{

    //-----------------------------------------------------------------------------------------------//
    //-------------------------------------INITILIZE-------------------------------------------------//
    //-----------------------------------------------------------------------------------------------//
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);



    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Test Win", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);  // Enable depth testing



    // build and compile our shader program
    // ------------------------------------
    Shader ourShader("Exam.vs", "Exam.fs");




    //-----------------------------------------------------------------------------------------------//
    //-------------------------------------Forward--Declarations-------------------------------------//
    //-----------------------------------------------------------------------------------------------//
    Camera camera;
    World world;

    world.addBox(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(50.0f, 10.0f, 50.0f)); // Add a box to the world
    //world.createSphereEntity(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), 1.0f, glm::vec3(0.0f, 1.0f, 0.0f)); 
    //world.createSphereEntity(glm::vec3(0.0f, 1.0f, -10.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, glm::vec3(0.0f, 1.0f, 0.0f)); // Add a sphere to the box ECS style

    /*world.addSphereToBox(Spheres(1.0f, glm::vec3(0.5f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    world.addSphereToBox(Spheres(1.0f, glm::vec3(0.0f, 1.0f, -10.0f), glm::vec3(0.0f, 0.0f, -0.3f)));*/


    //-----------------------------------------------------------------------------------------------//
    //---------------------------------------------Lua-----------------------------------------------//
    //-----------------------------------------------------------------------------------------------//

    lua_State* L = luaL_newstate();
    luaL_openlibs(L); // Load Lua standard libraries


    registerLuaFunctions(L);

    std::unordered_set<uint32_t> processedEntities;

    // Load and execute the Lua script
    if (luaL_dofile(L, "myLua.lua") != LUA_OK) {
        std::cerr << "Error loading Lua script: " << lua_tostring(L, -1) << std::endl;
    }

    
    for (size_t i = 0; i < gComponents.transforms.size(); ++i) {
        if (gComponents.transforms[i].position != glm::vec3(0.0f)) {
            std::cout << "Adding entity " << i << " to the world with position: ("
                << gComponents.transforms[i].position.x << ", "
                << gComponents.transforms[i].position.y << ", "
                << gComponents.transforms[i].position.z << ")" << std::endl;
            world.createSphereEntity(
                gComponents.transforms[i].position,
                gComponents.physics[i].velocity,
                1.0f,  // default radius
                gComponents.renders[i].color
            );
        }
    }
   
    bool shouldReloadScript = false;

    //-----------------------------------------------------------------------------------------------//
    //-----------------------------------------RenderLoop--------------------------------------------//
    //-----------------------------------------------------------------------------------------------//

    float deltaTime = 0.016f;

    while (!glfwWindowShouldClose(window))
    {
        camera.processInput(window);
        processInput(window, camera, shouldReloadScript);


        if (shouldReloadScript) {
            std::cout << "Reloading Lua script...\n";

            // Mark existing entities as processed before reloading
            for (size_t i = 0; i < gComponents.transforms.size(); ++i) {
                processedEntities.insert(i);
            }

            if (luaL_dofile(L, "myLua.lua") != LUA_OK) {
                std::cerr << "Error reloading Lua script: " << lua_tostring(L, -1) << std::endl;
            }
            else {
                std::cout << "Lua script reloaded successfully! Entities: " << gComponents.transforms.size() << "\n";

                // Add only new entities created by Lua
                for (size_t i = 0; i < gComponents.transforms.size(); ++i) {
                    if (processedEntities.find(i) == processedEntities.end()) {
                        std::cout << "Adding new entity " << i << " to the world with position: ("
                            << gComponents.transforms[i].position.x << ", "
                            << gComponents.transforms[i].position.y << ", "
                            << gComponents.transforms[i].position.z << ")" << std::endl;

                        world.createSphereEntity(
                            gComponents.transforms[i].position,
                            gComponents.physics[i].velocity,
                            1.0f, // default radius
                            gComponents.renders[i].color
                        );
                        processedEntities.insert(i); // Mark entity as processed
                    }
                }
            }
            shouldReloadScript = false;
        }

        world.update(deltaTime);

        

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::lookAt(camera.position, camera.position + camera.orientation, camera.up);
        glm::mat4 projection = glm::perspective(glm::radians(60.0f), 800.0f / 600.0f, 0.1f, 10000.0f);
        glm::mat4 model = glm::mat4(1.0f);

        world.render(ourShader, view, projection);
        

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    lua_close(L);
    return 0;
}


void processInput(GLFWwindow* window, Camera& camera, bool& shouldReloadScript) {
    // Check for escape key
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    static bool reloadKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        if (!reloadKeyPressed) {
            shouldReloadScript = true; 
            reloadKeyPressed = true;  
        }
    }
    else {
        reloadKeyPressed = false; 
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
