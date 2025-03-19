/*
* 
*  KAMIL PILNUJ TEGO
* 
snake_case - zmienne
camelCase - metody
PascalCase - klasy/struktury

*/

#include "config.h"

#include "Shader.h"
#include "Camera.h"
#include "src/Basic/Model.h"
#include "src/Basic/Node.h"


#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double posX, double posY);
void changeMouse(GLFWwindow* window);
glm::vec4 getRayWorld(GLFWwindow* window);

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

   
const char* vertexPath = "res/shaders/basic.vert";
const char* fragmentPath = "res/shaders/basic.frag";
const char* fragmentPath_outline = "res/shaders/outline.frag";

glm::mat4* matrices;
glm::vec3* minPoints;
glm::vec3* maxPoints;
glm::mat4 view;
glm::mat4 projection;

std::vector<BoundingBox*> colliders;

bool is_camera = true, is_camera_prev = false;
bool mouse_pressed = false;

bool firstMouseMovement = true;

float lastX = (float)WINDOW_WIDTH / 2.0;
float lastY = (float)WINDOW_HEIGHT / 2.0;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

Node rootNode("rootNode", colliders);
Camera camera(0.f, 0.f, -3.f);

// Cursor teleport to the other side of the screen
float xCursorMargin = 30.0f;
float yCursorMargin = 30.0f;

//const aiScene* dragon = aiImportFile("res/models/dragon1/Dragon_2.5_For_Animations.obj", aiProcessPreset_TargetRealtime_MaxQuality);

void checkFMODResult(FMOD_RESULT result) {
    if (result != FMOD_OK) {
        std::cerr << "FMOD error! Code: " << result << std::endl;
        exit(-1);  // Zakończ program w przypadku błędu
    }
}

// -- MAIN --

