#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include <iostream>
#include <map>

#include "../Basic/Shader.h"

struct Character {
    unsigned int texture_id;
    glm::ivec2 size;
    glm::ivec2 offset_baseline; //from baseline
    unsigned int offset_chars;
};

enum Alignment {
	LEFT,
	CENTER,
	RIGHT
};

class Text {
public:

    std::map<char, Character> characters;
    FT_Library ft;
    FT_Face face;
    unsigned int VAO, VBO;
    glm::mat4 screen_projection;
    std::string name;

	Text(const char* font_path, int size = 48) {
        
        if (FT_Init_FreeType(&ft))
            std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        

        
        if (FT_New_Face(ft, font_path, 0, &face))
            std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

        name = std::string(font_path);

        size_t pos = name.find_last_of('/');
        if (pos != std::string::npos) {
            std::string filename = name.substr(pos + 1);
            name = std::to_string(size) + "_" + filename;
        }

        

        FT_Set_Pixel_Sizes(face, 0, size);
        loadCharacters();
        FT_Done_Face(face);
        FT_Done_FreeType(ft);

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        screen_projection = glm::ortho(0.0f, 1920.f, 0.0f, 1080.f); //dodać zmienne z #define
	}

    void loadCharacters() {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        for (unsigned char c = 0; c < 255; c++) {

            if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
                std::cerr << "Cannot load character: " << c << std::endl;
                continue;
            }

            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows,
                0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            Character character = {
                texture, glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                face->glyph->advance.x
            };
            characters.insert(std::pair<char, Character>(c, character));
        }
    }

    void renderText(std::string text_value, float x, float y, Shader& shader, glm::vec3 color, Alignment alignment = LEFT) {

        shader.use();
        shader.setVec3("text_color", color);
        shader.setMat4("projection", screen_projection);
        shader.setInt("letter", 0);

        glDepthFunc(GL_ALWAYS);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);

        unsigned int width = 0;

        for (auto c = text_value.begin(); c != text_value.end(); c++) {
            char value = *c;
            Character character = characters[value];
			width += (character.offset_chars >> 6); //offset_chars is in 64th of pixels
        }

		if (alignment == CENTER) {
			x -= width / 2.f;
		}
		else if (alignment == RIGHT) {
			x -= width;
		}

        for (auto c = text_value.begin(); c != text_value.end(); c++) {
            char value = *c;
            Character character = characters[value];

            float glypth_x = x + character.offset_baseline.x;
            float glypth_y = y + character.offset_baseline.y - character.size.y;

            float glypth_w = character.size.x;
            float glypth_h = character.size.y;

            float vertices[] = {
                glypth_x, glypth_y + glypth_h,            0.0f, 0.0f,
                glypth_x, glypth_y,                       0.0f, 1.0f,
                glypth_x + glypth_w, glypth_y,            1.0f, 1.0f,
                glypth_x, glypth_y + glypth_h,            0.0f, 0.0f,
                glypth_x + glypth_w, glypth_y,            1.0f, 1.0f,
                glypth_x + glypth_w, glypth_y + glypth_h, 1.0f, 0.0f,

            };
            glBindTexture(GL_TEXTURE_2D, character.texture_id);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            x += (character.offset_chars >> 6); 
        }
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);
        glDisable(GL_BLEND);
    }
};
