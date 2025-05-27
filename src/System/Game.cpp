#include "Game.h"

#include "../System/ServiceLocator.h"
#include "../System/Serialization.h"
#include "../Basic/Node.h"
#include "../Component/BoundingBox.h"
#include "../Component/CameraGlobals.h"
#include "../ResourceManager.h"
#include "../System/PhysicsSystem.h"
#include "../Basic/Animator.h"
#include <random>

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

    glEnable(GL_DEPTH_TEST);
    sceneGraph->draw(viewWidth, viewHeight, framebuffer);

    // SSAO
    bool is_ssao = false;
    
    // SSAO pass
    if (is_ssao) {

        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
        glClear(GL_COLOR_BUFFER_BIT);

        ResourceManager::Instance().shader_PostProcess_ssao->use();

        // Uniformy:
        ResourceManager::Instance().shader_PostProcess_ssao->setInt("kernelSize", postProcessData.ssao_kernel_samples);
        for (unsigned int i = 0; i < postProcessData.ssao_kernel_samples; ++i) {
            ResourceManager::Instance().shader_PostProcess_ssao->setVec3("samples[" + std::to_string(i) + "]", ssao_kernel[i]);
        }

        ResourceManager::Instance().shader_PostProcess_ssao->setMat4("projection", camera->GetProjection());
        ResourceManager::Instance().shader_PostProcess_ssao->setMat4("invProjection", glm::inverse(camera->GetProjection()));
        ResourceManager::Instance().shader_PostProcess_ssao->setVec2("screenSize", glm::vec2(WINDOW_WIDTH, WINDOW_HEIGHT));
        ResourceManager::Instance().shader_PostProcess_ssao->setFloat("radius", 2.5f/*postProcessData.ssao_radius*/);


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
        ResourceManager::Instance().shader_PostProcess_ssao->setInt("gDepth", 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalTexture);
        ResourceManager::Instance().shader_PostProcess_ssao->setInt("gNormal", 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, noise_texture);
        ResourceManager::Instance().shader_PostProcess_ssao->setInt("texNoise", 2);

        // Narysuj fullscreen quad:
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // SSAO blur pass
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
        glClear(GL_COLOR_BUFFER_BIT);

        auto& blurShader = *ResourceManager::Instance().shader_PostProcess_ssao_blur;
        blurShader.use();
        blurShader.setInt("ssaoInput", 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer); // Input: raw SSAO

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }

    if (is_ssao) {
        glDisable(GL_DEPTH_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        auto& debug = *ResourceManager::Instance().shader_PostProcess_pass;
        debug.use();
        debug.setInt("ssaoMap", 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ssaoBlurColorBuffer);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glEnable(GL_DEPTH_TEST);
        return;  // skip the rest of post‐processing
    }

    //glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    //background->render(*ResourceManager::Instance().shader_background);
    //sprite2->render(*ResourceManager::Instance().shader_background);
    //sprite3->render(*ResourceManager::Instance().shader_background);
    //sprite->render(*ResourceManager::Instance().shader_background);
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);

    float time = glfwGetTime();
    float fpsValue = 1.f / ServiceLocator::getWindow()->deltaTime;

    //text->renderText("Fps: " + to_string(fpsValue), 4.f * WINDOW_WIDTH / 5.f, WINDOW_HEIGHT - 100.f, *ResourceManager::Instance().shader_text, glm::vec3(1.f, 0.3f, 0.3f));
    //text->renderText("We have text render!", 200, 200, *ResourceManager::Instance().shader_text, glm::vec3(0.6f, 0.6f, 0.98f));

    // Tu gdzieś ustawić uniformy dla shadera/shaderów SSAO i użyć

    glDisable(GL_DEPTH_TEST);

    //glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

    glBindVertexArray(quadVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    //glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);

    if(postProcessData.is_post_process) {

        if (postProcessData.is_bloom) {
            


        }

        if (postProcessData.is_crt_curved) {

            ResourceManager::Instance().shader_PostProcess_crt->use();
            ResourceManager::Instance().shader_PostProcess_crt->setInt("screenTexture", 0);

            ResourceManager::Instance().shader_PostProcess_crt->setVec2("curvature", postProcessData.crt_curvature);
            ResourceManager::Instance().shader_PostProcess_crt->setVec3("outline_color", postProcessData.crt_outline_color);

            ResourceManager::Instance().shader_PostProcess_crt->setVec2("screen_resolution", postProcessData.crt_screen_resolution);
            ResourceManager::Instance().shader_PostProcess_crt->setFloat("vignette_radius", postProcessData.crt_vignette_radius);

            ResourceManager::Instance().shader_PostProcess_crt->setVec2("lines_sinusoid_factor", postProcessData.crt_lines_sinusoid_factor);
            ResourceManager::Instance().shader_PostProcess_crt->setFloat("vignette_factor", postProcessData.crt_vignette_factor);
            
            ResourceManager::Instance().shader_PostProcess_crt->setVec3("brightness", postProcessData.crt_brightness);

            ResourceManager::Instance().shader_PostProcess_crt->setFloat("time", time);
        }

    }
    else {

        // glDisable(GL_DEPTH_TEST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        
        ResourceManager::Instance().shader_PostProcess_pass->use();
        //ResourceManager::Instance().shader_PostProcess_pass->setInt("screenTexture", 0);
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
        ResourceManager::Instance().shader_PostProcess_pass->setInt("ssaoMap", 0);
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthTexture);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        return;
    }

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

