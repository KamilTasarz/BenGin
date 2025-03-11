﻿#include "config.h"

#include "Shader.h"
#include "Camera.h"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double posX, double posY);
bool init();

struct PointLight {

    glm::vec3 position;

    // Colors
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float quadratic;
    float linear;
    float constant;

};

float vertices[] = {

        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f

};

float normals[] = {
    0.f, 0.f, 1.f, //front
    1.f, 0.f, 0.f, //prawa
    0.f, 1.f, 0.f, //gora
    -1.f, 0.f, 0.f, //lewa
    0.f, -1.f, 0.f, //dol
    0.f, 0.f, -1.f //tyl

};
    
const char* vertexPath = "res/shaders/basic.vert";
const char* fragmentPath = "res/shaders/basic.frag";

bool firstMouseMovement = true;

float lastX = (float)WINDOW_WIDTH / 2.0;
float lastY = (float)WINDOW_HEIGHT / 2.0;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

Camera camera(0.f, 0.f, -3.f);

// -- MAIN --

int main() {
    
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL Window", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glEnable(GL_DEPTH_TEST);

    PointLight light = {
        glm::vec3(0.0f, 2.0f, -3.0f), // position
        glm::vec3(0.1f, 0.1f, 0.1f), // ambient
        glm::vec3(0.7f, 0.7f, 0.7f), // diffuse
        glm::vec3(0.7f, 0.7f, 0.7f), // specular
        0.016f, 0.004f, 1.0f // quadratic, linear, constant
    };

    Shader *shader = new Shader(vertexPath, fragmentPath);
    
    unsigned int VAO, VBO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    int width, height, nrChannels;
    unsigned char* data = stbi_load("res/textures/stone.jpg", &width, &height, &nrChannels, 0);

    unsigned int texture1, texture2;
    glGenTextures(1, &texture1);
    glGenTextures(1, &texture2);

    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    data = stbi_load("res/textures/wood.jpg", &width, &height, &nrChannels, 0);

    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);


    shader->use();

    int num = 2;
    int lightNum = 1;
    glm::mat4 *matrices = new glm::mat4[num + lightNum];
    int* textureIDs = new int[num];

    for (int i = 0; i < num; i++) {
        glm::mat4 model = glm::mat4(1.f);
        model = glm::translate(model, glm::vec3(4 * i, 0.f, 0.f));
        matrices[i] = model;
    }

    matrices[num] = glm::translate(glm::mat4(1), light.position);
    light.position = glm::vec3(matrices[num] * glm::vec4(0, 0, 0, 1));
    
    //unsigned int buffer;

    //glGenBuffers(1, &buffer);
    //glBindBuffer(GL_ARRAY_BUFFER, buffer);
    //glBufferData(GL_ARRAY_BUFFER, num * sizeof(glm::mat4), &matrices[0], GL_DYNAMIC_DRAW);
    //
    //glBindVertexArray(VAO);
    //// Atrybuty wierzchołków
    //GLsizei vec4Size = sizeof(glm::vec4);
    //glEnableVertexAttribArray(3);
    //glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
    //glEnableVertexAttribArray(4);
    //glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size));
    //glEnableVertexAttribArray(5);
    //glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
    //glEnableVertexAttribArray(6);
    //glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

    //glVertexAttribDivisor(3, 1);
    //glVertexAttribDivisor(4, 1);
    //glVertexAttribDivisor(5, 1);
    //glVertexAttribDivisor(6, 1);

    //glBindVertexArray(0);
    
    //glUniformMatrix4fv(glGetUniformLocation(shader->ID, "model"), 1, false, glm::value_ptr(model));

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "projection"), 1, false, glm::value_ptr(projection));

    while (!glfwWindowShouldClose(window)) {
        
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        int direction = 0;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            direction += 1;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            direction += 2;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            direction += 4;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            direction += 8;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            direction += 16;

        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            direction += 32;
            
        }

        camera.ProcessKeyboard(deltaTime, direction);
		
        glm::mat4 view = camera.GetView();
        glUniformMatrix4fv(glGetUniformLocation(shader->ID, "view"), 1, false, glm::value_ptr(view));

        shader->setFloat("light.constant", light.constant);
        shader->setFloat("light.linear", light.linear);
        shader->setFloat("light.quadratic", light.quadratic);
        shader->setVec3("light.position", light.position);
        shader->setVec3("light.ambient", light.ambient);
        shader->setVec3("light.diffuse", light.diffuse);
        shader->setVec3("light.specular", light.specular);
        shader->setVec3("cameraPosition", camera.cameraPos);



        glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindTexture(GL_TEXTURE_2D, texture1);
        
        
        glBindVertexArray(VAO);
        
        glUniform1i(glGetUniformLocation(shader->ID, "isLight"), 0);
        glUniformMatrix4fv(glGetUniformLocation(shader->ID, "model"), 1, false, glm::value_ptr(matrices[0]));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindTexture(GL_TEXTURE_2D, texture2);
        glUniformMatrix4fv(glGetUniformLocation(shader->ID, "model"), 1, false, glm::value_ptr(matrices[1]));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //model swiatla
        glUniform1i(glGetUniformLocation(shader->ID, "isLight"), 1);
        glUniformMatrix4fv(glGetUniformLocation(shader->ID, "model"), 1, false, glm::value_ptr(matrices[2]));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete shader;
    delete[] matrices;
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouseCallback(GLFWwindow* window, double posX, double posY) {
    float x = static_cast<float>(posX);
    float y = static_cast<float>(posY);

    if (firstMouseMovement) {
        lastX = x;
        lastY = y;
        firstMouseMovement = false;
    }

    float offsetX = x - lastX;
    float offsetY = lastY - y; // reversed

    lastX = x;
    lastY = y;

    camera.ProcessMouseMovement(offsetX, offsetY);
    
}

bool init() {
    if (!glfwInit()) return false;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    return true;
}
