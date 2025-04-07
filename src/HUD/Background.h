#pragma once

#include "../Basic/Model.h"

class Background {
public:
	float width, height, speed;
    glm::vec2 pos;
	unsigned int texture_id, VBO, VAO;
    glm::mat4 screen_projection;

	Background(float width, float height, const char* texture_path, float speed = 4.f) : width(width), height(height), speed(speed) {
		texture_id = textureFromFile(texture_path);
		
        float vertices[] = {
            0.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
            
            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f,

        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        pos = glm::vec2(0.f, 0.f);
        screen_projection = glm::ortho(0.0f, width, 0.0f, height);
	}

    void update(float delta_time) {

        pos.x -= speed * delta_time;
        pos.x = fmodf(pos.x, width);
        float x = -pos.x / width;

        float vertices[] = {
            0.0f, 0.0f, x, 1.0f,
            width, 0.0f, x + 1.0f, 1.0f,
            0.0f, height, x, 0.0f,
            0.0f, height, x, 0.0f,
            width, 0.0f, x + 1.0f, 1.0f,
            width, height, x + 1.0f, 0.0f,

        };


        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void render(Shader &shader) {
        shader.use();
        shader.setMat4("projection", screen_projection);
        shader.setInt("background", 0);
        //glDepthFunc(GL_LESS);
        glDisable(GL_DEPTH_TEST);
        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);

        glBindTexture(GL_TEXTURE_2D, texture_id);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        glEnable(GL_DEPTH_TEST);
        
        //glDepthFunc(GL_LESS);
    }

};