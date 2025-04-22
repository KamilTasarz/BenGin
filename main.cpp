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
#include "src/Basic/Animator.h"
#include "src/Gameplay/Player.h"

//#include "src/Component/CameraGlobals.h"
#include "src/Text/Text.h"
#include "src/HUD/Background.h"
#include "src/HUD/Sprite.h"
#include "src/AudioEngine.h"

#include "src/System/ServiceLocator.h"
#include "src/System/Window.h"

#include "src/Input/InputManager.h"
#include "src/Input/Input.h"

#include "src/System/Serialization.h"

#include "config.h"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

void changeMouse(GLFWwindow* window);
glm::vec4 getRayWorld(GLFWwindow* window, const glm::mat4& _view, const glm::mat4& _projection);
//void setLights(Shader* shader);
// void initializeServices();
void leftClick(float value);
void BeginFullscreenInputLayer();
void DrawSceneNode(Node* node, int depth = 0);
void DrawNodeBlock(Node* node, int depth = 0);
void DrawHierarchyWindow(Node* root);

string print(glm::vec3 v);

//

// Audio paths
string track1 = "res/audios/music/kill-v-maim.ogg";
string track2 = "res/audios/music/burning-bright.ogg";

string sound_effect = "res/audios/sounds/bonk.ogg";

float pauseTimer = 0.0f;
float pauseCooldown = 0.5f; // Half second cooldown

//

//glm::mat4* matrices;
//glm::vec3* minPoints;
//glm::vec3* maxPoints;
//glm::mat4 view;
//glm::mat4 projection;

std::vector<BoundingBox*> colliders;

ImGuizmo::OPERATION currentOperation(ImGuizmo::TRANSLATE);
bool uniformScale = false;

float fps = 0.0f; // Current FPS
float fps_timer = 0.0f;
int frames = 0;

SceneGraph* sceneGraph;

// Cursor teleport to the other side of the screen
float xCursorMargin = 10.0f;
float yCursorMargin = 10.0f;

Player *player;

Text* text;
Background* background;
Sprite *sprite, *sprite2, *sprite3;

std::vector<Model> models;

int previewX = WINDOW_WIDTH / 6;
int previewY = WINDOW_HEIGHT / 3;
int previewWidth = 2 * WINDOW_WIDTH / 3;
int previewHeight = 2 * WINDOW_HEIGHT / 3;

