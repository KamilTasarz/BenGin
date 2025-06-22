#include "Game.h"

#include "../System/ServiceLocator.h"
#include "../System/Serialization.h"
#include "../Basic/Node.h"
#include "../Component/BoundingBox.h"
#include "../Component/CameraGlobals.h"
#include "../ResourceManager.h"
#include "../System/PhysicsSystem.h"
#include "../System/LineManager.h"
#include "../Basic/Animator.h"
#include "../AudioEngine.h"
#include <random>
#include "../System/GuiManager.h"
#include "../System/RenderSystem.h"
#include "../System/SceneManager.h"
#include "../Gameplay/GameManager.h"

Ray Game::getRayWorld(GLFWwindow* window, const glm::mat4& _view, const glm::mat4& _projection) {

    glm::vec4 rayClip = glm::vec4(normalizedMouse.x, normalizedMouse.y, 0.0f, 1.0f);
    glm::vec4 worldPos = glm::inverse(_projection * _view) * rayClip;
    worldPos /= worldPos.w;

    // 2. Kierunek: w ortho zawsze „w głąb kamery” — z= -1 w view space
    glm::vec4 direction = glm::normalize(glm::vec4(glm::inverse(_view) * glm::vec4(0, 0, -1, 0)));

    return { glm::vec3(worldPos), direction };
    
}

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

    if (glfwGetKey(window->window, GLFW_KEY_Z) == GLFW_PRESS) {
        if (!isClicked) {

            SceneManager::Instance().next();
			isClicked = true;
        }
    }
    else {
		isClicked = false;
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
	
    //float t = glfwGetTime();

    float time = glfwGetTime();
    float fpsValue = 1.f / ServiceLocator::getWindow()->deltaTime;

    // Render sceny do framebuffera

    glEnable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    // glClearColor(.01f, .01f, .01f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    float t = glfwGetTime();

    SceneManager::Instance().getCurrentScene()->draw(viewWidth, viewHeight, framebuffer); // Po tym etapie mamy gotowe color texture 
    
    float t2 = glfwGetTime();

    std::cout << "DRAW: " << t2 - t << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    LineManager::Instance().drawLines();
	
	

    GuiManager::Instance().draw();
	
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    GLuint current_texture = colorTexture;

    RenderSystem::Instance().clear();

    if (postProcessData.is_post_process) {
    
        // SSAO pass
        if (postProcessData.is_ssao) {

            glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
            glClear(GL_COLOR_BUFFER_BIT);

            ResourceManager::Instance().shader_PostProcess_ssao->use();

            // Uniformy:

            ResourceManager::Instance().shader_PostProcess_ssao->setMat4("projection", camera->GetProjection());
            ResourceManager::Instance().shader_PostProcess_ssao->setMat4("invProjection", glm::inverse(camera->GetProjection()));
            ResourceManager::Instance().shader_PostProcess_ssao->setVec2("screenSize", glm::vec2(WINDOW_WIDTH, WINDOW_HEIGHT));

            ResourceManager::Instance().shader_PostProcess_ssao->setInt("kernelSize", postProcessData.ssao_kernel_samples);
            for (unsigned int i = 0; i < postProcessData.ssao_kernel_samples; ++i) {
                ResourceManager::Instance().shader_PostProcess_ssao->setVec3("samples[" + std::to_string(i) + "]", ssao_kernel[i]);
            }

            ResourceManager::Instance().shader_PostProcess_ssao->setFloat("radius", postProcessData.ssao_radius);
            ResourceManager::Instance().shader_PostProcess_ssao->setFloat("bias", postProcessData.ssao_bias);
            ResourceManager::Instance().shader_PostProcess_ssao->setFloat("intensity", postProcessData.ssao_intensity);
            ResourceManager::Instance().shader_PostProcess_ssao->setVec2("noise_scale", postProcessData.ssao_noise_scale);


            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, depthTexture);
            ResourceManager::Instance().shader_PostProcess_ssao->setInt("gDepth", 0);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, normalTexture);
            ResourceManager::Instance().shader_PostProcess_ssao->setInt("gNormal", 1);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, noise_texture);
            ResourceManager::Instance().shader_PostProcess_ssao->setInt("texNoise", 2);

            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // SSAO blur pass
            glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
            glClear(GL_COLOR_BUFFER_BIT);

            auto& blurShader = *ResourceManager::Instance().shader_PostProcess_ssao_blur;
            blurShader.use();
            blurShader.setInt("ssaoInput", 0);
            blurShader.setVec2("screenSize", glm::vec2(WINDOW_WIDTH, WINDOW_HEIGHT));

            renderQuadWithTexture(ssaoColorBuffer);

            glDisable(GL_DEPTH_TEST);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

            auto& composite = *ResourceManager::Instance().shader_PostProcess_ssao_composite;

            composite.use();

            composite.setInt("sceneColor", 0);
            composite.setInt("ssaoMap", 1);
            composite.setFloat("aoPower", 2.f);

            renderQuadWithTextures(current_texture, ssaoBlurColorBuffer);
            current_texture = colorTexture;

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glEnable(GL_DEPTH_TEST);

        }

        if (postProcessData.is_bloom) {
            glBindFramebuffer(GL_FRAMEBUFFER, bloomFBO);
            glClear(GL_COLOR_BUFFER_BIT);

            auto& brightShader = *ResourceManager::Instance().shader_PostProcess_bloom;
            brightShader.use();
            brightShader.setInt("sceneTexture", 0);
            brightShader.setFloat("bloom_threshold", postProcessData.bloom_treshold);

            renderQuadWithTexture(current_texture);

            bool horizontal = true, firstIteration = true;
            int blurPassCount = postProcessData.bloom_blur_passes;

            auto& blurShader = *ResourceManager::Instance().shader_PostProcess_gaussian_blur;
            blurShader.use();
            blurShader.setVec2("screenSize", glm::vec2(WINDOW_WIDTH, WINDOW_HEIGHT));

            for (int i = 0; i < blurPassCount; ++i) {
                glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
                glClear(GL_COLOR_BUFFER_BIT);
                blurShader.setInt("horizontal", horizontal ? 1 : 0);

                glActiveTexture(GL_TEXTURE0);
                if (firstIteration)
                    glBindTexture(GL_TEXTURE_2D, brightTexture);
                else
                    glBindTexture(GL_TEXTURE_2D, pingpongTexture[!horizontal]);
                blurShader.setInt("image", 0);

                glBindVertexArray(quadVAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);

                horizontal = !horizontal;
                if (firstIteration) firstIteration = false;
            }  

            // Composite bloom onto scene
            glDisable(GL_DEPTH_TEST);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

            // glClear(GL_COLOR_BUFFER_BIT);

            auto& bloomComposite = *ResourceManager::Instance().shader_PostProcess_bloom_composite;
            bloomComposite.use();
            bloomComposite.setInt("sceneTexture", 0);
            bloomComposite.setInt("bloomTexture", 1);
            bloomComposite.setFloat("intensity", postProcessData.bloom_intensity);

            renderQuadWithTextures(colorTexture, pingpongTexture[!horizontal]);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glEnable(GL_DEPTH_TEST);

            current_texture = colorTexture;

        }

        if (postProcessData.is_rewind) {
        
            /* Tu dodamy obsluge uniformow i wgl */
            // Przy ustawieniu uniformu bool do shadera sprawdzaj to ze sceneGraph

            glBindFramebuffer(GL_FRAMEBUFFER, rewindFBO);
            glClear(GL_COLOR_BUFFER_BIT);

            auto& rewindShader = *ResourceManager::Instance().shader_PostProcess_rewind;
            rewindShader.use();

            rewindShader.setBool("is_rewind", SceneManager::Instance().getCurrentScene()->is_rewinidng);

            rewindShader.setFloat("time", glfwGetTime());

            rewindShader.setFloat("noise_alpha", postProcessData.rewind_noise_alpha);
            rewindShader.setFloat("band_speed", postProcessData.rewind_band_speed);
            rewindShader.setFloat("num_bands", float(postProcessData.rewind_band_amount));
            rewindShader.setFloat("band_thickness", postProcessData.rewind_band_thicc);

            rewindShader.setFloat("ripple_frequency", postProcessData.rewind_ripple_frequency);
            rewindShader.setFloat("ripple_amplitude", postProcessData.rewind_ripple_amplitude);
            rewindShader.setFloat("ripple_speed", postProcessData.rewind_ripple_speed);

            rewindShader.setInt("screenTexture", 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, current_texture);

            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            current_texture = rewindColorBuffer;

        }

        if (postProcessData.is_crt_curved) {

            glDisable(GL_DEPTH_TEST);

            if (alpha < 1.f) {
                glBindFramebuffer(GL_FRAMEBUFFER, crtFBO);
            }
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

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, current_texture);
            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);


            if (alpha < 1.f) {
                alpha += 0.01f;

                ResourceManager::Instance().shader_PostProcess_noise->use();
                ResourceManager::Instance().shader_PostProcess_noise->setInt("image", 0);
                ResourceManager::Instance().shader_PostProcess_noise->setFloat("time", glfwGetTime());
                ResourceManager::Instance().shader_PostProcess_noise->setFloat("alpha", alpha);
                ResourceManager::Instance().shader_PostProcess_noise->setVec2("resolution", glm::vec2(WINDOW_WIDTH, WINDOW_HEIGHT));

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, crtColorBuffer);
                glBindVertexArray(quadVAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
                glBindVertexArray(0);
                glEnable(GL_DEPTH_TEST);
            }

            float t2 = glfwGetTime();

			//std::cout << "draw time: " << t2 - t << std::endl;

            return;

        }

    }

    //glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    //background->render(*ResourceManager::Instance().shader_background);
    //sprite2->render(*ResourceManager::Instance().shader_background);
    //sprite3->render(*ResourceManager::Instance().shader_background);
    //sprite->render(*ResourceManager::Instance().shader_background);
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //text->renderText("Fps: " + to_string(fpsValue), 4.f * WINDOW_WIDTH / 5.f, WINDOW_HEIGHT - 100.f, *ResourceManager::Instance().shader_text, glm::vec3(1.f, 0.3f, 0.3f));
    //text->renderText("We have text render!", 200, 200, *ResourceManager::Instance().shader_text, glm::vec3(0.6f, 0.6f, 0.98f));




    glDisable(GL_DEPTH_TEST);

    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // glClear(GL_COLOR_BUFFER_BIT);
        
    ResourceManager::Instance().shader_PostProcess_pass->use();

    ResourceManager::Instance().shader_PostProcess_pass->setInt("screenTexture", 0);

    renderQuadWithTexture(current_texture);

    glEnable(GL_DEPTH_TEST);

    t2 = glfwGetTime();

    std::cout << "DRAW: " << t2 - t << std::endl;

	
}

