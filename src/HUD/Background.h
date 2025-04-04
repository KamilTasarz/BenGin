#pragma once

class Background {
public:
	float width, height;
	unsigned int texture_id;
	Background(float width, float height, const char* tile_map_path) : width(width), height(height) {

	}
};