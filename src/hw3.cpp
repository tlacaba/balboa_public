#include "hw3.h"
#include "3rdparty/glad.h" // needs to be included before GLFW!
#include "3rdparty/glfw/include/GLFW/glfw3.h"
#include "hw3_scenes.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace hw3;


// Debugging 
void error_callback(int code, const char* description)
{
    std::cout << "Error code: " << code << std::endl;
    std::cout << "Description: " << description << std::endl;
}

// RESIZING OF WINDOW 
// do we have to prototype this outside of hw_3_1?
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// This is ran in render loop to take in input
void processInput(GLFWwindow* window);

void hw_3_1(const std::vector<std::string> &params) {
    // HW 3.1: Open a window using GLFW

    // Debugging 
    glfwSetErrorCallback(error_callback);
    
    // INITIALIZING GLFW 
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // for Mac OS X

    // MAKING WINDOW 
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return; // -1, but we can't return any value
    }
    glfwMakeContextCurrent(window);

    // INITIALIZING GLAD 

    // do this before calling any OpenGL function, so shoud this be before initialization of GLFW? NEGATIVE.
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return; // -1, but we can't return any value
    }

    // TELLING OPENGL SIZE OF WINDOW 
    glViewport(0, 0, 800, 600);

    // REGISTERING CALLBACK FUNCTION FOR FRAME RESIZING 
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // RENDER LOOP 
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // CLEAN GLFW RESOURCES 
    glfwTerminate();
    return; // 0
}



void hw_3_2(const std::vector<std::string> &params) {
    // HW 3.2: Render a single 2D triangle

    // vertex shader code to be compiled dynamically
    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "uniform float time;\n"
        "void main()\n"
        "{\n"
            "float rotX = cos(time);\n"
            "float rotY = sin(time);\n"
            "gl_Position = vec4(rotX * aPos.x - rotY * aPos.y, rotY * aPos.x + rotX * aPos.y , aPos.z, 1.0);\n"
        "}\n";

    const char* fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
            "FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\n";

    // Debugging 

    glfwSetErrorCallback(error_callback);
    
    // INITIALIZING GLFW 
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // for Mac OS X

    // MAKING WINDOW 
    GLFWwindow* window = glfwCreateWindow(800, 800, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return; // -1, but we can't return any value
    }
    glfwMakeContextCurrent(window);

    // INITIALIZING GLAD 
    // do this before calling any OpenGL function, so shoud this be before initialization of GLFW? NEGATIVE.
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return; // -1, but we can't return any value
    }

    //TELLING OPENGL SIZE OF WINDOW
    glViewport(0, 0, 800, 800);

    //REGISTERING CALLBACK FUNCTION FOR FRAME RESIZING
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);



    // building and compiling the shader program
    // =========================================
    // making vertex shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // attach code to shader object and compile it
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // test for failure in compilation
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // do same for fragment shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // make program
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    // attach and link shaders within program
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // test for failure in compilation
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::PROGRAM::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    //glUseProgram(shaderProgram); // dont think i should use like this yet

    // don't need these anymore
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertext data and buffers and configure vertex attributes
    // Vertices of triangle
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    // Generate VBO to send data to VRAM
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    // generate VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    // 1. bind VAO
    glBindVertexArray(VAO);

    // 2. copy vertices to array in buffer for OpenGL
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 3. make a vertex attribute pointer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    // RENDER LOOP 
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        double timeValue = glfwGetTime();
        int vertexRotationLocation = glGetUniformLocation(shaderProgram, "time");
        glUniform1f(vertexRotationLocation, timeValue);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // CLEAN GLFW RESOURCES 
    glfwTerminate();
    return; // 0
}



