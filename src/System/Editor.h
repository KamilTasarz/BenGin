#pragma once

#ifndef EDITOR_H
#define EDITOR_H

#include "../config.h"



class Sprite;
class SceneGraph;
class Prefab;
class Node;
class BoundingBox;

struct Ray;

class Editor {
private:
	int previewX;
	int previewY;
	int previewWidth;
	int previewHeight;

	float xCursorMargin = 10.0f;
	float yCursorMargin = 10.0f;

	ImGuizmo::OPERATION currentOperation = ImGuizmo::OPERATION::TRANSLATE;
	bool uniformScale = false;

	glm::vec2 normalizedMouse;
	Sprite * icon, * eye_icon, * eye_slashed_icon, *switch_on, *switch_off, * dir_light_icon, * point_light_icon;
	SceneGraph* sceneGraph;
	SceneGraph* editor_sceneGraph;

	char tag_name[128], layer_name[128];

	unsigned int framebuffer, colorTexture, depthRenderbuffer;

	bool isHUD = false, isInPreview = false, pressed_add = false, scene_editor = true, is_initialized = false, AABB_changing = false;
	glm::vec3* local_point;

	bool isSnapped = false, is_static = true, lockPositionX = false, lockPositionY = false, lockPositionZ = false;
	glm::vec3 lastSnapOffset;
	glm::vec3 snapedPosition;

	float mass = 1.f, gravity = 1.f;
	
	int current_prefab = 0, current_opt1 = 0, current_opt2 = 0, direction = 0, prefab_inst_to_add = 0;
	int current_component = 0, current_script = 0;

	//std::vector<BoundingBox*> colliders;
	std::vector<std::shared_ptr<Prefab>>& prefabs;

	float fps = 0.0f;
	float fps_timer = 0.0f;
	int frames = 0;
	
	void input();
	void draw();
	void update(float deltaTime);

public:

	bool play = false;
	Editor(std::vector<std::shared_ptr<Prefab>>& prefabsref);

	void changeMouse(GLFWwindow* window);
	Ray getRayWorld(GLFWwindow* window, const glm::mat4& _view, const glm::mat4& _projection);
	void RenderGuizmo();
	void DrawNodeBlock(Node* node, int depth = 0);
	void DrawHierarchyWindow(Node* root, float x, float y, float width, float height);
	void previewDisplay();
	void assetBarDisplay(float x, float y, float width, float height);
	void operationBarDisplay(float x, float y, float width, float height);
	void propertiesWindowDisplay(SceneGraph* root, Node* preview_node, float x, float y, float width, float height);

	void init();
	void run();
	void shutdown();


};

#endif // !EDITOR_H