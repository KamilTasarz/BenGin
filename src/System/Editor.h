#pragma once

#ifndef EDITOR_H
#define EDITOR_H

#include "../config.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include "imgui/imgui_internal.h"

#include <imguizmo/ImGuizmo.h>

#include "PostProcessData.h"

class Sprite;
class SceneGraph;
class Prefab;
class Node;
class BoundingBox;
class Animator;
class Animation;
class LaserEmitterNode;

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

	//debug
	GLuint fbo, debugColorTex;
	unsigned int quadVAO, quadVBO;

	glm::vec2 normalizedMouse;
	Sprite * icon, * eye_icon, * eye_slashed_icon, *switch_on, *switch_off, * dir_light_icon, * point_light_icon, *vol_light_icon;
	SceneGraph* sceneGraph;
	SceneGraph* editor_sceneGraph;
	Node* emit;

	PostProcessData postProcessData;

	char tag_name[128], layer_name[128], new_prefab_name[128], new_prefab_name_puzz[128], UI_text[256];

	unsigned int framebuffer, colorTexture, depthRenderbuffer;

	bool isHUD = false, isInPreview = false, pressed_add = false, scene_editor = true, is_initialized = false, AABB_changing = false, show_gui = false;
	glm::vec3* local_point;

	bool isSnapped = false, is_static = true, lockPositionX = false, lockPositionY = false, lockPositionZ = false, puzz = true;
	glm::vec3 lastSnapOffset;
	glm::vec3 snapedPosition;
	glm::vec3 edit_camera_pos;
	glm::vec3 color = { 0.f, 0.f, 0.f };

	float mass = 1.f, gravity = 1.f, pos_x = 0.f, pos_y = 0.f, size = 1.f;

	
	int current_prefab = 0, current_puzzle = 0, current_opt1 = 0, current_opt2 = 0, direction = 0, prefab_inst_to_add = 0, 
		prefab_to_duplicate = 0, puzzle_inst_to_add = 0, puzzle_to_duplicate = 0, text_sprite = 0, text_id = 0, sprite_id = 0, order_id = 0;
	int current_component = 0, current_script = 0;

	//std::vector<BoundingBox*> colliders;
	std::vector<std::shared_ptr<Prefab>>& prefabs;
	std::vector<std::shared_ptr<Prefab>>& puzzle_prefabs;

	float fps = 0.0f;
	float fps_timer = 0.0f;
	int frames = 0;

	float angle = 0.f;

	Animator* animator;
	Animation* anim;
	
	void input();
	void draw();
	void update(float deltaTime);

public:

	bool play = false;
	Editor(std::vector<std::shared_ptr<Prefab>>& prefabsref, std::vector<std::shared_ptr<Prefab>>& prefabsref_puzzle);

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