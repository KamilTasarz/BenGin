/*
* 
*  KAMIL PILNUJ TEGO
* 
snake_case - zmienne
camelCase - metody
PascalCase - klasy/struktury

*/

/*

CHCIALEM TU TYLKO SPRECYZOWAC ZE JEBAC FREETYPE

*/

#include "src/Basic/Shader.h"
#include "src/Basic/Model.h"

#include "src/Component/Camera.h"
#include "src/Gameplay/Player.h"
#include "src/Text/Text.h"
#include "src/HUD/Background.h"
#include "src/HUD/Sprite.h"
#include "src/AudioEngine.h"

#include "src/System/ServiceLocator.h"
#include "src/System/Window.h"

#include "src/Input/InputManager.h"
#include "src/Input/Input.h"

#include "src/System/Serialization.h"

#include "Light.h"
#include "config.h"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

void changeMouse(GLFWwindow* window);
glm::vec4 getRayWorld(GLFWwindow* window);
void setLights(Shader* shader);
// void initializeServices();
void leftClick(float value);

string print(glm::vec3 v);

const char* vertexPath = "res/shaders/basic.vert";
const char* vertexPath_instanced = "res/shaders/instanced.vert";
const char* fragmentPath = "res/shaders/basic.frag";
const char* vertexPath_shadow = "res/shaders/shadow.vert";
const char* fragmentPath_shadow = "res/shaders/shadow.frag";
const char* fragmentPath_outline = "res/shaders/outline.frag";
const char* triangleVertexPath = "res/shaders/triangle.vert";
const char* triangleFragmentPath = "res/shaders/triangle.frag";
const char* vertexPath_text = "res/shaders/text.vert";
const char* fragmentPath_text = "res/shaders/text.frag";
const char* fragmentPath_background = "res/shaders/background.frag";

// Audio paths
string track1 = "res/audios/music/kill-v-maim.ogg";
string track2 = "res/audios/music/burning-bright.ogg";

string sound_effect = "res/audios/sounds/bonk.ogg";

float pauseTimer = 0.0f;
float pauseCooldown = 0.5f; // Half second cooldown

//

glm::mat4* matrices;
glm::vec3* minPoints;
glm::vec3* maxPoints;
glm::mat4 view;
glm::mat4 projection;

std::vector<BoundingBox*> colliders;

float fps = 0.0f; // Current FPS
float fps_timer = 0.0f;
int frames = 0;

Node rootNode("rootNode");

// Cursor teleport to the other side of the screen
float xCursorMargin = 30.0f;
float yCursorMargin = 30.0f;

Player *player;

unsigned int point_light_number = 1;
unsigned int directional_light_number = 1;

DirectionalLight *directional_lights;
PointLight *point_lights;

Text* text;
Background* background;
Sprite *sprite, *sprite2, *sprite3;

std::vector<Model> models;