int main() {

	// --- !!! THIS SHOULD ALL HAPPEN IN GAME/ENGINE CLASS (FRIEND TO INPUT MANAGER) - ESCPECIALLY PROCESS INPUT !!! --- //

    // --- REGISTER INPUT DEVICES AND SOME CALLBACKS --- //
    // Kind of a test for now - create services (InputManager only for now)

    ///

    Window* _window = new Window(WINDOW_WIDTH, WINDOW_HEIGHT, "Ben-Gin Alpha Version 1.1.2");

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

    //rootNode = new Node("root");

	//rootNode->transform.setLocalPosition({ 0.0f, 0.0f, 0.0f });

    // --- AUDIO INIT --- //
    // Initialize our audio engine
    CAudioEngine audioEngine;
    audioEngine.Init();

    // Load two music tracks and one sound effect
    audioEngine.LoadSound(track1, true, true, true);
    audioEngine.LoadSound(track2, true, true, true);
    audioEngine.LoadSound(sound_effect, false, false, true);

    int current_track_id = 0;//audioEngine.PlaySounds(track1, Vector3{ 0.0f }, -10.0);
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

	sceneGraph = new SceneGraph();   

    Model Tmodel("res/models/nanosuit2/nanosuit2.obj", 0);
    Model Kmodel("res/models/kutasiarz/The_Thing.obj", 1);
    Model Lmodel("res/models/man/CesiumMan2.gltf", 2);    
    

    //Model Lmodel("res/models/ludzik/ludzik.gltf");
    //const char* anim_path = "res/models/ludzik/ludzik.gltf";
    
	//anim->speed *= 2.f;


    const char* box_spec = "res/textures/box_specular.png", * box_diff = "res/textures/box_diffuse.png",
        * stone_name = "res/textures/stone.jpg", * wood_name = "res/textures/wood.jpg", * grass_name = "res/textures/grass.jpg";

    const char* texture_names[] = { box_spec, box_diff };
    Model Tmodel_box_diff_spec(texture_names, 2, 3);

    *texture_names = { wood_name };
    Model Tmodel_box_wood(texture_names, 1, 4);

    *texture_names = { stone_name };
    Model Tmodel_box_stone(texture_names, 1, 5);

    Model Tmodel_light(texture_names, 0, 6);

    *texture_names = { grass_name };
    Model Tmodel_plane(texture_names, 1, 7);

    models.push_back(Tmodel);
	models.push_back(Kmodel);
	models.push_back(Lmodel);
	models.push_back(Tmodel_box_diff_spec);
	models.push_back(Tmodel_box_wood);
	models.push_back(Tmodel_box_stone);
	models.push_back(Tmodel_light);
	models.push_back(Tmodel_plane);

    Node* kutasiarz = new Node(Tmodel, "kutasiarz", colliders, 0, glm::vec3(-2.f, -3.f, -2.f), glm::vec3(2.f, 3.f, 2.f));
    Node* cos = new Node(Kmodel, "cos", colliders, 0, glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    Node* ludzik = new Node(Lmodel, "ludzik", colliders, 0, glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f));



    Node* box_diff_spec = new Node(Tmodel_box_diff_spec, "box1", colliders);
    Node* box_diff_spec2 = new Node(Tmodel_box_diff_spec, "box1", colliders);
    Node* box_wood = new Node(Tmodel_box_wood, "box2", colliders);
    Node* box_stone = new Node(Tmodel_box_stone, "box3", colliders);
    //Node* plane = new Node(Tmodel_plane, "plane1", colliders, false, 0);
    Node* plane = new Node(Tmodel_plane, "plane1", colliders, 0, glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f));

    player = new Player(ludzik, 3.f, 3.f, 10.f);

    sceneGraph->addChild(box_diff_spec);
    sceneGraph->addChild(box_diff_spec2);
    sceneGraph->addChild(box_wood);
    sceneGraph->addChild(box_stone);
    sceneGraph->addChild(kutasiarz);
    sceneGraph->addChild(cos);
    sceneGraph->addChild(ludzik);
    sceneGraph->addChild(plane);

    ludzik->transform.setLocalPosition({ 3.f, 5.f, 3.f });
    //ludzik->transform.setLocalRotation({ -90.f, 0.f, 0.f });

    kutasiarz->transform.setLocalPosition({ 3.f, 2.f, 3.f });
    kutasiarz->transform.setLocalScale({ 0.3f, 0.3f, 0.3f });
    kutasiarz->transform.setLocalRotation({ 0.f, 45.f, 0.f });

    cos->transform.setLocalPosition({ -5.0, 0.5f, -5.0f });

    cos->transform.setLocalScale({ 1.0f, 1.0f, 1.0f });

    box_diff_spec->transform.setLocalPosition({ 7.5f, 0.0f, 0.0f });
    box_diff_spec->transform.setLocalScale({ 1.0f, 3.0f, 14.0f });

    box_diff_spec2->transform.setLocalPosition({ 0.0f, 0.0f, -7.5f });
    box_diff_spec2->transform.setLocalScale({ 16.0f, 3.0f, 1.0f });

    box_wood->transform.setLocalPosition({ -7.5f, 0.0f, 0.0f });
    box_wood->transform.setLocalScale({ 1.0f, 3.0f, 14.0f });

    box_stone->transform.setLocalPosition({ 0.0f, 0.0f, 7.5f });
    box_stone->transform.setLocalScale({ 16.0f, 3.0f, 1.0f });

    plane->transform.setLocalPosition({ 0.0f, -0.501f, 0.0f }); // z - fighting
    plane->transform.setLocalScale({ 15.f, 1.0f, 15.f });

    sceneGraph->addPointLight(new PointLight(Tmodel_light, "light", 0.032f, 0.09f));
    sceneGraph->addPointLight(new PointLight(Tmodel_light, "light", 0.032f, 0.09f));
    sceneGraph->addDirectionalLight(new DirectionalLight(Tmodel_light, "dir_light", glm::vec3(-1.f, -1.f, 0.f)));

	sceneGraph->root->getChildByName("light")->transform.setLocalPosition({ 0.f, 2.f, 0.f });
	sceneGraph->root->getChildByName("dir_light")->transform.setLocalPosition({ 4.f, 2.f, 4.f });

    /*walls->addChild(box_diff_spec);
    walls->addChild(box_diff_spec2);

    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 2; j++) {
            Node* temp = new Node(Tmodel_box_diff_spec, "inst" + to_string(i), colliders);
            temp->transform.setLocalPosition({ i + 4.f, 2.f, 4.f * j + 4.f });
            temp->transform.computeModelMatrix();
            walls->addChild(temp);
        }
    }*/

	//loadScene("res/scene/scene.json", rootNode, player, camera, point_lights, point_light_number, directional_lights, directional_light_number, models, colliders);

    const char* anim_path = "res/models/man/CesiumMan2.gltf";

    Animation* anim = new Animation(anim_path, models[2]);
	sceneGraph->root->getChildByName("ludzik")->animator = new Animator(anim);

	glm::vec3 origin = glm::vec3(0.f, 2.f, 0.f);

	camera->setObjectToFollow(sceneGraph->root->getChildByName("ludzik"), origin);

    sceneGraph->forcedUpdate();

    // --- GAME LOOP --- //

    while (!glfwWindowShouldClose(window->window)) {


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
        if (glfwGetKey(window->window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
            direction += 32;
        }

        if (glfwGetKey(window->window, GLFW_KEY_Q) == GLFW_PRESS) {
            direction += 64;
        }

        if (glfwGetKey(window->window, GLFW_KEY_E) == GLFW_PRESS) {
            direction += 128;
        }

		if (glfwGetKey(window->window, GLFW_KEY_T) == GLFW_PRESS) {
			currentOperation = ImGuizmo::TRANSLATE;
		}
		if (glfwGetKey(window->window, GLFW_KEY_R) == GLFW_PRESS) {
			currentOperation = ImGuizmo::ROTATE;
		}
		if (glfwGetKey(window->window, GLFW_KEY_Y) == GLFW_PRESS) {
			currentOperation = ImGuizmo::SCALE;
			uniformScale = false;
		}
        if (glfwGetKey(window->window, GLFW_KEY_U) == GLFW_PRESS) {
            currentOperation = ImGuizmo::SCALE;
			uniformScale = true;
        }

		sceneGraph->update(deltaTime);

        if (camera->mode != FREE) {
            player->update(deltaTime, direction, camera->Yaw);
        }

        for (auto&& collider : colliders) {
            
            if (player->player_node->AABB != collider && player->player_node->AABB->isBoundingBoxIntersects(*collider)) {


                player->player_node->separate(collider);
                
            }
        }

        camera->ProcessKeyboard(deltaTime, direction);
        changeMouse(window->window);
        

		
        if (glfwGetKey(window->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window->window, true);

        if (glfwGetMouseButton(window->window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
            mouse_pressed = true;
        } else {
            mouse_pressed = false;
        }

        background->update(deltaTime);
        background->render(*sceneGraph->shader_background);

        sceneGraph->mark(getRayWorld(window->window, camera->GetView(), camera->GetProjection()));


        if (mouse_pressed && glfwGetKey(window->window, GLFW_KEY_LEFT_CONTROL)) {
            if (sceneGraph->marked_object != nullptr) sceneGraph->marked_object->is_marked = false;

            sceneGraph->marked_object = sceneGraph->new_marked_object;

            if (sceneGraph->marked_object != nullptr) sceneGraph->marked_object->is_marked = true;
        }

        // == stencil buffer ==

        glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0x00);

        // == standard drawing ==

        sceneGraph->draw(previewX, previewY, previewWidth, previewHeight);

        // == outline ==

        sceneGraph->drawMarkedObject();
        float fps = 1.f / deltaTime;
        


        text->renderText("Fps: " + to_string(fps), 4.f * WINDOW_WIDTH / 5.f, WINDOW_HEIGHT - 100.f, *sceneGraph->shader_text, glm::vec3(1.f, 0.3f, 0.3f));
        text->renderText("We have text render!", 200, 200, *sceneGraph->shader_text, glm::vec3(0.6f, 0.6f, 0.98f));
        sprite->update(deltaTime);
        sprite3->update(deltaTime);
        
        sprite2->render(*sceneGraph->shader_background);
        sprite3->render(*sceneGraph->shader_background);
        sprite->render(*sceneGraph->shader_background);

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
        ImGuizmo::BeginFrame();

        // Window content
        ImGui::Begin("Test Window");
        ImGui::Text("Test test test test 123 123");
        ImGui::Text("Current FPS: %.1f", fps); // Dipslay current fps

        BeginFullscreenInputLayer();

        // Window render
        ImGui::End();


        
        DrawHierarchyWindow(sceneGraph->root);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // ---------------------
    
        window->updateWindow();

    }

	//saveScene("res/scene/scene.json", rootNode, player, point_lights, point_light_number, directional_lights, directional_light_number, models);

    // Audio engine cleanup
    audioEngine.Shutdown();

    // Nullptr to the window and input manager
    ServiceLocator::shutdownServices();

    //ImGui cleanup
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();

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

glm::vec4 getRayWorld(GLFWwindow* window, const glm::mat4& _view, const glm::mat4& _projection) {
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);


    bool isInPreview = (mouseX >= previewX && mouseX < previewX + previewWidth &&
        mouseY >= previewY && mouseY < previewY + previewHeight);

    if (isInPreview) {
        glm::vec2 normalizedMouse;
        normalizedMouse.x = (2.0f * (mouseX - previewX)) / previewWidth - 1.0f;
        normalizedMouse.y = 1.0f - (2.0f * (previewY - mouseY)) / previewHeight;

        glm::vec4 rayClip = glm::vec4(normalizedMouse.x, normalizedMouse.y, -1.0f, 1.0f);
        glm::vec4 rayEye = glm::inverse(_projection) * rayClip;
        rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
        glm::vec4 rayWorld = glm::normalize(glm::inverse(_view) * rayEye);

		return rayWorld;
	}
	else {
		return glm::vec4(0.f, 0.f, 0.f, 0.f);
    }
    
}

string print(glm::vec3 v) {
    return std::to_string(v.x) + " " + std::to_string(v.y) + " " + std::to_string(v.z);
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

void BeginFullscreenInputLayer()
{
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoBackground;

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    //ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

    ImGui::Begin("ImGuizmo Input Layer", nullptr, window_flags);
    // To okno jest niewidzialne, ale aktywne — i ImGuizmo może działać wewnątrz

    // IMGUIZMO
    Node* modified_object = sceneGraph->marked_object;
    if (modified_object != nullptr) {

        //io = ImGui::GetIO();

        ImGuizmo::SetOrthographic(false); // If we want to change between perspective and orthographic it's added just in case
        ImGuizmo::SetDrawlist(); // Draw to the current window

        //ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
        ImGuizmo::SetRect(WINDOW_WIDTH / 6, 0, 2 * WINDOW_WIDTH / 3, 2 * WINDOW_HEIGHT / 3);

        // Camera matrices for rendering ImGuizmo
        glm::mat4 _view = camera->GetView();
        glm::mat4 _projection = camera->GetProjection();

        // Getting marked object transform
        auto& _transform = modified_object->getTransform();
        glm::mat4 _model_matrix = _transform.getModelMatrix();

        
        ImGuizmo::Manipulate(glm::value_ptr(_view), glm::value_ptr(_projection),
            currentOperation, ImGuizmo::WORLD, glm::value_ptr(_model_matrix));

        if (modified_object->parent) {
			// w celu otrzymania loklalnych transformacji
            _model_matrix = glm::inverse(modified_object->parent->transform.getModelMatrix()) * _model_matrix;
        }
		

        glm::vec3 translation, scale, skew;
        glm::vec4 perspective;
		glm::quat rotation;
        //ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(_model_matrix), glm::value_ptr(translation), glm::value_ptr(rotation), glm::value_ptr(scale));
		glm::decompose(_model_matrix, scale, rotation, translation, skew, perspective);

        if (ImGuizmo::IsUsing()) {
            if (currentOperation == ImGuizmo::OPERATION::TRANSLATE) {
                modified_object->transform.setLocalPosition(translation);
			}
			else if (currentOperation == ImGuizmo::OPERATION::ROTATE) {
				modified_object->transform.setLocalRotation(rotation);
			}
            else if (currentOperation == ImGuizmo::OPERATION::SCALE) {
                if (uniformScale) {
                    if (scale.x != scale.y && scale.x != scale.z) {
                        scale.y = scale.x;
                        scale.z = scale.x;
                    }
                    else if (scale.y != scale.z && scale.y != scale.x) {
                        scale.x = scale.y;
                        scale.z = scale.y;
                    }
                    else {
                        scale.x = scale.z;
                        scale.y = scale.z;
                    }
                }
                modified_object->transform.setLocalScale(scale);
            }
            

        }

    }

    // Ale UWAGA: ImGuizmo::SetRect(...) MUSI być ustawione na pełny ekran!
    ImGuizmo::SetRect(viewport->Pos.x, viewport->Pos.y, viewport->Size.x, viewport->Size.y);

    // Rysuj GUI albo nic — okno istnieje tylko po to, żeby przekazywać inputy

    // Pamiętaj: nadal musi być End()!
    ImGui::End();

    ImGui::PopStyleVar(3);
}

void DrawSceneNode(Node* node, int depth) {
    ImGui::Indent(depth * 20.0f); // wcięcie zależne od poziomu

    ImVec2 item_start = ImGui::GetCursorScreenPos();
    ImGui::Text("%s", node->name.c_str());
    ImVec2 item_end = ImGui::GetCursorScreenPos();
    item_end.y += ImGui::GetTextLineHeight();

    // Klikalna powierzchnia
    ImRect rect(item_start, item_end);
    bool hovered = ImGui::IsMouseHoveringRect(rect.Min, rect.Max);
    bool clicked = hovered && ImGui::IsMouseClicked(0);

    if (clicked) {
        node->is_marked = !node->is_marked;
		if (node->is_marked) {
			sceneGraph->marked_object = node;
		}
		else {
			sceneGraph->marked_object = nullptr;
		}
    }

    if (node->is_marked) {
        ImU32 color = ImGui::GetColorU32(ImGuiCol_Header); // kolor zaznaczenia
        ImGui::GetWindowDrawList()->AddRectFilled(rect.Min, rect.Max, color);
        ImGui::SetCursorScreenPos(item_start);
        ImGui::Text("%s", node->name.c_str()); // ponowne rysowanie na zaznaczeniu
    }

    // Reset wcięcia
    ImGui::Unindent(depth * 20.0f);

    for (auto& child : node->children) {
        DrawSceneNode(child, depth + 1);
    }
}

void DrawNodeBlock(Node* node, int depth)
{
    if (!node) return;

    Node* selectedNode = node->scene_graph->marked_object;

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    float lineHeight = ImGui::GetTextLineHeightWithSpacing();
    float indentSize = 20.0f;

    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
	
    ImVec2 itemPos = ImVec2(cursorPos.x + depth * indentSize, cursorPos.y);
    ImVec2 itemSize = ImVec2(ImGui::GetContentRegionAvail().x - depth * indentSize, lineHeight);

    // Niewidzialny przycisk dla kliknięcia
    ImGui::SetCursorScreenPos(itemPos);
    ImGui::InvisibleButton(node->name.c_str(), itemSize);

    // Zaznaczenie
    if (selectedNode == node)
    {
        drawList->AddRect(itemPos,
            ImVec2(itemPos.x + itemSize.x, itemPos.y + itemSize.y),
            IM_COL32(255, 255, 0, 255), 4.0f, 0, 2.0f);
    }

    // Nazwa
    drawList->AddText(ImVec2(itemPos.x + 4, itemPos.y), IM_COL32_WHITE, node->name.c_str());

    if (ImGui::IsItemClicked())
    {

        node->scene_graph->marked_object = node;
    }

    // Ustaw pozycję kursora na następną linię
    ImGui::SetCursorScreenPos(ImVec2(cursorPos.x, cursorPos.y + lineHeight));

    // Rekurencja: dzieci
    for (auto& child : node->children)
    {
        DrawNodeBlock(child, depth + 1);
    }
}

void DrawHierarchyWindow(Node* root)
{
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoTitleBar;

    // Ustawiamy stałą pozycję i rozmiar okna
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(WINDOW_WIDTH / 6, WINDOW_HEIGHT / 2), ImGuiCond_Always);

    ImGui::Begin("Scene Graph", nullptr, window_flags);

    if (root)
        DrawNodeBlock(root, 0);

    ImGui::End();
}
