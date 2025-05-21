#include "Game.h"

#include "../System/ServiceLocator.h"
#include "../System/Serialization.h"
#include "../Basic/Node.h"
#include "../Component/BoundingBox.h"
#include "../Component/CameraGlobals.h"
#include "../ResourceManager.h"
#include "../System/PhysicsSystem.h"

void Game::input()
{
    if (ServiceLocator::getInputManager()) {
        ServiceLocator::getInputManager()->processInput();
    }

    auto* window = ServiceLocator::getWindow();

    if (glfwGetKey(window->window, GLFW_KEY_TAB) == GLFW_PRESS) {
        //engine_work = false;
        //glfwSetWindowShouldClose(window->window, true);
        play = false;
    }

    if (glfwGetKey(window->window, GLFW_KEY_F11) == GLFW_PRESS) {
        window->toggleFullscreen();
        // Buffer button press
        while (glfwGetKey(window->window, GLFW_KEY_F11) == GLFW_PRESS)
            glfwPollEvents();
    }

}
void Game::draw()
{

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glClearColor(.01f, .01f, .01f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    sceneGraph->draw(viewWidth, viewHeight, framebuffer);

    
    //glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    //background->render(*ResourceManager::Instance().shader_background);
    //sprite2->render(*ResourceManager::Instance().shader_background);
    //sprite3->render(*ResourceManager::Instance().shader_background);
    //sprite->render(*ResourceManager::Instance().shader_background);
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);

    float fpsValue = 1.f / ServiceLocator::getWindow()->deltaTime;
    //text->renderText("Fps: " + to_string(fpsValue), 4.f * WINDOW_WIDTH / 5.f, WINDOW_HEIGHT - 100.f, *ResourceManager::Instance().shader_text, glm::vec3(1.f, 0.3f, 0.3f));
    //text->renderText("We have text render!", 200, 200, *ResourceManager::Instance().shader_text, glm::vec3(0.6f, 0.6f, 0.98f));
  
    // Tu gdzieś ustawić uniformy dla shadera/shaderów SSAO i użyć

    glDisable(GL_DEPTH_TEST);
	ResourceManager::Instance().shader_crt->use();
    glBindVertexArray(quadVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorTexture);

    ResourceManager::Instance().shader_crt->setInt("screenTexture", 0);
    ResourceManager::Instance().shader_crt->setVec2("curvature", glm::vec2(3.0f, 3.0f));
    float time = glfwGetTime();
    ResourceManager::Instance().shader_crt->setFloat("time", time);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);
}
void Game::update(float deltaTime)
{
    auto* window = ServiceLocator::getWindow();

    // Kamera
    camera->ProcessKeyboard(deltaTime, 0);

    // Scena
    //sceneGraph->root->checkIfInFrustrum();

	PhysicsSystem::instance().updateColliders(sceneGraph);
	PhysicsSystem::instance().updateCollisions();
    sceneGraph->update(deltaTime);
    sceneGraph->clearDeleteVector();
    // HUD
    
    //background->update(deltaTime);
    //sprite->update(deltaTime);
    //sprite3->update(deltaTime);
    
    
}

Game::Game(std::vector<std::shared_ptr<Prefab>>& prefabsref) : prefabs(prefabsref)
{
	viewX = 0;
	viewY = 0;
	viewWidth = WINDOW_WIDTH;
	viewHeight = WINDOW_HEIGHT;
}

void Game::init()
{
	is_initialized = true;

	loadScene("res/scene/scene.json", sceneGraph, prefabs);

	sceneGraph->forcedUpdate();

	sceneGraph->is_editing = false;

	camera->Pitch = 0.0f;
	camera->Yaw = -90.0f;
	camera->setPosition(glm::vec3(0.f, 0.f, 0.f));
    glm::vec3 origin = glm::vec3(0.f);
	camera->setObjectToFollow(sceneGraph->root->getChildByName("camera_follow"), origin);
	camera->changeMode(FOLLOWING);
	

    int fbWidth = viewWidth;
    int fbHeight = viewHeight;

    // 1. Tekstura kolorów
    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, fbWidth, fbHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // 2. Bufor głębokości
    glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, fbWidth, fbHeight);

    // 3. Framebuffer
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer nie jest kompletny!" << std::endl;
    }
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    sceneGraph->root->createComponents();

}
void Game::run()
{
	if (!is_initialized)
		init();
	while (!glfwWindowShouldClose(ServiceLocator::getWindow()->window) && play) {
        
        GLfloat currentFrame = glfwGetTime();
        ServiceLocator::getWindow()->deltaTime = currentFrame - ServiceLocator::getWindow()->lastFrame;
        ServiceLocator::getWindow()->lastFrame = currentFrame;

        if (ServiceLocator::getWindow()->deltaTime > 0.25f) {
            ServiceLocator::getWindow()->deltaTime = 0.0f;
        }

        input();
        update(ServiceLocator::getWindow()->deltaTime);
        draw();

        ServiceLocator::getWindow()->updateWindow();

	}

    if (glfwWindowShouldClose(ServiceLocator::getWindow()->window)) engine_work = false;
}
void Game::shutdown()
{
    glDeleteTextures(1, &colorTexture);
    glDeleteRenderbuffers(1, &depthRenderbuffer);
    glDeleteFramebuffers(1, &framebuffer);
    //glfwTerminate();
}