void hw_3_3(const std::vector<std::string> &params) {
    // HW 3.3: Render a scene
    if (params.size() == 0) {
        return;
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    // vertex shader code to be compiled dynamically
    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "void main()\n"
        "{\n"
            "gl_Position = projection * view * model * vec4(aPos, 1.0f);\n"
        "}\n";

    // TODO: handle vertex colors
    const char* fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
            "FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\n";

    // Debugging 

    glfwSetErrorCallback(error_callback);
    
    // INITIALIZING GLFW 
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // for Mac OS X

    // MAKING WINDOW 
    int sceneWidth = scene.camera.resolution.x;
    int sceneHeight = scene.camera.resolution.y;

    GLFWwindow* window = glfwCreateWindow(sceneWidth, sceneHeight, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return; // -1, but we can't return any value
    }
    glfwMakeContextCurrent(window);

    // INITIALIZING GLAD 
    // do this before calling any OpenGL function, so shoud this be before initialization of GLFW? NEGATIVE.
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return; // -1, but we can't return any value
    }

    //TELLING OPENGL SIZE OF WINDOW
    glViewport(0, 0, sceneWidth, sceneWidth);

    //REGISTERING CALLBACK FUNCTION FOR FRAME RESIZING
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // building and compiling the shader program
    // =========================================
    // making vertex shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // attach code to shader object and compile it
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // test for failure in compilation
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // do same for fragment shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // make program
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    // attach and link shaders within program
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // test for failure in compilation
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::PROGRAM::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    //glUseProgram(shaderProgram); // dont think i should use like this yet

    // don't need these anymore
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertext data and buffers and configure vertex attributes
    
    // Vertices of triangles WE'RE GONNA BE USING FROM SCENES
    // float vertices[] = {
    //      0.5f,  0.5f, 0.0f,  // top right
    //      0.5f, -0.5f, 0.0f,  // bottom right
    //     -0.5f, -0.5f, 0.0f,  // bottom left
    //     -0.5f,  0.5f, 0.0f   // top left  
    // };
    // unsigned int indices[] = {  // note that we start from 0!
    //     0, 1, 3,   // first triangle
    //     1, 2, 3    // second triangle
    // };  

    // TODO: repeat this process for multiple meshes

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // TODO: see if this is right
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, scene.meshes[0].vertices.size() * sizeof(Vector3f), scene.meshes[0].vertices.data(), GL_STATIC_DRAW);
    // we're saying we're gonna write numVertices * size of Vector3f's, but isn't the actual size of vertices larger than that since they're just Vector3's (doubles not floats?)

    // TODO: see if this is right
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, scene.meshes[0].faces.size() * 3 * sizeof(Vector3f), scene.meshes[0].faces.data(), GL_STATIC_DRAW);
    // don't i have to multiply faces.size by 3 since there are 3 indices per?

    // position attribute (gl float, not double?), set stride to NULL, be careful
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, NULL, (void*)0);
    glEnableVertexAttribArray(0);

    Vector3f bgColor = scene.background;

//std::cout << "meshes: " << (scene.meshes.size()) << std::endl;
    
    // can i put this out here? it doesn't change over time im pretty sure
    float z_far = scene.camera.z_far;
    float z_near = scene.camera.z_near;
    glm::mat4 projection;
    projection[0][0] = 1.0f / ((float)sceneWidth / sceneHeight * scene.camera.s);
    projection[1][1] = 1.0f / scene.camera.s;
    projection[2][2] = - z_far / (z_far - z_near);
    projection[3][2] = - z_far * z_near / (z_far - z_near);
    projection[2][3] = - 1.0f;

    // RENDER LOOP 
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(bgColor.x, bgColor.y, bgColor.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        // TODO: see how to convert this easily

        // How to repeat this for each mesh?
        glm::mat4 model; //= scene.meshes[0].model_matrix;
        glm::mat4 view; //= inverse(scene.camera.cam_to_world);

        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, scene.meshes[0].faces.size() * 3, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // CLEAN GLFW RESOURCES 
    glfwTerminate();
    return; // 0
}

// ================================
// Old code:
    // unsigned int EBO;
    // glGenBuffers(1, &EBO);

    // // Generate VBO to send data to VRAM
    // unsigned int VBO;
    // glGenBuffers(1, &VBO);

    // // generate VAO
    // unsigned int VAO;
    // glGenVertexArrays(1, &VAO);

    // // 1. bind VAO
    // glBindVertexArray(VAO);

    // // 2. copy vertices to array in buffer for OpenGL
    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // // 3. copy our index array into element buffer for OpenGL
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // // 4. make vertex attribute pointers
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(0);

    // glBindBuffer(GL_ARRAY_BUFFER, 0);

    // glBindVertexArray(0);
// ================================

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