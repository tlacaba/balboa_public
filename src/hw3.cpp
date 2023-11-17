#include "hw3.h"
#include "3rdparty/glad.h" // needs to be included before GLFW!
#include "3rdparty/glfw/include/GLFW/glfw3.h"
#include "hw3_scenes.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace hw3;


// Debugging 
void error_callback(int code, const char* description);

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



unsigned int makeShaderProgram(const GLchar** vertexShaderSourceAddress, const GLchar** fragementShaderSourceAddress);

GLFWwindow* initGlfwAndWindow(int sceneWidth, int sceneHeight);


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
        "layout (location = 1) in vec3 aColor;\n"
        "out vec3 ourColor;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "void main()\n"
        "{\n"
            "gl_Position = projection * view * model * vec4(aPos, 1.0f);\n"
            "ourColor = aColor;\n"
        "}\n";

    // fragment shader code to be compiled dynamically
    const char* fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "in vec3 ourColor;\n"
        "void main()\n"
        "{\n"
            "FragColor = vec4(ourColor, 1.0f);\n"
        "}\n";

    // MAKING WINDOW 
    int sceneWidth = scene.camera.resolution.x;
    int sceneHeight = scene.camera.resolution.y;
    
    GLFWwindow* window = initGlfwAndWindow(sceneWidth, sceneHeight);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);

    // MAKING SHADER PROGRAM
    unsigned int shaderProgram = makeShaderProgram(&vertexShaderSource, &fragmentShaderSource);

    // TODO: repeat this process for multiple meshes
    for (unsigned int i = 0; i < scene.meshes.size(); ++i) {
        TriangleMesh mesh = scene.meshes[i];

std::cout << "DEBUG: ITERATION OUTSIDE RENDER" << std::endl;
        // set up vertex data and buffers and configure vertex attributes
        // unsigned int VBO_vertex, VBO_color, VAO, EBO;
        glGenVertexArrays(1, &(mesh.VAO));
std::cout << "0: " << glGetError() << std::endl;
        glGenBuffers(1, &(mesh.VBO_vertex));
std::cout << "1: " << glGetError() << std::endl;
        glGenBuffers(1, &(mesh.VBO_color));
std::cout << "2: " << glGetError() << std::endl;
        glGenBuffers(1, &(mesh.EBO));
std::cout << "3: " << glGetError() << std::endl;

        glBindVertexArray(mesh.VAO);
std::cout << "4: " << glGetError() << std::endl;

        // TODO: see if this is right
        glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO_vertex);
std::cout << "5: " << glGetError() << std::endl;
        glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vector3f), mesh.vertices.data(), GL_STATIC_DRAW);
std::cout << "6: " << glGetError() << std::endl;
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), (void*)0);
std::cout << "7: " << glGetError() << std::endl;
        glEnableVertexAttribArray(0);
std::cout << "8: " << glGetError() << std::endl;
        // we're saying we're gonna write numVertices * size of Vector3f's, but isn't the actual size of vertices larger than that since they're just Vector3's (doubles not floats?) IT'S ACTUALLY JUST Vector3f so we're chillin

        glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO_color);
std::cout << "9: " << glGetError() << std::endl;
        glBufferData(GL_ARRAY_BUFFER, mesh.vertex_colors.size() * sizeof(Vector3f), mesh.vertex_colors.data(), GL_STATIC_DRAW);
std::cout << "10: " << glGetError() << std::endl;
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), (void*)0);
std::cout << "11: " << glGetError() << std::endl;
        glEnableVertexAttribArray(1);
std::cout << "12: " << glGetError() << std::endl;


        // TODO: see if this is right
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
std::cout << "13: " << glGetError() << std::endl;
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.faces.size() * 3 * sizeof(Vector3f), mesh.faces.data(), GL_STATIC_DRAW);
std::cout << "14: " << glGetError() << std::endl;
        // don't i have to multiply faces.size by 3 since there are 3 indices per?

        //glBindBuffer(GL_ARRAY_BUFFER, 0);
        //glBindVertexArray(0);
    }


    Vector3f bgColor = scene.background;

