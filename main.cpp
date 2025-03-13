#include "config.h"

#include "Shader.h"
#include "Camera.h"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double posX, double posY);
void changeMouse(GLFWwindow* window);
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



float boxVertices[] = {

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

float planeVertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  0.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 0.0f
};


    
const char* vertexPath = "res/shaders/basic.vert";
const char* fragmentPath = "res/shaders/basic.frag";
const char* fragmentPath_outline = "res/shaders/outline.frag";

bool firstMouseMovement = true;

float lastX = (float)WINDOW_WIDTH / 2.0;
float lastY = (float)WINDOW_HEIGHT / 2.0;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

Camera camera(0.f, 0.f, -3.f);

// Cursor teleport to the other side of the screen
float xCursorMargin = 30.0f;
float yCursorMargin = 30.0f;

const aiScene* dragon = aiImportFile("res/models/dragonModel.fbx", aiProcessPreset_TargetRealtime_MaxQuality);

// -- MAIN --

int main() {
    
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Ben-Gin Alpha Version", NULL, NULL);
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
    glfwSetCursorPosCallback(window, mouseCallback);

    glEnable(GL_DEPTH_TEST);

    PointLight light = {
        glm::vec3(0.0f, 2.0f, 0.0f), // position
        glm::vec3(0.1f, 0.1f, 0.1f), // ambient
        glm::vec3(0.7f, 0.7f, 0.7f), // diffuse
        glm::vec3(0.7f, 0.7f, 0.7f), // specular
        0.016f, 0.004f, 1.0f // quadratic, linear, constant
    };

    Shader *shader = new Shader(vertexPath, fragmentPath);
    Shader *shader_outline = new Shader(vertexPath, fragmentPath_outline);
    
    unsigned int VAO, VAO_plane, VBO, VBO_plane;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &VBO_plane);
    glGenVertexArrays(1, &VAO);
    glGenVertexArrays(1, &VAO_plane);

    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertices), boxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(VAO_plane);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_plane);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    int width, height, nrChannels;
    unsigned char* data = stbi_load("res/textures/stone.jpg", &width, &height, &nrChannels, 0);

    unsigned int woodTexture, stoneTexture, boxTexture_diff, boxTexture_spec, grassTexture;
    glGenTextures(1, &woodTexture);
    glGenTextures(1, &stoneTexture);
    glGenTextures(1, &boxTexture_diff);
    glGenTextures(1, &boxTexture_spec);
    glGenTextures(1, &grassTexture);

    glBindTexture(GL_TEXTURE_2D, woodTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    data = stbi_load("res/textures/wood.jpg", &width, &height, &nrChannels, 0);

    glBindTexture(GL_TEXTURE_2D, stoneTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    data = stbi_load("res/textures/box_diffuse.png", &width, &height, &nrChannels, 0);

    glBindTexture(GL_TEXTURE_2D, boxTexture_diff);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    data = stbi_load("res/textures/box_specular.png", &width, &height, &nrChannels, 0);

    glBindTexture(GL_TEXTURE_2D, boxTexture_spec);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    data = stbi_load("res/textures/grass.jpg", &width, &height, &nrChannels, 0);

    glBindTexture(GL_TEXTURE_2D, grassTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);




    int num = 4;
    int lightNum = 1;
    glm::mat4 *matrices = new glm::mat4[num + lightNum];
    int* textureIDs = new int[num];

    glm::mat4 model = glm::mat4(1.f);
    model = glm::translate(model, glm::vec3(4.f, 0.f, 0.f));
    model = glm::scale(model, glm::vec3(15, 1, 15));
    matrices[0] = model;

    for (int i = 0; i < num; i++) {
        model = glm::mat4(1.f);
        model = glm::translate(model, glm::vec3(4 * i, 0.f, 0.f));
        matrices[i + 1] = model;
    }
    float scaleFactor = 0.2f;
    
    matrices[num] = glm::translate(glm::mat4(1.f), light.position);
    matrices[num] = glm::scale(matrices[num], glm::vec3(scaleFactor));
    
    //light.position = glm::vec3(matrices[num] * glm::vec4(0, 0, 0, 1.f));
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
    shader_outline->use();
    glUniformMatrix4fv(glGetUniformLocation(shader_outline->ID, "projection"), 1, false, glm::value_ptr(projection));

    shader->use();
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "projection"), 1, false, glm::value_ptr(projection));

    std::cout << sizeof(dragon->mMeshes) / sizeof(dragon->mMeshes[0]) << std::endl;

    while (!glfwWindowShouldClose(window)) {
        

        glEnable(GL_STENCIL_TEST);
        glEnable(GL_DEPTH_TEST);

        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        changeMouse(window);

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
        float speed = 6.f;
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            light.position.x -= speed * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            light.position.x += speed * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            light.position.z += speed * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            light.position.z -= speed * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
            light.position.y += speed * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
            light.position.y -= speed * deltaTime;
        }

        matrices[num] = glm::translate(glm::mat4(1.f), light.position);
        matrices[num] = glm::scale(matrices[num], glm::vec3(scaleFactor));
        //matrices[num] = glm::scale(glm::mat4(1), glm::vec3(scaleFactor));
        //matrices[num] = glm::translate(matrices[num], light.position);
        //light.position = glm::vec3(matrices[num] * glm::vec4(0, 0, 0, 1.f));

        camera.ProcessKeyboard(deltaTime, direction);
		
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glm::mat4 view = camera.GetView();

        // unforms for ouline

        shader_outline->use();
        glUniformMatrix4fv(glGetUniformLocation(shader_outline->ID, "view"), 1, false, glm::value_ptr(view));

        shader_outline->setFloat("light.constant", light.constant);
        shader_outline->setFloat("light.linear", light.linear);
        shader_outline->setFloat("light.quadratic", light.quadratic);
        shader_outline->setVec3("light.position", light.position);
        shader_outline->setVec3("light.ambient", light.ambient);
        shader_outline->setVec3("light.diffuse", light.diffuse);
        shader_outline->setVec3("light.specular", light.specular);
        shader_outline->setVec3("cameraPosition", camera.cameraPos);

        shader->use();
        glUniformMatrix4fv(glGetUniformLocation(shader->ID, "view"), 1, false, glm::value_ptr(view));
        

        shader->setFloat("light.constant", light.constant);
        shader->setFloat("light.linear", light.linear);
        shader->setFloat("light.quadratic", light.quadratic);
        shader->setVec3("light.position", light.position);
        shader->setVec3("light.ambient", light.ambient);
        shader->setVec3("light.diffuse", light.diffuse);
        shader->setVec3("light.specular", light.specular);
        shader->setVec3("cameraPosition", camera.cameraPos);

        glClearColor(.01f, .01f, .01f, 1.0f);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // == stencil buffer ==

        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glStencilFunc(GL_ALWAYS, 1, 0xFF); 
        glStencilMask(0xFF);

        // == standard drawing ==

        shader->use();

        glActiveTexture(GL_TEXTURE0);

        glBindVertexArray(VAO_plane);
        glBindTexture(GL_TEXTURE_2D, grassTexture);

        glUniform1f(glGetUniformLocation(shader->ID, "material.shininess"), 64.f);
        glUniform1i(glGetUniformLocation(shader->ID, "material.diffuse_map"), 0);
        glUniform1i(glGetUniformLocation(shader->ID, "material.specular_map"), 0);
        glUniform1i(glGetUniformLocation(shader->ID, "isLight"), 0);


        glUniformMatrix4fv(glGetUniformLocation(shader->ID, "model"), 1, false, glm::value_ptr(matrices[0]));
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(VAO);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        glUniformMatrix4fv(glGetUniformLocation(shader->ID, "model"), 1, false, glm::value_ptr(matrices[1]));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindTexture(GL_TEXTURE_2D, stoneTexture);
        glUniformMatrix4fv(glGetUniformLocation(shader->ID, "model"), 1, false, glm::value_ptr(matrices[2]));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindTexture(GL_TEXTURE_2D, boxTexture_diff);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, boxTexture_spec);
        glUniform1i(glGetUniformLocation(shader->ID, "material.specular_map"), 1);
        glUniformMatrix4fv(glGetUniformLocation(shader->ID, "model"), 1, false, glm::value_ptr(matrices[3]));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // == stencil buffer ==

        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00); 
        glDisable(GL_DEPTH_TEST);

        // == drawing ouline ==
        shader_outline->use();

        glActiveTexture(GL_TEXTURE0);

        glBindVertexArray(VAO_plane);
        glBindTexture(GL_TEXTURE_2D, grassTexture);

        glUniform1f(glGetUniformLocation(shader_outline->ID, "material.shininess"), 64.f);
        glUniform1i(glGetUniformLocation(shader_outline->ID, "material.diffuse_map"), 0);
        glUniform1i(glGetUniformLocation(shader_outline->ID, "material.specular_map"), 0);
        glUniform1i(glGetUniformLocation(shader_outline->ID, "isLight"), 0);


        glUniformMatrix4fv(glGetUniformLocation(shader_outline->ID, "model"), 1, false, glm::value_ptr(glm::scale(matrices[0], glm::vec3(1.05f))));
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(VAO);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        glUniformMatrix4fv(glGetUniformLocation(shader_outline->ID, "model"), 1, false, glm::value_ptr(glm::scale(matrices[1], glm::vec3(1.05f))));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindTexture(GL_TEXTURE_2D, stoneTexture);
        glUniformMatrix4fv(glGetUniformLocation(shader_outline->ID, "model"), 1, false, glm::value_ptr(glm::scale(matrices[2], glm::vec3(1.05f))));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindTexture(GL_TEXTURE_2D, boxTexture_diff);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, boxTexture_spec);
        glUniform1i(glGetUniformLocation(shader_outline->ID, "material.specular_map"), 1);
        glUniformMatrix4fv(glGetUniformLocation(shader_outline->ID, "model"), 1, false, glm::value_ptr(glm::scale(matrices[3], glm::vec3(1.05f))));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glEnable(GL_DEPTH_TEST);

        // == end drawing outline ==

        //model swiatla
        glUniform1i(glGetUniformLocation(shader_outline->ID, "isLight"), 1);
        glUniformMatrix4fv(glGetUniformLocation(shader_outline->ID, "model"), 1, false, glm::value_ptr(matrices[num]));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete shader, shader_outline;
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

void changeMouse(GLFWwindow* window) {
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    if (mouseX <= xCursorMargin) {
        glfwSetCursorPos(window, windowWidth - xCursorMargin - 1, mouseY);
        lastX = windowWidth - xCursorMargin - 1;
    }
    else if (mouseX >= windowWidth - xCursorMargin) {
        glfwSetCursorPos(window, xCursorMargin + 1, mouseY);
        lastX = xCursorMargin + 1;
    }

    if (mouseY <= yCursorMargin) {
        glfwSetCursorPos(window, mouseX, windowHeight - yCursorMargin - 1);
        lastY = windowHeight - yCursorMargin - 1;

    }
    else if (mouseY >= windowHeight - yCursorMargin) {
        glfwSetCursorPos(window, mouseX, yCursorMargin + 1);
        lastY = yCursorMargin + 1;
    }
}

bool init() {
    if (!glfwInit()) return false;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    return true;
}
