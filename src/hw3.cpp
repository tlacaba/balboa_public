#include "hw3.h"
#include "3rdparty/glad.h" // needs to be included before GLFW!
#include "3rdparty/glfw/include/GLFW/glfw3.h"
#include "hw3_scenes.h"

using namespace hw3;

// ==== RESIZING OF WINDOW ==== //

// do we have to prototype this outside of hw_3_1?
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// This is ran in render loop to take in input
void processInput(GLFWwindow* window);

void hw_3_1(const std::vector<std::string> &params) {
    // HW 3.1: Open a window using GLFW
    
    // ==== INITIALIZING GLFW ==== //

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // for Mac OS X

    // ==== MAKING WINDOW ==== //

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return; // -1, but we can't return any value
    }
    glfwMakeContextCurrent(window);

    // ==== INITIALIZING GLAD ==== //

    // do this before calling any OpenGL function, so shoud this be before initialization of GLFW? NEGATIVE.
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return; // -1, but we can't return any value
    }

    // ==== TELLING OPENGL SIZE OF WINDOW ==== //
    
    glViewport(0, 0, 800, 600);

    // ==== REGISTERING CALLBACK FUNCTION FOR FRAME RESIZING ==== //

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // ==== RENDER LOOP ==== //

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // ==== CLEAN GLFW RESOURCES ==== //

    glfwTerminate();
    return; // 0
}

void hw_3_2(const std::vector<std::string> &params) {
    // HW 3.2: Render a single 2D triangle
    
}

void hw_3_3(const std::vector<std::string> &params) {
    // HW 3.3: Render a scene
    if (params.size() == 0) {
        return;
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;
}

void hw_3_4(const std::vector<std::string> &params) {
    // HW 3.4: Render a scene with lighting
    if (params.size() == 0) {
        return;
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// So far, just handling escape
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) 
        glfwSetWindowShouldClose(window, true);
}