void Game::update(float deltaTime)
{
    auto* window = ServiceLocator::getWindow();
    auto* audio = ServiceLocator::getAudioEngine();
    
    audio->Update();
    GameManager::instance().Update(deltaTime, SceneManager::Instance().getCurrentScene());

    // Kamera
    camera->ProcessKeyboard(deltaTime, 0);
    LineManager::Instance().clearLines();
    // Scena
    //sceneGraph->root->checkIfInFrustrum();
	float t = glfwGetTime();
	PhysicsSystem::instance().updateColliders(SceneManager::Instance().getCurrentScene());
    float t2 = glfwGetTime();
	PhysicsSystem::instance().updateCollisions();
	float t3 = glfwGetTime();
    SceneManager::Instance().getCurrentScene()->update(deltaTime);
    float t4 = glfwGetTime();
    SceneManager::Instance().getCurrentScene()->clearDeleteVector();

    GuiManager::Instance().update(ServiceLocator::getWindow()->deltaTime);

    // HUD
	cout << "FPS: " << 1.f / deltaTime << endl;
    cout << "updateColldiers" << t2 - t << endl;
	cout << "updateCollisions" << t3 - t2 << endl;
	cout << "updateSceneGraph" << t4 - t3 << endl;
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

    //GuiManager::Instance().init();
    
    
    //SceneManager::Instance().reset();
    
	//SceneManager::Instance().getCurrentScene();

	//loadScene("res/scene/scene.json", sceneGraph, prefabs, puzzle_prefabs);

    loadPostProcessData("res/scene/postprocess_data.json", postProcessData);

    // SSAO
    ssao_kernel = updateSSAOKernel(postProcessData.ssao_kernel_samples);
    generateNoiseTexture();
    debug_print();

    SceneManager::Instance().getCurrentScene()->forcedUpdate();

    SceneManager::Instance().getCurrentScene()->is_editing = false;

	camera->Pitch = 0.0f;
	camera->FarPlane = 33.1f;
	camera->Yaw = -90.0f;
	camera->setPosition(glm::vec3(0.f, 0.f, 0.f));
    glm::vec3 origin = glm::vec3(0.f);
	camera->setObjectToFollow(SceneManager::Instance().getCurrentScene()->root->getChildByName("camera_follow"), origin);
	camera->changeMode(FOLLOWING);
	
    alpha = 0.f;

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

    // ======== BLOOM SETUP ========

// 1) Framebuffer do ekstrakcji jasnych obszarów
    glGenFramebuffers(1, &bloomFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, bloomFBO);

    // Tekstura bright-pass (RGB16F dla szerokiego zakresu)
    glGenTextures(1, &brightTexture);
    glBindTexture(GL_TEXTURE_2D, brightTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, fbWidth, fbHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brightTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Bloom FBO not complete!" << std::endl;

    // 2) Ping-pong FBO do Gaussian blur
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongTexture);
    for (unsigned int i = 0; i < 2; ++i) {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongTexture[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, fbWidth, fbHeight, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongTexture[i], 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cerr << "Pingpong FBO " << i << " not complete!" << std::endl;
    }

    // Przywróć domyślny FBO
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ======== END BLOOM SETUP ========

    // ======== REWIND SETUP ========

    glGenFramebuffers(1, &rewindFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, rewindFBO);

    glGenTextures(1, &rewindColorBuffer);
    glBindTexture(GL_TEXTURE_2D, rewindColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fbWidth, fbHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rewindColorBuffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Rewind FBO not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ======== END REWIND SETUP ========


    // ======== NOISE START ========
    
    glGenFramebuffers(1, &crtFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, crtFBO);

    glGenTextures(1, &crtColorBuffer);
    glBindTexture(GL_TEXTURE_2D, crtColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fbWidth, fbHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, crtColorBuffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "CRT FBO not complete!" << std::endl;
    
    
    // ======== NOISE END ========

    ServiceLocator::getAudioEngine()->Init();
    loadSounds();

    SceneManager::Instance().getCurrentScene()->forcedUpdate();

    SceneManager::Instance().getCurrentScene()->root->createComponents();
	PhysicsSystem::instance().colliders.clear();
	PhysicsSystem::instance().colliders_RigidBody.clear();  
	PhysicsSystem::instance().rooms.clear();  
	
	//ResourceManager::Instance().shader_tile->use();
	//ResourceManager::Instance().shader_tile->setFloat("start_time", glfwGetTime());

    GameManager::instance().Init(SceneManager::Instance().getCurrentScene());

    if (GameManager::instance().emitter) glfwSetInputMode(ServiceLocator::getWindow()->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

}
void Game::run()
{
	if (!is_initialized)
		init();
    while (!glfwWindowShouldClose(ServiceLocator::getWindow()->window) && play) {

        if (SceneManager::Instance().isSwitched()) {
            
            shutdown();
            init();
            SceneManager::Instance().resetSwitched();
        }
        GLfloat currentFrame = glfwGetTime();
        ServiceLocator::getWindow()->deltaTime = currentFrame - ServiceLocator::getWindow()->lastFrame;
        ServiceLocator::getWindow()->lastFrame = currentFrame;

        if (ServiceLocator::getWindow()->deltaTime > 0.25f) {
            ServiceLocator::getWindow()->deltaTime = 0.0f;
        }

        float t3 = glfwGetTime();

        input();

        if (!SceneManager::Instance().isSwitched()) {

            float t4 = glfwGetTime();
            //cout << "Input time" << t4 - t3 << endl;

            update(ServiceLocator::getWindow()->deltaTime);
        }
        if (!SceneManager::Instance().isSwitched()) {
            draw();
            //float t5 = glfwGetTime();


            float t = glfwGetTime();

            ServiceLocator::getWindow()->updateWindow();

            float t2 = glfwGetTime();
        }
        //cout << "Draw time" << t2 - t << endl;

        //std::cout << "DeltaTime: " << ServiceLocator::getWindow()->deltaTime << std::endl;
	}

    if (glfwWindowShouldClose(ServiceLocator::getWindow()->window)) engine_work = false;
}
void Game::shutdown()
{
    GameManager::instance().onEnd();

    glfwSetInputMode(ServiceLocator::getWindow()->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    unloadSounds();
    glDeleteTextures(1, &colorTexture);
    glDeleteTextures(1, &normalTexture);
    glDeleteTextures(1, &depthTexture);
    glDeleteTextures(1, &noise_texture);
    glDeleteTextures(1, &pingpongTexture[0]);
    glDeleteTextures(1, &pingpongTexture[1]);
    glDeleteTextures(1, &rewindColorBuffer);
    glDeleteTextures(1, &ssaoColorBuffer);
    glDeleteTextures(1, &ssaoBlurColorBuffer);
    glDeleteTextures(1, &crtColorBuffer);
    glDeleteRenderbuffers(1, &depthRenderbuffer);
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteFramebuffers(1, &bloomFBO);
    glDeleteFramebuffers(1, &crtFBO);
    glDeleteFramebuffers(1, &pingpongFBO[0]);
    glDeleteFramebuffers(1, &pingpongFBO[1]);
    glDeleteFramebuffers(1, &rewindFBO);
    glDeleteFramebuffers(1, &ssaoFBO);
    glDeleteFramebuffers(1, &ssaoBlurFBO);
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

void Game::renderQuadWithTexture(GLuint tex) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Game::renderQuadWithTextures(GLuint tex0, GLuint tex1) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex1);
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Game::loadSounds() {
    
    auto* audio = ServiceLocator::getAudioEngine();

    audio->loadAllGameSounds();
    
}

void Game::unloadSounds() {

    auto* audio = ServiceLocator::getAudioEngine();

    audio->unloadAllGameSounds();

}

