#pragma once

#include "../Basic/Model.h"

class Sprite {
public:
	unsigned int VAO, VBO, sprite_id;
	GLint sprite_w, sprite_h;
	glm::mat4 screen_projection;
	glm::vec2 pos;
	float scale;

	Sprite() = default;

	Sprite(float width, float height, const char* sprite_path, float x, float y, float scale = 1.0f) : scale(scale) {
		screen_projection = glm::ortho(0.0f, width, 0.0f, height);
		sprite_id = textureFromFile(sprite_path);

		glBindTexture(GL_TEXTURE_2D, sprite_id);

		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &sprite_w);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &sprite_h);

		glBindTexture(GL_TEXTURE_2D, 0);

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		
		
		setPosition(x, y);
		
 	}

	void setPosition(float x, float y) {

		pos = glm::vec2(x, y);

		float vertices[] = {
			x,			  y + sprite_h * scale, 0.0f, 0.0f,
			x,			  y,            0.0f, 1.0f,
			x + sprite_w * scale, y,            1.0f, 1.0f,
			x,		      y + sprite_h * scale, 0.0f, 0.0f,
			x + sprite_w * scale, y,            1.0f, 1.0f,
			x + sprite_w * scale, y + sprite_h * scale, 1.0f, 0.0f,

		};

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}


	virtual void render(Shader& shader) {

		shader.use();
		shader.setMat4("projection", screen_projection);
		shader.setInt("background", 0);

		glBindVertexArray(VAO);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthFunc(GL_ALWAYS);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sprite_id);
		
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);
		glDisable(GL_BLEND);
	}
	virtual void update(float delta_time) {};
};

class AnimatedSprite : public Sprite {
public:
	float current_time = 0.0f, duration;
	unsigned int sprite_num, sprite_id;

	//konstruktor wykorzystujacy sciezki do plikow wraz z ilosca plikow, wczyta je i umiesci w atlasie
	AnimatedSprite(float duration, const char** paths_to_sprites, int sprite_number) : Sprite(), duration(duration) {

	}
	
	//konstruktor wykorzystujacy sciezke do atlasu - wymagane umieszczenie spritow w 1 lini
	AnimatedSprite(float duration, const char* path_to_atlas, int sprite_number) : Sprite(), duration(duration) {

	}

	void update(float delta_time) override {
		current_time += delta_time;
		current_time = fmodf(current_time, duration);
	}
};