//std::cout << "meshes: " << (scene.meshes.size()) << std::endl;
    // can i put this out here? it doesn't change over time im pretty sure
    float z_far = scene.camera.z_far;
    float z_near = scene.camera.z_near;

    Matrix4x4f Projection;
    Projection(0,0) = 1.0f / ((float)sceneWidth / (float)sceneHeight * (float)scene.camera.s);
    Projection(1,1) = 1.0f / (float)scene.camera.s;
    Projection(2,2) = - (float)z_far / ((float)z_far - (float)z_near);
    Projection(2,3) = - (float)z_far * (float)z_near / ((float)z_far - (float)z_near);
    Projection(3,2) = - 1.0f;

std::cout << "DEBUG: ITERATION OUTSIDE RENDER" << std::endl;

    // RENDER LOOP
//int count = 0;
    //while (!glfwWindowShouldClose(window) && count < 10) {
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(bgColor.x, bgColor.y, bgColor.z, 1.0f);
std::cout << "in 0: " << glGetError() << std::endl;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
std::cout << "in 1: " << glGetError() << std::endl;

        glUseProgram(shaderProgram);
std::cout << "in 2: " << glGetError() << std::endl;

        Matrix4x4f View = inverse(scene.camera.cam_to_world);

        for (unsigned int i = 0; i < scene.meshes.size(); ++i) {
            TriangleMesh mesh = scene.meshes[i];
            glBindVertexArray(mesh.VAO);
std::cout << "in 3: " << glGetError() << std::endl;

            Matrix4x4f Model = mesh.model_matrix;

            unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, Model.ptr());
std::cout << "in 4: " << glGetError() << std::endl;
            unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, View.ptr());
std::cout << "in 5: " << glGetError() << std::endl;
            unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, Projection.ptr());
std::cout << "in 6: " << glGetError() << std::endl;

            //glDrawArrays(GL_TRIANGLES, 0, 3);
            glDrawElements(GL_TRIANGLES, mesh.faces.size() * 3, GL_UNSIGNED_INT, 0);
std::cout << "in 7: " << glGetError() << std::endl;
            glBindVertexArray(0);
std::cout << "in 8: " << glGetError() << std::endl;
        }
        

        glfwSwapBuffers(window);
        glfwPollEvents();
        
//++count;
    }

    // CLEAN GLFW RESOURCES 
    glfwTerminate();
    return; // 0
}



void hw_3_4(const std::vector<std::string> &params) {
    // HW 3.4: Render a scene with lighting
    if (params.size() == 0) {
        return;
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;
}




// ==================================================
// Functions!

void error_callback(int code, const char* description) {
    std::cout << "Error code: " << code << std::endl;
    std::cout << "Description: " << description << std::endl;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// So far, just handling escape
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) 
        glfwSetWindowShouldClose(window, true);
}

unsigned int makeShaderProgram(const GLchar** vertexShaderSourceAddress, const GLchar** fragementShaderSourceAddress) {
    // building and compiling the shader program
    // =========================================
    // making vertex shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // attach code to shader object and compile it
    glShaderSource(vertexShader, 1, vertexShaderSourceAddress, NULL);
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
    glShaderSource(fragmentShader, 1, fragementShaderSourceAddress, NULL);
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

    // don't need these anymore
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

GLFWwindow* initGlfwAndWindow(int sceneWidth, int sceneHeight) {
    // Debugging 
    glfwSetErrorCallback(error_callback);
    
    // INITIALIZING GLFW 
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // for Mac OS X

    GLFWwindow* window = glfwCreateWindow(sceneWidth, sceneHeight, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return window; // -1, but we can't return any value
    }
    glfwMakeContextCurrent(window);

    // INITIALIZING GLAD 
    // do this before calling any OpenGL function, so shoud this be before initialization of GLFW? NEGATIVE.
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return window; // -1, but we can't return any value
    }

    //TELLING OPENGL SIZE OF WINDOW
    glViewport(0, 0, sceneWidth, sceneHeight);

    //REGISTERING CALLBACK FUNCTION FOR FRAME RESIZING
    //glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    return window;
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