Game::Game(std::vector<std::shared_ptr<Prefab>>& prefabsref, std::vector<std::shared_ptr<Prefab>>& prefabsref_puzzle) : prefabs(prefabsref), puzzle_prefabs(prefabsref_puzzle)
{
	viewX = 0;
	viewY = 0;
	viewWidth = WINDOW_WIDTH;
	viewHeight = WINDOW_HEIGHT;
}

void Game::init()
{

	is_initialized = true;

	loadScene("res/scene/scene.json", sceneGraph, prefabs, puzzle_prefabs);

    loadPostProcessData("res/scene/postprocess_data.json", postProcessData);

    // SSAO
    ssao_kernel = updateSSAOKernel(postProcessData.ssao_kernel_samples);
    generateNoiseTexture();
    debug_print();

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

    // 2. Bufor normalnych
    glGenTextures(1, &normalTexture);
    glBindTexture(GL_TEXTURE_2D, normalTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, fbWidth, fbHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // 3. Bufor głębokości
    /*glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, fbWidth, fbHeight);*/

    // 3. Tekstura głębokości
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, fbWidth, fbHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //float border[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    //glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);

    // 4. Framebuffer
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);       // Color / albedo + lighting
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalTexture, 0);      // Normal
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer); // Depth
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);        // Depth

    GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer nie jest kompletny!" << std::endl;
    }
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 1. SSAO color buffer (jednokanałowy)
    glGenFramebuffers(1, &ssaoFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

    glGenTextures(1, &ssaoColorBuffer);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, fbWidth, fbHeight, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "SSAO FBO not complete!\n";

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // SSAO Blur
    glGenFramebuffers(1, &ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);

    glGenTextures(1, &ssaoBlurColorBuffer);
    glBindTexture(GL_TEXTURE_2D, ssaoBlurColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, fbWidth, fbHeight, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoBlurColorBuffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "SSAO Blur FBO not complete!\n";

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

        std::cout << "Fps: " << 1 / ServiceLocator::getWindow()->deltaTime << std::endl;
	}

    if (glfwWindowShouldClose(ServiceLocator::getWindow()->window)) engine_work = false;
}
void Game::shutdown()
{
    glDeleteTextures(1, &colorTexture);
    glDeleteTextures(1, &normalTexture);
    glDeleteTextures(1, &depthTexture);
    glDeleteRenderbuffers(1, &depthRenderbuffer);
    glDeleteFramebuffers(1, &framebuffer);
    //glfwTerminate();
}

std::vector<glm::vec3> Game::updateSSAOKernel(unsigned int samples)
{
    std::vector<glm::vec3> ssaoKernel;

    std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // distributor providing floats from [0, 1] range
    std::default_random_engine generator; // silnik pseudolosowy

    for (unsigned int i = 0; i < samples; ++i) {
    
        glm::vec3 sample(
            randomFloats(generator) * 2.0 - 1.0, // [-1, 1]
            randomFloats(generator) * 2.0 - 1.0, // [-1, 1]
            randomFloats(generator)              // tylko dodatnie Z
        );

        sample = glm::normalize(sample);
        sample *= randomFloats(generator);

        // Bias dla skupienia próbek przy środku
        float scale = float(i) / 64.0f;
        scale = glm::mix(0.1f, 1.0f, scale * scale);
        sample *= scale;

        ssaoKernel.push_back(sample);

    }

    return ssaoKernel;
}

void Game::generateNoiseTexture() {

    std::vector<glm::vec3> ssaoNoise;
    
    std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // distributor providing floats from [0, 1] range
    std::default_random_engine generator; // silnik pseudolosowy

    for (unsigned int i = 0; i < 16; i++) {
        glm::vec3 noise(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            0.0f // na płaszczyźnie XY
        );
        ssaoNoise.push_back(noise);
    }

    // Utwórz teksturę 4x4
    glGenTextures(1, &noise_texture);
    glBindTexture(GL_TEXTURE_2D, noise_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);

}

void Game::debug_print() {

    for (int i = 0; i < ssao_kernel.size(); ++i) {
        glm::vec3 v = ssao_kernel[i];
        std::cout << "Kernel[" << i << "]: ("
            << v.x << ", " << v.y << ", " << v.z << ")\n";
    }


}
