#pragma once

#include "../config.h"
#include "../Basic/Shader.h"

enum GridType
{
    GRID_XY,
    GRID_XZ
};

class Grid
{

    
    public:

		

        Grid() {
			gridShader = new Shader("res/shaders/grid.vert", "res/shaders/grid.frag");
			Init();
        }
        void Init(); // tworzy VAO, VBO, ładuje shader
		void Update(); // aktualizuje VBO
        void Draw();
		GridType gridType = GRID_XZ;
        unsigned int vao = 0;
        unsigned int vbo = 0;
        Shader* gridShader;
        std::vector<glm::vec3> lineVertices;
        float gridSize = 1.0f;
        int halfCount = 100; // siatka -100..+100
};