int main() {
    
    FMOD::System* system;
    FMOD_RESULT TWOJA_STARUCHA;

    // Tworzenie systemu FMOD
    TWOJA_STARUCHA = FMOD::System_Create(&system);
    checkFMODResult(TWOJA_STARUCHA);

    // Inicjalizacja systemu
    TWOJA_STARUCHA = system->init(512, FMOD_INIT_NORMAL, nullptr);
    checkFMODResult(TWOJA_STARUCHA);

    // Pobranie wersji FMOD
    unsigned int version;
    TWOJA_STARUCHA = system->getVersion(&version);
    checkFMODResult(TWOJA_STARUCHA);

    std::cout << "FMOD działa! Wersja: "
        << (version >> 16) << "."
        << ((version >> 8) & 0xFF) << "."
        << (version & 0xFF) << std::endl;

    // Zwolnienie zasobów
    system->release();

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Ben-Gin Alpha Version 1.01", NULL, NULL);
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

    // -- CULLING -- //

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glEnable(GL_MULTISAMPLE);

    PointLight light = {
        glm::vec3(0.0f, 2.0f, 0.0f), // position
        glm::vec3(0.1f, 0.1f, 0.1f), // ambient
        glm::vec3(0.7f, 0.7f, 0.7f), // diffuse
        glm::vec3(0.7f, 0.7f, 0.7f), // specular
        0.016f, 0.004f, 1.0f // quadratic, linear, constant
    };

    rootNode.transform.setLocalPosition({0.0f, 0.0f, 0.0f});
    rootNode.transform.setLocalScale({1.0f, 1.0f, 1.0f});

    Model Tmodel("res/models/nanosuit2/nanosuit2.obj");
    Model Kmodel("res/models/kutasiarz/The_Thing.obj");
        
    const char *box_spec = "res/textures/box_specular.png", *box_diff = "res/textures/box_diffuse.png",
    *stone_name = "res/textures/stone.jpg", *wood_name = "res/textures/wood.jpg", *grass_name = "res/textures/grass.jpg";

    const char* texture_names[] = { box_spec, box_diff };
    Model Tmodel_box_diff_spec(texture_names, 2);

    *texture_names = { wood_name };
    Model Tmodel_box_wood(texture_names, 1);

    *texture_names = { stone_name };
    Model Tmodel_box_stone(texture_names, 1);

    Model Tmodel_light(texture_names, 0);

    *texture_names = { grass_name };
    Model Tmodel_plane(texture_names, 1, "plane");
    
    Node* kutasiarz = new Node(Tmodel, "kutasiarz", colliders, false, 0, glm::vec3( - 2.f, -3.f, -2.f ), glm::vec3(2.f, 3.f, 2.f));
    Node* cos = new Node(Kmodel, "cos", colliders, false, 0, glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    Node* box_diff_spec = new Node(Tmodel_box_diff_spec, "box1", colliders);
    Node* box_wood = new Node(Tmodel_box_wood, "box2", colliders);
    Node* box_stone = new Node(Tmodel_box_stone, "box3", colliders);
    Node* box_light = new Node(Tmodel_light, "light", true);
    Node* plane = new Node(Tmodel_plane, "plane1", colliders, false, 0, glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, -0.5f, 0.5f));

    rootNode.addChild(box_diff_spec);
    rootNode.addChild(box_wood);
    rootNode.addChild(box_stone);
    rootNode.addChild(box_light);
    rootNode.addChild(kutasiarz);
    rootNode.addChild(cos);
    rootNode.addChild(plane);

    kutasiarz->transform.setLocalPosition({ 3.f, 1.f, 3.f });
    kutasiarz->transform.setLocalScale({ 0.3f, 0.3f, 0.3f });

    cos->transform.setLocalPosition({-5.0, 0.0f, -5.0f});
    cos->transform.setLocalScale({2.0, 2.0f, 2.0f});

    box_diff_spec->transform.setLocalPosition({0.0f, 0.0f, 0.0f});
    box_diff_spec->transform.setLocalScale({ 1.0f, 1.0f, 1.0f });

    box_wood->transform.setLocalPosition({ 4.0f, 0.0f, 0.0f });

    box_stone->transform.setLocalPosition({ 8.0f, 0.0f, 0.0f });

    box_light->transform.setLocalPosition( light.position );
    box_light->transform.setLocalScale({ 0.3f, 0.3f, 0.3f });

    plane->transform.setLocalPosition({ 4.0f, -0.001f, 0.0f }); // z - fighting
    plane->transform.setLocalScale({ 15.f, 1.0f, 15.f });

    Shader *shader = new Shader(vertexPath, fragmentPath);
    Shader *shader_outline = new Shader(vertexPath, fragmentPath_outline);
    
    projection = glm::perspective(glm::radians(30.f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.5f, 100.0f);

    shader_outline->use();
    glUniformMatrix4fv(glGetUniformLocation(shader_outline->ID, "projection"), 1, false, glm::value_ptr(projection));

    shader->use();
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "projection"), 1, false, glm::value_ptr(projection));

    rootNode.updateSelfAndChild(true);

    while (!glfwWindowShouldClose(window)) {
        

        glEnable(GL_STENCIL_TEST);
        glEnable(GL_DEPTH_TEST);

        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        

        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
            if (is_camera == is_camera_prev) {
                is_camera = !is_camera;
            }
        } else {
            is_camera_prev = is_camera;
        }

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

        box_light->transform.setLocalPosition(light.position);

        for (auto&& collider : colliders) {
            if (box_light->AABB->isBoundingBoxIntersects(*collider)) {
                cout << "Kolizja" << endl;
            }
        }

        if (is_camera) {
            camera.ProcessKeyboard(deltaTime, direction);
            changeMouse(window);
        }
        
		
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
            mouse_pressed = true;
        } else {
            mouse_pressed = false;
        }


        view = camera.GetView();

        // unforms for ouline

        shader_outline->use();
        glUniformMatrix4fv(glGetUniformLocation(shader_outline->ID, "view"), 1, false, glm::value_ptr(view));


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
        shader->setFloat("shininess", 64.f);

        //int index = findObjectToMark(window);
        
        glClearColor(.01f, .01f, .01f, 1.0f);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        rootNode.updateSelfAndChild(false);
        float t = FLT_MAX;
        rootNode.new_marked_object = nullptr;
        rootNode.mark(getRayWorld(window), rootNode.new_marked_object, t, camera.cameraPos);

        if (mouse_pressed) {
            if (rootNode.marked_object != nullptr) rootNode.marked_object->is_marked = false;

            rootNode.marked_object = rootNode.new_marked_object;

            if (rootNode.marked_object != nullptr) rootNode.marked_object->is_marked = true;
        }

        // == stencil buffer ==

        glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0x00);

        // == standard drawing ==

        unsigned int dis, tot;
        rootNode.drawSelfAndChild(*shader, dis, tot);

        // == outline ==

        shader_outline->use();
        rootNode.drawMarkedObject(*shader_outline);

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

    if (is_camera) {
        camera.ProcessMouseMovement(offsetX, offsetY);
    }
    
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



glm::vec4 getRayWorld(GLFWwindow* window) {
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    glm::vec2 normalizedMouse;
    normalizedMouse.x = (2.0f * mouseX) / WINDOW_WIDTH - 1.0f;
    normalizedMouse.y = 1.0f - (2.0f * mouseY) / WINDOW_HEIGHT;

    glm::vec4 rayClip = glm::vec4(normalizedMouse.x, normalizedMouse.y, 0.0f, 1.0f);
    glm::vec4 rayEye = glm::inverse(projection) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
    glm::vec4 rayWorld = glm::normalize(glm::inverse(view) * rayEye);

    
    return rayWorld;
}