int main() {

	// --- !!! THIS SHOULD ALL HAPPEN IN GAME/ENGINE CLASS (FRIEND TO INPUT MANAGER) - ESCPECIALLY PROCESS INPUT !!! --- //

    // --- REGISTER INPUT DEVICES AND SOME CALLBACKS --- //
    // Kind of a test for now - create services (InputManager only for now)

    ///

    Window* _window = new Window(WINDOW_WIDTH, WINDOW_HEIGHT, "Ben-Gin Alpha Version 1.1.1");

    ServiceLocator::provide(_window);
    
    auto* window = ServiceLocator::getWindow();

    // --- //

    text = new Text("res/fonts/arial.ttf");
    background = new Background(1920.f, 1080.f, "res/textures/sky.png", 200.f);

    const char* sprites[] = {"res/sprites/ghostFlying1.png", "res/sprites/ghostFlying2.png",
                             "res/sprites/ghostFlying3.png", "res/sprites/ghostFlying4.png",
                             "res/sprites/ghostFlying5.png", "res/sprites/ghostFlying6.png" };

    
    sprite = new AnimatedSprite(1920.f, 1080.f, 1.f, sprites, 6, 100.f, 300.f);
    sprite3 = new AnimatedSprite(1920.f, 1080.f, 1.f, "res/sprites/piratWalking.png", 1, 9, 9, 300.f, 300.f);
    sprite2 = new Sprite(1920.f, 1080.f, "res/sprites/heart2.png", 700.f, 100.f, 0.1f);

	rootNode.transform.setLocalPosition({ 0.0f, 0.0f, 0.0f });

    // --- AUDIO INIT --- //
    // Initialize our audio engine
    CAudioEngine audioEngine;
    audioEngine.Init();

    // Load two music tracks and one sound effect
    audioEngine.LoadSound(track1, true, true, true);
    audioEngine.LoadSound(track2, true, true, true);
    audioEngine.LoadSound(sound_effect, false, false, true);

    int current_track_id = audioEngine.PlaySounds(track1, Vector3{ 0.0f }, -10.0);
    bool paused = false;

    // When we want to call PlaySounds and don't care about the channel number
    int useless_garbage;

    // --- //

    // --- IMGUI INIT --- //

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window->window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    Shader* shader = new Shader(vertexPath, fragmentPath);
    Shader* shader_instanced = new Shader(vertexPath_instanced, fragmentPath);
    Shader* shader_instanced_outline = new Shader(vertexPath_instanced, fragmentPath_outline);
    Shader* shader_outline = new Shader(vertexPath, fragmentPath_outline);
    Shader* shader2D = new Shader(triangleVertexPath, triangleFragmentPath);
    Shader* shader_shadow = new Shader(vertexPath_shadow, fragmentPath_shadow);
    Shader* shader_text = new Shader(vertexPath_text, fragmentPath_text);
    Shader* shader_background = new Shader(vertexPath_text, fragmentPath_background);

    // --- //

    /*Model Tmodel("res/models/nanosuit2/nanosuit2.obj", 0);
    Model Kmodel("res/models/kutasiarz/The_Thing.obj", 1);

    const char* box_spec = "res/textures/box_specular.png", * box_diff = "res/textures/box_diffuse.png",
        * stone_name = "res/textures/stone.jpg", * wood_name = "res/textures/wood.jpg", * grass_name = "res/textures/grass.jpg";

    const char* texture_names[] = { box_spec, box_diff };
    Model Tmodel_box_diff_spec(texture_names, 2, 2);

    *texture_names = { wood_name };
    Model Tmodel_box_wood(texture_names, 1, 3);

    *texture_names = { stone_name };
    Model Tmodel_box_stone(texture_names, 1, 4);

    Model Tmodel_light(texture_names, 0, 5);

    *texture_names = { grass_name };
    Model Tmodel_plane(texture_names, 1, 6, "plane");

	models.push_back(Kmodel);
	models.push_back(Tmodel);
	models.push_back(Tmodel_box_diff_spec);
	models.push_back(Tmodel_box_wood);
	models.push_back(Tmodel_box_stone);
	models.push_back(Tmodel_light);
	models.push_back(Tmodel_plane);


    Node* kutasiarz = new Node(Tmodel, "kutasiarz", colliders, false, 0, glm::vec3(-2.f, -3.f, -2.f), glm::vec3(2.f, 3.f, 2.f));
    Node* cos = new Node(Kmodel, "cos", colliders, false, 0, glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    Node* box_light = new Node(Tmodel_light, "light", true);
    Node* box_light2 = new Node(Tmodel_light, "light2", true);
    Node* box_light_directional = new Node(Tmodel_light, "light_directional", true);
    Node* box_diff_spec = new Node(Tmodel_box_diff_spec, "box1", colliders);
    Node* box_diff_spec2 = new Node(Tmodel_box_diff_spec, "box1_2", colliders);
    Node* box_wood = new Node(Tmodel_box_wood, "box2", colliders);
    Node* box_stone = new Node(Tmodel_box_stone, "box3", colliders);

    Node* plane = new Node(Tmodel_plane, "plane1", colliders, false, 0, glm::vec3(-0.5f, 0.0f, -0.5f), glm::vec3(0.5f, 0.0f, 0.5f));

    //InstanceManager* walls = new InstanceManager(Tmodel_box_diff_spec, "instance_manager_wall", shader_instanced, 20);

    player = new Player(cos, 3.f, 3.f, 10.f);

	camera->setObjectToFollow(player->player_node, glm::vec3(0.f, 2.f, 0.f));

    rootNode.addChild(box_wood);
    rootNode.addChild(box_stone);
    rootNode.addChild(box_light);
    rootNode.addChild(box_light2);
    rootNode.addChild(box_light_directional);
    rootNode.addChild(kutasiarz);
    rootNode.addChild(cos);
    rootNode.addChild(plane);
    //rootNode.addChild(walls);

    kutasiarz->transform.setLocalPosition({3.f, 2.f, 3.f});
    kutasiarz->transform.setLocalScale({ 0.3f, 0.3f, 0.3f });

    cos->transform.setLocalPosition({ -5.0, 0.5f, -5.0f });
    cos->transform.setLocalScale({ 2.0, 2.0f, 2.0f });

    box_diff_spec->transform.setLocalPosition({ 7.5f, 0.0f, 0.0f });
    box_diff_spec->transform.setLocalScale({ 1.0f, 3.0f, 14.0f });

    box_diff_spec2->transform.setLocalPosition({ 0.0f, 0.0f, -7.5f });
    box_diff_spec2->transform.setLocalScale({ 16.0f, 3.0f, 1.0f });

    box_wood->transform.setLocalPosition({ -7.5f, 0.0f, 0.0f });
    box_wood->transform.setLocalScale({ 1.0f, 3.0f, 14.0f });

    box_stone->transform.setLocalPosition({ 0.0f, 0.0f, 7.5f });
    box_stone->transform.setLocalScale({ 16.0f, 3.0f, 1.0f });

    box_light->transform.setLocalPosition({ 0.0f, 2.0f, 0.0f });
    box_light->transform.setLocalScale({ 0.3f, 0.3f, 0.3f });

    box_light2->transform.setLocalPosition({ 8.0f, 4.0f, -8.0f });
    box_light2->transform.setLocalScale({ 0.3f, 0.3f, 0.3f });

    box_light_directional->transform.setLocalPosition({ -8.0f, 4.0f, -8.0f });
    box_light_directional->transform.setLocalScale({ 0.3f, 0.3f, 0.3f });

    plane->transform.setLocalPosition({ 0.0f, -0.501f, 0.0f }); // z - fighting
    plane->transform.setLocalScale({ 15.f, 15.0f, 15.f });

   /* walls->addChild(box_diff_spec);
    walls->addChild(box_diff_spec2);

    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 2; j++) {
            Node* temp = new Node(Tmodel_box_diff_spec, "inst" + to_string(i), colliders);
            temp->transform.setLocalPosition({ i + 4.f, 2.f, 4.f * j + 4.f });
            temp->transform.computeModelMatrix();
            walls->addChild(temp);
        }
    }*/

    point_lights = new PointLight[10];
    directional_lights = new DirectionalLight[10];

    /*point_lights[0] = PointLight(box_light, 0.032f, 0.09f);
    point_lights[1] = PointLight(box_light2, 0.032f, 0.09f);
    directional_lights[0] = DirectionalLight(box_light_directional, glm::vec3(1.f, -1.f, 1.f));*/

	loadScene("res/scene/scene.json", &rootNode, player, camera, point_lights, point_light_number, directional_lights, directional_light_number, models, colliders);

    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    
    projection = glm::perspective(glm::radians(30.f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.5f, 100.0f);

    shader_outline->use();
    shader_outline->setMat4("projection", projection);

    shader_instanced->use();
    shader_instanced->setMat4("projection", projection);
    shader_instanced->setInt("point_light_number", point_light_number);
    shader_instanced->setInt("directional_light_number", directional_light_number);

    shader->use();
    shader->setMat4("projection", projection);
    shader->setInt("point_light_number", point_light_number);
    shader->setInt("directional_light_number", directional_light_number);

    //shader->setFloat("shininess", 64.f); //ustawione na stałe w shaderze, bo i tak nie zmienialiśmy

    rootNode.updateSelfAndChild(true);

    // 2d zadanie

    float rectangleVertices[] = {
        // positions
        -0.8f, -0.6f,
         -0.8f, -0.8f,
         -0.6f,  -0.6f,

         -0.8f, -0.8f,
         -0.6f, -0.8f,
         - 0.6f, -0.6f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // --- GAME LOOP --- //

    while (!glfwWindowShouldClose(window->window)) {

        unsigned int dis, tot;

        glEnable(GL_STENCIL_TEST);
        glEnable(GL_DEPTH_TEST);

        // Delta time and fps calculation

        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        frames++;
        fps_timer += deltaTime;
        if (fps_timer >= 1.0f) {
            fps = frames / fps_timer;
            fps_timer = 0.0f;
            frames = 0;
        }
        
        // Move to main game loop
        if (ServiceLocator::getInputManager()) {
            ServiceLocator::getInputManager()->processInput();
        }

        // --- //

        // Audio control section (just temporarily hardcoded)
        audioEngine.Update();

        if (glfwGetKey(window->window, GLFW_KEY_1) == GLFW_PRESS) {
            audioEngine.stopSound(current_track_id);
            current_track_id = audioEngine.PlaySounds(track1, Vector3{0.0f}, -11.0);
        }
        if (glfwGetKey(window->window, GLFW_KEY_2) == GLFW_PRESS) {
            audioEngine.stopSound(current_track_id);
            current_track_id = audioEngine.PlaySounds(track2, Vector3{ 0.0f }, -11.0);
        }

        // Pausing/resuming
        if (glfwGetKey(window->window, GLFW_KEY_3) == GLFW_PRESS && pauseTimer <= 0.0f) {
            if (paused) {
                audioEngine.resumeSound(current_track_id);
            } else {
                audioEngine.pauseSound(current_track_id);
            }
            paused = !paused;
            pauseTimer = pauseCooldown;
        }

        if (glfwGetKey(window->window, GLFW_KEY_4) == GLFW_PRESS) {
            useless_garbage = audioEngine.PlaySounds(sound_effect);
        }

        if (pauseTimer > 0.0f) {
            pauseTimer -= deltaTime;
        }

        // --- //

        if (glfwGetKey(window->window, GLFW_KEY_C) == GLFW_PRESS) {
            if (is_camera == is_camera_prev) {
                is_camera = !is_camera;
                camera->changeMode(static_cast<CameraMode>((camera->mode + 1) % 3));
            }
        } else {
            is_camera_prev = is_camera;
        }

        int direction = 0;
        if (glfwGetKey(window->window, GLFW_KEY_W) == GLFW_PRESS)
            direction += 1;
        if (glfwGetKey(window->window, GLFW_KEY_S) == GLFW_PRESS)
            direction += 2;
        if (glfwGetKey(window->window, GLFW_KEY_A) == GLFW_PRESS)
            direction += 4;
        if (glfwGetKey(window->window, GLFW_KEY_D) == GLFW_PRESS)
            direction += 8;
        if (glfwGetKey(window->window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            direction += 16;

        }
        if (glfwGetKey(window->window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            direction += 32;
        }

        point_lights[0].updatePosition();

        float speed = 6.f;
        if (glfwGetKey(window->window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            point_lights[0].position += camera->cameraRight * speed * deltaTime;
        }
        if (glfwGetKey(window->window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            point_lights[0].position -= camera->cameraRight * speed * deltaTime;
        }
        if (glfwGetKey(window->window, GLFW_KEY_UP) == GLFW_PRESS) {
            point_lights[0].position += camera->cameraFront * speed * deltaTime;
        }
        if (glfwGetKey(window->window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            point_lights[0].position -= camera->cameraFront * speed * deltaTime;
        }
        if (glfwGetKey(window->window, GLFW_KEY_O) == GLFW_PRESS) {
            point_lights[0].position += camera->cameraUp * speed * deltaTime;
        }
        if (glfwGetKey(window->window, GLFW_KEY_L) == GLFW_PRESS) {
            point_lights[0].position -= camera->cameraUp * speed * deltaTime;
        }

        point_lights[0].setModelPosition();
        rootNode.updateSelfAndChild(false);

        if (camera->mode != FREE) {
            player->update(deltaTime, direction, camera->Yaw);
        }
        
        

        for (auto&& collider : colliders) {
            
            if (player->player_node->AABB != collider && player->player_node->AABB->isBoundingBoxIntersects(*collider)) {

               
                player->player_node->separate(collider);
                
            }
        }

        
        

        //if (is_camera) {
            camera->ProcessKeyboard(deltaTime, direction);
            changeMouse(window->window);
        //}
		
        if (glfwGetKey(window->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window->window, true);

        if (glfwGetMouseButton(window->window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
            mouse_pressed = true;
        } else {
            mouse_pressed = false;
        }

        view = camera->GetView();

        // unforms for ouline

        shader_outline->use();
        shader_outline->setMat4("view", view);   
        shader_instanced->use();
        shader_instanced->setMat4("view", view);
        setLights(shader_instanced);

        shader->use();
        shader->setMat4("view", view);
        
        setLights(shader);

        rootNode.updateSelfAndChild(false);

        //renderowanie pod cienie
        /*point_lights[0].render(depthMapFBO, *shader_shadow);
        glClear(GL_DEPTH_BUFFER_BIT);
        rootNode.drawShadows(*shader_shadow);
        point_lights[0].renderBack(depthMapFBO, *shader_shadow);
        glClear(GL_DEPTH_BUFFER_BIT);
        rootNode.drawShadows(*shader_shadow);*/

        directional_lights[0].render(depthMapFBO, *shader_shadow);
        glClear(GL_DEPTH_BUFFER_BIT);
        rootNode.drawShadows(*shader_shadow);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        glClearColor(.01f, .01f, .01f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


        background->update(deltaTime);
        background->render(*shader_background);

        shader_instanced->use();
        shader_instanced->setMat4("view", view);
        shader_instanced->setMat4("projection", projection);
        //shader_instanced->setMat4("light_view_projection", point_lights[0].getMatrix());
        //shader_instanced->setMat4("light_view_projection_back", point_lights[0].view_projection_back);
        shader_instanced->setMat4("light_view_projection", directional_lights[0].getMatrix());
        shader_instanced->setInt("shadow_map", 3);
        //shader_instanced->setInt("shadow_map3", 5);
        //shader_instanced->setInt("shadow_map_back", 4);

        shader->use();
        shader->setMat4("view", view);
        shader->setMat4("projection", projection);
        //shader->setMat4("light_view_projection", point_lights[0].getMatrix());
        //shader->setMat4("light_view_projection_back", point_lights[0].view_projection_back);
        shader->setMat4("light_view_projection", directional_lights[0].getMatrix());
        shader->setInt("shadow_map", 3);
        //shader->setInt("shadow_map3", 5);
        //shader->setInt("shadow_map_back", 4);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, directional_lights[0].depthMap);
        //glActiveTexture(GL_TEXTURE4);
        //glBindTexture(GL_TEXTURE_2D, point_lights[0].depthMapBack);
        //glActiveTexture(GL_TEXTURE5);
        //glBindTexture(GL_TEXTURE_2D, directional_lights[0].depthMap);

        float t = FLT_MAX;
        rootNode.new_marked_object = nullptr;
        rootNode.mark(getRayWorld(window->window), rootNode.new_marked_object, t, camera->cameraPos);

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

        rootNode.drawSelfAndChild(*shader, *shader_outline, dis, tot);
        // == outline ==

        shader_outline->use();
        rootNode.drawMarkedObject(*shader_outline);
        float fps = 1.f / deltaTime;
        


        text->renderText("Fps: " + to_string(fps), 4.f * WINDOW_WIDTH / 5.f, WINDOW_HEIGHT - 100.f, *shader_text, glm::vec3(1.f, 0.3f, 0.3f));
        text->renderText("We have text render!", 200, 200, *shader_text, glm::vec3(0.6f, 0.6f, 0.98f));
        sprite->update(deltaTime);
        sprite3->update(deltaTime);
        
        sprite2->render(*shader_background);
        sprite3->render(*shader_background);
        sprite->render(*shader_background);
        


        // Render the 2D triangle
        /*glDisable(GL_DEPTH_TEST); // Disable depth test to render on top
        shader2D->use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glEnable(GL_DEPTH_TEST);*/

        // ---------------------

        // ImGui Quick Test
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Window content
        ImGui::Begin("Test Window");
        ImGui::Text("Test test test test 123 123");
        ImGui::Text("Current FPS: %.1f", fps); // Dipslay current fps
        ImGui::End();

        // Window render
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // ---------------------
    
        window->updateWindow();

    }

	saveScene("res/scene/scene.json", &rootNode, player, camera, point_lights, point_light_number, directional_lights, directional_light_number, models);

    // Audio engine cleanup
    audioEngine.Shutdown();

    // Nullptr to the window and input manager
    ServiceLocator::shutdownServices();

    //ImGui cleanup
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();

    delete shader, shader_outline, shader2D;
    delete[] matrices, point_lights, directional_lights;
    glfwTerminate();
    return 0;
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

    glm::vec4 rayClip = glm::vec4(normalizedMouse.x, normalizedMouse.y, -1.0f, 1.0f);
    glm::vec4 rayEye = glm::inverse(projection) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
    glm::vec4 rayWorld = glm::normalize(glm::inverse(view) * rayEye);

    return rayWorld;
}

string print(glm::vec3 v) {
    return std::to_string(v.x) + " " + std::to_string(v.y) + " " + std::to_string(v.z);
}

void setLights(Shader* shader) {

    shader->setVec3("cameraPosition", camera->cameraPos);
    for (int i = 0; i < point_light_number; i++) {
        string index = to_string(i);
        point_lights[i].updatePosition();
        shader->setFloat("point_lights["+index+"].constant", point_lights[i].constant);
        shader->setFloat("point_lights["+index+"].linear", point_lights[i].linear);
        shader->setFloat("point_lights["+index+"].quadratic", point_lights[i].quadratic);
        shader->setVec3("point_lights["+index+"].position", point_lights[i].position);
        shader->setVec3("point_lights["+index+"].ambient", point_lights[i].ambient);
        shader->setVec3("point_lights["+index+"].diffuse", point_lights[i].diffuse);
        shader->setVec3("point_lights["+index+"].specular", point_lights[i].specular);
    }

    for (int i = 0; i < directional_light_number; i++) {
        string index = to_string(i);
        shader->setVec3("directional_lights[" + index + "].direction", directional_lights[i].direction);
        shader->setVec3("directional_lights[" + index + "].ambient", directional_lights[i].ambient);
        shader->setVec3("directional_lights[" + index + "].diffuse", directional_lights[i].diffuse);
        shader->setVec3("directional_lights[" + index + "].specular", directional_lights[i].specular);
    }

}

//void initializeServices() {
//
//    // Provide a window
//    ServiceLocator::provide(new Window(1920, 1080, "Ben-Gin Alpha Version 1.1"));
//
//    // Provide an Input Manager
//    ServiceLocator::provide(new InputManager());
//
//}
