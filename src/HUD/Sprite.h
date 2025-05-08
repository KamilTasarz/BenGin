#pragma once

#include "../Basic/Shader.h"
#include "../config.h"
#include "../Basic/Model.h"

struct UV {
	glm::vec2 left_up, right_down;
};

class Sprite {
public:
	unsigned int VAO, VBO, sprite_id;
	GLint sprite_w, sprite_h;
	glm::mat4 screen_projection;
	glm::vec2 pos;
	float scale;

	virtual ~Sprite() {}

	Sprite() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

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

	virtual void setPosition(float x, float y) {

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
	unsigned int sprite_num, sprite_id, atlas_w, atlas_h;
	UV* uvs;

	~AnimatedSprite() override {
		delete[] uvs;
	}

	//konstruktor wykorzystujacy sciezki do plikow wraz z ilosca plikow, wczyta je i umiesci w atlasie
	AnimatedSprite(float window_width, float window_height, float duration, const char** paths_to_sprites, int sprite_number, float x, float y, float scale = 1.f) : Sprite(), duration(duration) {
		screen_projection = glm::ortho(0.0f, window_width, 0.0f, window_height);
		this->scale = scale;
		uvs = new UV[sprite_number];

		GLint maxTextureSize;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

		glGenTextures(1, &sprite_id);

		int width, height, nrComponents;
		unsigned char* data = stbi_load(paths_to_sprites[0], &width, &height, &nrComponents, 0);

		if (width * sprite_number > maxTextureSize) {
			atlas_w = maxTextureSize;
			if ((width * sprite_number) % maxTextureSize == 0) {
				atlas_h = height * ((width * sprite_number) / maxTextureSize);
			}
			else {
				atlas_h = height * (((width * sprite_number) / maxTextureSize) + 1);
			}
		}
		else {
			atlas_w = width * sprite_number;
			atlas_h = height;
		}

		sprite_num = sprite_number;

		sprite_w = width;
		sprite_h = height;

		if (data)
		{

			glBindTexture(GL_TEXTURE_2D, sprite_id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, atlas_w, atlas_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
			UV uv;
			uv.left_up = glm::vec2(0.f, height / (float)atlas_h);
			uv.right_down = glm::vec2(width / (float)atlas_w, 0.f);
			uvs[0] = uv;

			int index = 0;
			stbi_image_free(data);
			int off_width = width, off_height = height;
			for (int j = 0; j < atlas_h && index < sprite_num; j += off_height) {
				for (int i = 0; i < atlas_w && index < sprite_num; i += off_width) {

					if (i == 0 && j == 0) continue;

					index = (j / sprite_h) * (atlas_w / sprite_w) + (i / sprite_w);


					data = stbi_load(paths_to_sprites[index], &width, &height, &nrComponents, 0);
					if (data) {
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, sprite_id);
						glTexSubImage2D(GL_TEXTURE_2D, 0, i, j, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
						UV uv;
						uv.left_up = glm::vec2(i / (float)atlas_w, (j + height) / (float)atlas_h);
						uv.right_down = glm::vec2((i + width) / (float)atlas_w, j / (float)atlas_h);
						uvs[index] = uv;
					}
					else {
						std::cout << "Texture failed to load at path: " << paths_to_sprites[index] << std::endl;
					}
					glBindTexture(GL_TEXTURE_2D, 0);
					
					stbi_image_free(data);

				}
			}
		}
		else
		{
			std::cout << "Texture failed to load at path: " << paths_to_sprites[0] << std::endl;
			stbi_image_free(data);
		}
		
		setPosition(x, y);
	}
	
	//konstruktor wykorzystujacy sciezke do sprite'u ze wszystkimi klatkami - wymagane ile rzedow, ile w rzedzie,
	// ile lacznie i offset po y do poczatku nastepnych
	AnimatedSprite(float window_width, float window_height, float duration, const char* path_to_atlas,
		int row_number, int sprites_per_row, int sprite_number, float x, float y, float scale = 1.f) : Sprite(), duration(duration) {
		
		screen_projection = glm::ortho(0.0f, window_width, 0.0f, window_height);
		this->scale = scale;

		sprite_id = textureFromFile(path_to_atlas);
		uvs = new UV[sprite_number];


		glBindTexture(GL_TEXTURE_2D, sprite_id); 
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &sprite_w);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &sprite_h);

		atlas_w = sprite_w;
		atlas_h = sprite_h;

		sprite_h = atlas_h / row_number;
		sprite_w = atlas_w / sprites_per_row;

		sprite_num = sprite_number;

		unsigned int index = 0;

		for (int j = atlas_h; j > 0 && index < sprite_num; j -= sprite_h) {
			for (int i = 0; i < sprite_w * sprites_per_row && index < sprite_num; i += sprite_w) {
					
				UV uv;
				uv.left_up = glm::vec2(i / (float)atlas_w, j / (float)atlas_h);
				uv.right_down = glm::vec2((i + sprite_w) / (float)atlas_w, (j - sprite_h) / (float)atlas_h);
				uvs[index] = uv;
				
				index++;
			}
		}
		glBindTexture(GL_TEXTURE_2D, 0);
		setPosition(x, y);
	}

	void update(float delta_time) override {
		current_time += delta_time;
		current_time = fmodf(current_time, duration);
		setPosition(pos.x, pos.y);
	}

	void setPosition(float x, float y) override {
		pos = glm::vec2(x, y);

		float segment_time = duration / sprite_num;
		int index = static_cast<int>(current_time / segment_time);

		

		UV uv = uvs[index];


		float vertices[] = {
			x,			  y + sprite_h * scale,			uv.left_up.x, uv.right_down.y,
			x,			  y,							uv.left_up.x, uv.left_up.y,
			x + sprite_w * scale, y,					uv.right_down.x, uv.left_up.y,
			x,		      y + sprite_h * scale,			uv.left_up.x, uv.right_down.y,
			x + sprite_w * scale, y,					uv.right_down.x, uv.left_up.y,
			x + sprite_w * scale, y + sprite_h * scale, uv.right_down.x, uv.right_down.y,

		};

		

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void render(Shader &shader) override {
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
};