#include "Editor.h"

#include <memory>

#include "../Basic/Shader.h"
#include "../Basic/Model.h"
#include "../Basic/Animator.h"

#include "../Text/Text.h"
#include "../HUD/Background.h"
#include "../HUD/Sprite.h"
#include "../AudioEngine.h"

#include "ServiceLocator.h"
#include "Window.h"

#include "../Input/InputManager.h"
#include "../Input/Input.h"

#include "Serialization.h"
#include "../ResourceManager.h"
#include "../Basic/Node.h"
#include "../Component/BoundingBox.h"
#include "../Component/CameraGlobals.h"
#include "../Component/Camera.h"
#include "../Grid.h"
#include "../Gameplay/ScriptFactory.h"
#include "../System/PhysicsSystem.h"
#include "../System/LineManager.h"
#include "../System/Rigidbody.h"
#include "../System/Tag.h"
#include "../System/GuiManager.h"

#include <glm/gtx/matrix_decompose.hpp>



Editor::Editor(std::vector<std::shared_ptr<Prefab>>& prefabsref, std::vector<std::shared_ptr<Prefab>>& prefabsref_puzzle) : prefabs(prefabsref), puzzle_prefabs(prefabsref_puzzle) {
    icon = new Sprite(1920.f, 1080.f, "res/sprites/icon.png", 0.f, 0.f, 1.f);
    eye_icon = new Sprite(1920.f, 1080.f, "res/sprites/eye.png", 0.f, 0.f, 1.f);
    eye_slashed_icon = new Sprite(1920.f, 1080.f, "res/sprites/eye_slashed.png", 0.f, 0.f, 1.f);
    dir_light_icon = new Sprite(1920.f, 1080.f, "res/sprites/dir_light_icon.png", 0.f, 0.f, 1.f);
    point_light_icon = new Sprite(1920.f, 1080.f, "res/sprites/point_light_icon.png", 0.f, 0.f, 1.f);
    switch_off = new Sprite(1920.f, 1080.f, "res/sprites/switch_off.png", 0.f, 0.f, 1.f);
    switch_on = new Sprite(1920.f, 1080.f, "res/sprites/switch_on.png", 0.f, 0.f, 1.f);

    previewX = WINDOW_WIDTH / 6;
    previewY = 0;
    previewWidth = 2 * WINDOW_WIDTH / 3;
    previewHeight = 2 * WINDOW_HEIGHT / 3;
}

void Editor::changeMouse(GLFWwindow* window) {
    if (camera->mode == FREE) {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);

        if (mouseX <= xCursorMargin) {
            glfwSetCursorPos(window, windowWidth - xCursorMargin - 1, mouseY);
            ServiceLocator::getWindow()->lastX = windowWidth - xCursorMargin - 1;
        }
        else if (mouseX >= windowWidth - xCursorMargin) {
            glfwSetCursorPos(window, xCursorMargin + 1, mouseY);
            ServiceLocator::getWindow()->lastX = xCursorMargin + 1;
        }

        if (mouseY <= yCursorMargin) {
            glfwSetCursorPos(window, mouseX, windowHeight - yCursorMargin - 1);
            ServiceLocator::getWindow()->lastY = windowHeight - yCursorMargin - 1;

        }
        else if (mouseY >= windowHeight - yCursorMargin) {
            glfwSetCursorPos(window, mouseX, yCursorMargin + 1);
            ServiceLocator::getWindow()->lastY = yCursorMargin + 1;
        }
    }

}

Ray Editor::getRayWorld(GLFWwindow* window, const glm::mat4& _view, const glm::mat4& _projection) {

    if (camera->mode != FRONT_ORTO) {

        glm::vec4 rayClip = glm::vec4(normalizedMouse.x, normalizedMouse.y, -1.0f, 1.0f);
        glm::vec4 rayEye = glm::inverse(_projection) * rayClip;
        rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
        glm::vec4 rayWorld = glm::normalize(glm::inverse(_view) * rayEye);

        return { camera->cameraPos, rayWorld };
    }
    else {
        glm::vec4 rayClip = glm::vec4(normalizedMouse.x, normalizedMouse.y, 0.0f, 1.0f);
        glm::vec4 worldPos = glm::inverse(_projection * _view) * rayClip;
        worldPos /= worldPos.w;

        // 2. Kierunek: w ortho zawsze „w głąb kamery” — z= -1 w view space
        glm::vec4 direction = glm::normalize(glm::vec4(glm::inverse(_view) * glm::vec4(0, 0, -1, 0)));

        return { glm::vec3(worldPos), direction };
    }
}

void Editor::DrawNodeBlock(Node* node, int depth)
{

    if (!node) return;

    float eye_offset = 40.f;

    Node* selectedNode = node->scene_graph->marked_object;

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    float lineHeight = ImGui::GetTextLineHeightWithSpacing();
    float indentSize = 20.0f;

    ImVec2 cursorPos = ImGui::GetCursorScreenPos();

    ImVec2 itemPos = ImVec2(cursorPos.x + depth * indentSize, cursorPos.y);
    ImVec2 itemSize = ImVec2(ImGui::GetContentRegionAvail().x - depth * indentSize - eye_offset, lineHeight);

    // Niewidzialny przycisk dla kliknięcia
    ImGui::SetCursorScreenPos(itemPos);
    ImGui::InvisibleButton(node->name.c_str(), itemSize);

    // Zaznaczenie
    if (selectedNode == node)
    {
        drawList->AddRect(itemPos,
            ImVec2(itemPos.x + itemSize.x, itemPos.y + itemSize.y),
            IM_COL32(255, 255, 0, 255), 4.0f, 0, 2.0f);


    }

    // Nazwa
    drawList->AddText(ImVec2(itemPos.x + 4, itemPos.y), IM_COL32_WHITE, node->name.c_str());

    if (ImGui::IsItemClicked())
    {

        node->scene_graph->marked_object = node;
    }

    ImVec2 iconPos = ImVec2(itemPos.x + itemSize.x, itemPos.y);
    ImGui::SetCursorScreenPos(iconPos);


    ImTextureID icon = node->is_visible ? eye_icon->sprite_id : eye_slashed_icon->sprite_id;
    ImTextureID switch_icon;
    if (dynamic_cast<PointLight*>(node) || dynamic_cast<DirectionalLight*>(node)) {
        Light* temp = dynamic_cast<Light*>(node);
        switch_icon = temp->is_shining ? switch_on->sprite_id : switch_off->sprite_id;
    }

    ImVec2 icon_size = ImVec2(lineHeight, lineHeight);
    std::string img_id = "##eye_" + node->name;
    std::string img2_id = "##switch_" + node->name;

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    //ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0)); 
    //ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));

    if (dynamic_cast<Light*>(node)) {
        Light* temp = dynamic_cast<Light*>(node);
        if (ImGui::ImageButton(img2_id.c_str(), switch_icon, icon_size)) {
            temp->is_shining = !temp->is_shining;
        }
    }

    ImGui::SameLine();

    if (ImGui::ImageButton(img_id.c_str(), icon, icon_size)) {
        node->is_visible = !node->is_visible;
    }

    ImGui::PopStyleColor();

    // Ustaw pozycję kursora na następną linię
    ImGui::SetCursorScreenPos(ImVec2(cursorPos.x, cursorPos.y + lineHeight + 10.f));


    // Rekurencja: dzieci
    for (auto& child : node->children)
    {
        DrawNodeBlock(child, depth + 1);
    }

    if (selectedNode == node) {
        if (ImGui::IsKeyPressed(ImGuiKey_Delete)) {

            //sceneGraph->deleteChild(node);

            sceneGraph->to_delete = selectedNode;

            sceneGraph->marked_object = nullptr;
        }

    }

    if (node == sceneGraph->root && sceneGraph->to_delete) {
        if (dynamic_cast<DirectionalLight*>(sceneGraph->to_delete)) {
            sceneGraph->deleteDirectionalLight(dynamic_cast<DirectionalLight*>(sceneGraph->to_delete));
            sceneGraph->to_delete = nullptr;
        }
        else if (dynamic_cast<PointLight*>(sceneGraph->to_delete)) {
            sceneGraph->deletePointLight(dynamic_cast<PointLight*>(sceneGraph->to_delete));
            sceneGraph->to_delete = nullptr;
        }
        else {
            sceneGraph->deleteChild(sceneGraph->to_delete);
            sceneGraph->to_delete = nullptr;
        }
    }
}

void Editor::DrawHierarchyWindow(Node* root, float x, float y, float width, float height)
{
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse;

    // Ustawiamy stałą pozycję i rozmiar okna
    ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);

    ImGui::Begin("Scene", nullptr, window_flags);

    if (root)
        DrawNodeBlock(root, 0);

    ImGui::End();
}

void Editor::previewDisplay()
{
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoBackground;


    ImGui::SetNextWindowPos(ImVec2(previewX, previewY), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(previewWidth, previewHeight), ImGuiCond_Always);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

    ImGui::Begin("Viewport", nullptr, window_flags);

    ImGui::Image((ImTextureID)(intptr_t)colorTexture, ImVec2(previewWidth, previewHeight), ImVec2(0, 1), ImVec2(1, 0));


    isInPreview = ImGui::IsItemHovered();
    if (isInPreview) {
        ImVec2 mousePos = ImGui::GetMousePos();
        normalizedMouse.x = (mousePos.x - previewX) / previewWidth;
        normalizedMouse.y = (mousePos.y - previewY) / previewHeight;
        normalizedMouse.y = 1.0f - normalizedMouse.y; // Odwrócenie Y
        normalizedMouse.x = normalizedMouse.x * 2.0f - 1.0f; // Normalizacja do zakresu [-1, 1]
        normalizedMouse.y = normalizedMouse.y * 2.0f - 1.0f; // Normalizacja do zakresu [-1, 1]
    }

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_DRAG")) {
            int id = *(int*)payload->Data;
            if (id == 30) {
                DirectionalLight* newNode = new DirectionalLight(ResourceManager::Instance().getModel(6), "dir_light", true);
                newNode->transform.setLocalPosition({ 0.f, 0.f, 0.f });
                sceneGraph->addDirectionalLight(newNode);
            }
            else if (id == 31) {
                PointLight* newNode = new PointLight(ResourceManager::Instance().getModel(6), "point_light", true, 0.032f, 0.09f);
                newNode->transform.setLocalPosition({ 0.f, 0.f, 0.f });
                sceneGraph->addPointLight(newNode);
            }
            else {
                //Model
                Node* p = new Node(ResourceManager::Instance().getModel(id), "entity");

                sceneGraph->addChild(p);
                sceneGraph->marked_object = p;
            }
        }
        ImGui::EndDragDropTarget();
    }



    RenderGuizmo();

    ImGui::End();

    ImGui::PopStyleVar(3);
}

void Editor::RenderGuizmo()
{
    //ImGuiViewport* viewport = ImGui::GetMainViewport();
    //ImGui::SetNextWindowPos(viewport->Pos);
    //ImGui::SetNextWindowSize(viewport->Size);
    ////ImGui::SetNextWindowViewport(viewport->ID);



    //ImGui::Begin("ImGuizmo Input Layer", nullptr, window_flags);
    // To okno jest niewidzialne, ale aktywne — i ImGuizmo może działać wewnątrz

    // IMGUIZMO
    Node* modified_object = sceneGraph->marked_object;

    if (!AABB_changing) {

        if (modified_object != nullptr) {

            //io = ImGui::GetIO();

            ImGuizmo::SetOrthographic(camera->mode == FRONT_ORTO); // If we want to change between perspective and orthographic it's added just in case
            ImGuizmo::SetDrawlist(); // Draw to the current window

            //ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
            ImGuizmo::SetRect(previewX, previewY, previewWidth, previewHeight);

            // Camera matrices for rendering ImGuizmo
            glm::mat4 _view = camera->GetView();
            glm::mat4 _projection = camera->GetProjection();

            // Getting marked object transform
            auto& _transform = modified_object->getTransform();
            glm::mat4 _model_matrix = _transform.getModelMatrix();

            glm::dvec3 lastPos = _transform.getLocalPosition();

            bool useSnap = ImGui::IsKeyDown(ImGuiKey_LeftShift);

            float snap[3] = { 0.5f, 0.5f, 0.5f };
            ImGuizmo::Manipulate(glm::value_ptr(_view), glm::value_ptr(_projection),
                currentOperation, ImGuizmo::LOCAL, glm::value_ptr(_model_matrix), nullptr, useSnap ? snap : nullptr);

            if (ImGuizmo::IsUsing()) {
                /*SnapResult result;
                for (auto& collider : colliders) {
                    if (modified_object->AABB != collider) {
                         SnapResult tempX, tempY, tempZ;
                         tempX = modified_object->AABB->trySnapToWallsX(*collider, 0.5f);
                         tempY = modified_object->AABB->trySnapToWallsY(*collider, 0.5f);
                         tempZ = modified_object->AABB->trySnapToWallsZ(*collider, 0.5f);
                         result.shouldSnap = tempX.shouldSnap || tempY.shouldSnap || tempZ.shouldSnap;

                         if (result.shouldSnap) {
                             result.snapOffset = tempX.snapOffset + tempY.snapOffset + tempZ.snapOffset;
                             break;
                         }
                    }
                }*/





                if (modified_object->parent) {
                    // w celu otrzymania loklalnych transformacji
                    _model_matrix = glm::inverse(modified_object->parent->transform.getModelMatrix()) * _model_matrix;
                }


                glm::vec3 translation, scale, skew;
                glm::vec4 perspective;
                glm::quat rotation;
                //ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(_model_matrix), glm::value_ptr(translation), glm::value_ptr(rotation), glm::value_ptr(scale));
                glm::decompose(_model_matrix, scale, rotation, translation, skew, perspective);




                /*if (result.shouldSnap && !isSnapped) {
                    translation += result.snapOffset;
                    lastSnapOffset = result.snapOffset;
                    snapedPosition = translation;
                    isSnapped = true;
                }
                else if (isSnapped) {
                    glm::vec3 currentOffset = translation - snapedPosition;
                    if (glm::length(currentOffset - lastSnapOffset) > 0.5f) {
                        isSnapped = false;
                    }
                }*/


                /*if (isGridSnap) {
                    glm::dvec3 delta = (glm::dvec3)translation - lastPos;
                    int axis = getDominantAxis(delta);

                    // 2. pobierz aktywną krawędź AABB
                    if (axis >= 0) {
                        float edge = sceneGraph->marked_object->AABB->min_point_world[axis];

                        // 3. snap
                        double snapValue = 1.0;
                        float snapped = round(edge / snapValue) * snapValue;
                        /*snapped.x = round(edge.x / snapValue) * snapValue;
                        snapped.y = round(edge.y / snapValue) * snapValue;
                        snapped.z = round(edge.z / snapValue) * snapValue;
                        float offset = snapped - edge;

                        if (std::abs(offset) < 0.2) {
                            glm::dvec3 snapVec(0.0);
                            snapVec[axis] = offset;
                            translation += snapVec;
                        }
                    }
                }*/





                if (currentOperation == ImGuizmo::OPERATION::TRANSLATE) {
                    modified_object->transform.setLocalPosition(translation);
                }
                else if (currentOperation == ImGuizmo::OPERATION::ROTATE) {
                    modified_object->transform.setLocalRotation(rotation);
                }
                else if (currentOperation == ImGuizmo::OPERATION::SCALE) {
                    if (uniformScale) {
                        if (scale.x != scale.y && scale.x != scale.z) {
                            scale.y = scale.x;
                            scale.z = scale.x;
                        }
                        else if (scale.y != scale.z && scale.y != scale.x) {
                            scale.x = scale.y;
                            scale.z = scale.y;
                        }
                        else {
                            scale.x = scale.z;
                            scale.y = scale.z;
                        }
                    }
                    modified_object->transform.setLocalScale(scale);
                }
            }
            modified_object->updateSelfAndChild(true);

        }
    }
    else {
		


        if (local_point && modified_object) {
            glm::mat4 modelMatrix = modified_object->transform.getModelMatrix();
            glm::vec3 world_pos = glm::vec3(modelMatrix * glm::vec4(*local_point, 1.0f));

            glm::mat4 gizmo_matrix = glm::translate(glm::mat4(1.0f), world_pos);
            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(previewX, previewY, previewWidth, previewHeight);

            // Camera matrices for rendering ImGuizmo
            glm::mat4 _view = camera->GetView();
            glm::mat4 _projection = camera->GetProjection();


            bool useSnap = ImGui::IsKeyDown(ImGuiKey_LeftShift);

            float snap[3] = { 0.5f, 0.5f, 0.5f };
            bool changed = ImGuizmo::Manipulate(glm::value_ptr(_view), glm::value_ptr(_projection),
                ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::LOCAL, glm::value_ptr(gizmo_matrix), nullptr, useSnap ? snap : nullptr);


            // Jeśli ruszono:
            if (changed) {
                glm::vec3 translation, rotation, scale;
                ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(gizmo_matrix),
                    glm::value_ptr(translation),
                    glm::value_ptr(rotation),
                    glm::value_ptr(scale));

                // Odwrócenie modelMatrix żeby wrócić do przestrzeni lokalnej:
                glm::mat4 inverseModel = glm::inverse(modelMatrix);
                *local_point = glm::vec3(inverseModel * glm::vec4(translation, 1.0f));



            }
        }
    }


}

void Editor::assetBarDisplay(float x, float y, float width, float height) {

    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoSavedSettings;


    ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);

    ImGui::Begin("Assets", nullptr, window_flags);

    float padding = 16.0f;
    float thumbnailSize = 64.0f;
    float cellSize = thumbnailSize + padding;

    float panelWidth = ImGui::GetContentRegionAvail().x;
    int columnCount = (int)(panelWidth / cellSize);
    if (columnCount < 1) columnCount = 1;




    ImGui::Columns(columnCount, nullptr, false);

    int size = ResourceManager::Instance().getModels().size();

    for (auto& [key, model] : ResourceManager::Instance().getModels()) {

        ImGui::PushID(model->id);

        ImTextureID _icon = icon->sprite_id;
        string name = model->directory;
        if (!model->mode.empty() && model->textures_loaded.size() > 0) {

            _icon = model->textures_loaded[0].id;
            name = model->mode + " " + model->textures_loaded[0].path;
        }

        // Ikona (np. asset.textureID albo fallback ikona)
        if (ImGui::ImageButton("cos", (ImTextureID)(intptr_t)_icon, ImVec2(thumbnailSize, thumbnailSize))) {
            // (opcjonalne: obsługa kliknięcia na asset)
        }

        // DRAG SOURCE
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            ImGui::SetDragDropPayload("ASSET_DRAG", &model->id, sizeof(model->id));
            ImGui::Text("Dragging %s", model->directory.c_str());
            ImGui::EndDragDropSource();
        }
        // Nazwa assetu


        ImGui::TextWrapped(name.c_str());

        ImGui::NextColumn();
        ImGui::PopID();
    }

    // Directional light

    {

        std::shared_ptr<ViewLight> temp = ResourceManager::Instance().getLight(30);

        ImGui::PushID(temp->id);

        ImTextureID _icon = dir_light_icon->sprite_id;
        string name = temp->type;

        if (ImGui::ImageButton("cos", (ImTextureID)(intptr_t)_icon, ImVec2(thumbnailSize, thumbnailSize))) {
            // (opcjonalne: obsługa kliknięcia na asset)
        }

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            ImGui::SetDragDropPayload("ASSET_DRAG", &temp->id, sizeof(temp->id));
            ImGui::Text("Dragging %s", name.c_str());
            ImGui::EndDragDropSource();
        }

        ImGui::TextWrapped(name.c_str());

        ImGui::NextColumn();
        ImGui::PopID();

    }

    // Point light

    {

        std::shared_ptr<ViewLight> temp = ResourceManager::Instance().getLight(31);

        ImGui::PushID(temp->id);

        ImTextureID _icon = point_light_icon->sprite_id;
        string name = temp->type;

        if (ImGui::ImageButton("cos", (ImTextureID)(intptr_t)_icon, ImVec2(thumbnailSize, thumbnailSize))) {
            // (opcjonalne: obsługa kliknięcia na asset)
        }

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            ImGui::SetDragDropPayload("ASSET_DRAG", &temp->id, sizeof(temp->id));
            ImGui::Text("Dragging %s", name.c_str());
            ImGui::EndDragDropSource();
        }

        ImGui::TextWrapped(name.c_str());

        ImGui::NextColumn();
        ImGui::PopID();

    }

    ImGui::Columns(1);
    ImGui::End();

}

void Editor::operationBarDisplay(float x, float y, float width, float height)
{
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoSavedSettings;


    ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Once);

    ImGui::Begin("Operations", nullptr, window_flags);
    ImGui::PushItemWidth(150);

    if (ImGui::Button("ORTO", ImVec2(150, 24))) {
        camera->mode != FRONT_ORTO ? camera->changeMode(FRONT_ORTO) : camera->changeMode(FREE);
        sceneGraph->grid->gridType = camera->mode == FRONT_ORTO ? GRID_XY : GRID_XZ;
        sceneGraph->grid->Update();
    }
    ImGui::SameLine();

    if (ImGui::Button("SAVE", ImVec2(150, 24))) {
        
        saveScene("res/scene/scene.json", editor_sceneGraph);
        if (!scene_editor) {
            if (puzz) {
                savePuzzle(puzzle_prefabs[current_puzzle]);
            }
            else {
                savePrefab(prefabs[current_prefab]);
            }
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("EDITING_VIEW", ImVec2(150, 24))) {
        sceneGraph->is_editing = !sceneGraph->is_editing;
    }
    ImGui::Separator();
    if (ImGui::Button("PLAY", ImVec2(150, 24))) {
        play = true;
        //glfwSetWindowShouldClose(ServiceLocator::getWindow()->window, true);
    }
    ImGui::SameLine();
    if (ImGui::Button("FULL-SCREEN", ImVec2(150, 24))) {
        auto* window = ServiceLocator::getWindow();
        window->toggleFullscreen();
    }
    ImGui::Separator();
    if (ImGui::Button("ADD_EMPTY_NODE", ImVec2(150, 24))) {
        Node* new_node = new Node("empty_node");
		sceneGraph->addChild(new_node);
        sceneGraph->marked_object = new_node;
    }
    if (ImGui::Button("ADD_GAS_EMITTER", ImVec2(150, 24))) {
        Node* new_node = new InstanceManager(ResourceManager::Instance().getModel(7), "Gas_Emitter");
        sceneGraph->addChild(new_node);
        sceneGraph->marked_object = new_node;
    }

    if (ImGui::Button("ADD_MIRROR", ImVec2(150, 24))) {
        Node* new_node = new MirrorNode(ResourceManager::Instance().getModel(21), "MIRORR");
        sceneGraph->addChild(new_node);
        sceneGraph->marked_object = new_node;
    }

    if (sceneGraph->marked_object) {
        std::vector<const char*> items;

		items.push_back("Script");
		items.push_back("Rigidbody");

        if (ImGui::Combo("Choose Component", &current_component, items.data(), items.size())) {

        }
        std::vector<std::string> scripts = ScriptFactory::instance().getScriptNames();
        std::vector<const char*> scripts_names;
        for (auto& s : scripts) {
            scripts_names.push_back(s.c_str());
        }



        if (current_component == 0) {
            

            if (ImGui::Combo("Choose Script", &current_script, scripts_names.data(), scripts_names.size())) {

            }
		}
		else if (current_component == 1) {
			ImGui::InputFloat("Mass", &mass);
			ImGui::InputFloat("Gravity", &gravity);
			ImGui::Checkbox("Static", &is_static);
			ImGui::Checkbox("Lock Position X", &lockPositionX);
			ImGui::Checkbox("Lock Position Y", &lockPositionY);
			ImGui::Checkbox("Lock Position Z", &lockPositionZ);
		}

		if (ImGui::Button("ADD_COMPONENT", ImVec2(150, 24))) {

			bool canAdd = true;
            for (auto& comp : sceneGraph->marked_object->components) {
                
                if (comp->name._Equal(items[current_component]) || (current_component == 0 && comp->name._Equal(scripts_names[current_script]))) {
					canAdd = false;
                }
            }
            if (canAdd) {
                if (current_component == 0) {
                    sceneGraph->marked_object->addComponent(ScriptFactory::instance().create(scripts[current_script]));
				}
                else if (current_component == 1) {
                    sceneGraph->marked_object->addComponent(std::make_unique<Rigidbody>(mass, gravity, is_static, true, lockPositionX, lockPositionY, lockPositionZ));
                }
            }
			
		}
    }


    std::vector<const char*> items;


    int size = prefabs.size();

    for (const auto& prefab : prefabs) {
        items.push_back(prefab->prefab_scene_graph->root->name.c_str());
    }
    if (size == 0) {
        items.push_back("<none>");
    }

    std::vector<const char*> puzzles;


    int size_puzzle = puzzle_prefabs.size();

    for (const auto& prefab : puzzle_prefabs) {
        puzzles.push_back(prefab->prefab_scene_graph->root->name.c_str());
    }
    if (size == 0) {
        puzzles.push_back("<none>");
    }


    if (scene_editor) {

        ImGui::Separator();
        ImGui::Text("ROOMS");
        ImGui::PushID("##rooms");
        // wyświetl combo box tylko jeśli jest coś do wyboru
        if (ImGui::Combo("Choose prefab", &current_prefab, items.data(), items.size())) {
            if (current_prefab == size) {
                std::cout << "Wybrano: brak\n";
            }
            else {
                std::cout << "Wybrano: " << items[current_prefab] << "\n";
            }
        }

        if (ImGui::Button("ADD_PREFAB_INSTANCE", ImVec2(150, 24))) {
            if (size > 0) {
                Node* inst = new PrefabInstance(prefabs[current_prefab], sceneGraph, "_"+to_string(prefabs[current_prefab]->prefab_instances.size()));
                sceneGraph->addChild(inst);
                sceneGraph->marked_object = inst;
            }

        }

        ImGui::Separator();

        const char* opt1[] = { "HORIZONTAL", "VERTICAL" };

        if (ImGui::Combo("Choose direction", &current_opt1, opt1, 2)) {

        }
        

        const char* opt2[2];

        if (current_opt1 == 0) {
            opt2[0] = "UP";
            opt2[1] = "DOWN";
        }
        else {
            opt2[0] = "LEFT";
            opt2[1] = "RIGHT";
        }

        if (ImGui::Combo("Choose orientation", &current_opt2, opt2, 2)) {

        }


        if (ImGui::Button("CREATE_PREFAB", ImVec2(150, 24))) {

            PrefabType type;
            if (current_opt1 == 0) {
                if (current_opt2 == 0) {
                    type = HORIZONTAL_LEFT;
                }
                else {
                    type = HORIZONTAL_RIGHT;
                }

            }
            else {
                if (current_opt2 == 0) {
                    type = VERTICAL_UP;
                }
                else {
                    type = VERTICAL_DOWN;
                }
            }
            std::string name = "Prefab_new";
            bool unique = false;
            while (!unique) {
                unique = true;
                for (auto& prefab : prefabs) {
                    if (prefab->prefab_scene_graph->root->name._Equal(name)) {
                        name += "new";
                        unique = false;
                        break;
                    }
                }
            }
            shared_ptr<Prefab> new_prefab = make_shared<Prefab>(name, type);
            prefabs.push_back(new_prefab);

        }

        if (ImGui::Button("EDIT_PREFAB", ImVec2(150, 24))) {
            scene_editor = !scene_editor;
            puzz = false;
            edit_camera_pos = camera->cameraPos;
            camera->setPosition(glm::vec3(0.f, 0.f, 20.f));
            
        }

        ImGui::Separator();
        if (ImGui::Button("DUPLICATE_PREFAB", ImVec2(150, 24))) {

            std::string name = new_prefab_name;
            if (name == "") name = "New_prefab";

            shared_ptr<Prefab> new_prefab = make_shared<Prefab>(name);

            new_prefab->prefab_scene_graph->root = prefabs[prefab_to_duplicate]->clone("", new_prefab->prefab_scene_graph, true);
            new_prefab->prefab_scene_graph->root->name = name;
            prefabs.push_back(new_prefab);
            current_prefab = prefabs.size() - 1;
            scene_editor = false;
            puzz = false;
            new_prefab->prefab_type = prefabs[prefab_to_duplicate]->prefab_type;
            

            new_prefab_name[0] = '\0';
        }

        if (ImGui::Combo("Choose prefab to duplicate", &prefab_to_duplicate, items.data(), items.size())) {
            if (current_prefab == size) {
                std::cout << "Wybrano: brak\n";
            }
            else {
                std::cout << "Wybrano: " << items[prefab_to_duplicate] << "\n";
            }
        }

        ImGui::InputText("Duplicate name:", new_prefab_name, 128);
        ImGui::PopID();
        ImGui::Separator();
        
        ImGui::Text("PUZZLES");
        ImGui::PushID("##puzzles");

        if (ImGui::Combo("Choose puzzle", &current_puzzle, puzzles.data(), puzzles.size())) {
            if (current_prefab == size_puzzle) {
                std::cout << "Wybrano: brak\n";
            }
            else {
                std::cout << "Wybrano: " << puzzles[current_puzzle] << "\n";
            }
        }

        if (ImGui::Button("ADD_PUZZLE_INSTANCE", ImVec2(150, 24))) {
            if (size_puzzle > 0) {
                Node* inst = new PrefabInstance(puzzle_prefabs[current_puzzle], sceneGraph, "_" + to_string(puzzle_prefabs[current_puzzle]->prefab_instances.size()));
                sceneGraph->addChild(inst);
                sceneGraph->marked_object = inst;
            }

        }

        ImGui::Separator();


        if (ImGui::Button("CREATE_PUZZLE", ImVec2(150, 24))) {

            std::string name = "Puzzle_new";
            bool unique = false;
            while (!unique) {
                unique = true;
                for (auto& prefab : puzzle_prefabs) {
                    if (prefab->prefab_scene_graph->root->name._Equal(name)) {
                        name += "new";
                        unique = false;
                        break;
                    }
                }
            }
            shared_ptr<Prefab> new_prefab = make_shared<Prefab>(name);
            puzzle_prefabs.push_back(new_prefab);

        }

        if (ImGui::Button("EDIT_PUZZLE", ImVec2(150, 24))) {
            scene_editor = !scene_editor;
            puzz = true;
            edit_camera_pos = camera->cameraPos;
            camera->setPosition(glm::vec3(0.f, 0.f, 20.f));
        }

        ImGui::Separator();
        if (ImGui::Button("DUPLICATE_PUZZLE", ImVec2(150, 24))) {

            std::string name = new_prefab_name_puzz;
            if (name == "") name = "New_puzzle";

            shared_ptr<Prefab> new_prefab = make_shared<Prefab>(name);

            new_prefab->prefab_scene_graph->root = puzzle_prefabs[puzzle_to_duplicate]->clone("", new_prefab->prefab_scene_graph, true);
            new_prefab->prefab_scene_graph->root->name = name;
            puzzle_prefabs.push_back(new_prefab);
            current_puzzle = puzzle_prefabs.size() - 1;
            scene_editor = false;
            puzz = true;
            new_prefab->prefab_type = puzzle_prefabs[puzzle_to_duplicate]->prefab_type;


            new_prefab_name_puzz[0] = '\0';
        }

        if (ImGui::Combo("Choose prefab to duplicate", &puzzle_to_duplicate, puzzles.data(), puzzles.size())) {
            if (current_prefab == size_puzzle) {
                std::cout << "Wybrano: brak\n";
            }
            else {
                std::cout << "Wybrano: " << puzzles[puzzle_to_duplicate] << "\n";
            }
        }

        ImGui::InputText("Duplicate name:", new_prefab_name_puzz, 128);
        ImGui::PopID();
        ImGui::Separator();

    }
    else {


        // wyświetl combo box tylko jeśli jest coś do wyboru
        if (ImGui::Combo("Choose prefab", &prefab_inst_to_add, items.data(), items.size())) {
            if (current_prefab == size) {
                std::cout << "Wybrano: brak\n";
            }
            else {
                std::cout << "Wybrano: " << items[prefab_inst_to_add] << "\n";
            }
        }

        if (ImGui::Button("ADD_ROOM_COPY", ImVec2(150, 24))) {
            if (size > 0) {
                std::string name = prefabs[prefab_inst_to_add]->prefab_scene_graph->root->name + "_new";
                Node* inst = prefabs[prefab_inst_to_add]->clone(name, sceneGraph, true);
                sceneGraph->addChild(inst);
                sceneGraph->marked_object = inst;
            }

        }

        if (ImGui::Combo("Choose puzzle", &puzzle_inst_to_add, puzzles.data(), puzzles.size())) {
            if (current_prefab == size_puzzle) {
                std::cout << "Wybrano: brak\n";
            }
            else {
                std::cout << "Wybrano: " << puzzles[prefab_inst_to_add] << "\n";
            }
        }

        if (ImGui::Button("ADD_PUZZLE_COPY", ImVec2(150, 24))) {
            if (size_puzzle > 0) {
                std::string name = puzzle_prefabs[puzzle_inst_to_add]->prefab_scene_graph->root->name + "_new";
                Node* inst = puzzle_prefabs[puzzle_inst_to_add]->clone(name, sceneGraph, true);
                sceneGraph->addChild(inst);
                sceneGraph->marked_object = inst;
            }

        }

        if (ImGui::Button("RETURN", ImVec2(150, 24))) {
            if (!puzz) {
                Prefab prefab = *prefabs[current_prefab];
                prefab.notifyInstances();
            }
            else {
                Prefab prefab = *puzzle_prefabs[current_puzzle];
                prefab.notifyInstances();
            }

            scene_editor = !scene_editor;
            camera->setPosition(edit_camera_pos);
        }
    }


    ImGui::Separator();

	ImGui::InputText("Tag name: ", tag_name, 128);

    if (ImGui::Button("Add tag", ImVec2(150, 24))) {
        if (!TagLayerManager::Instance().getTag(tag_name)) {
			TagLayerManager::Instance().addTag(tag_name);
        }
    }

    if (ImGui::Button("Delete tag", ImVec2(150, 24))) {
        if (TagLayerManager::Instance().getTag(tag_name)) {
            TagLayerManager::Instance().removeTag(tag_name);
        }
    }

    ImGui::InputText("Layer name: ", layer_name, 128);

    if (ImGui::Button("Add layer", ImVec2(150, 24))) {
        if (!TagLayerManager::Instance().getLayer(layer_name)) {
            TagLayerManager::Instance().addLayer(layer_name);
        }
    }

    if (ImGui::Button("Delete layer", ImVec2(150, 24))) {
        if (TagLayerManager::Instance().getLayer(layer_name)) {
            TagLayerManager::Instance().removeLayer(layer_name);
        }
    }


    ImGui::End();
}

void Editor::propertiesWindowDisplay(SceneGraph* root, Node* preview_node, float x, float y, float width, float height) {

    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoSavedSettings;

    ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);

    ImGui::Begin("Properties", nullptr, window_flags);

    if (preview_node) {

        // Name and rename section
        static char name_buffer[128];
        static std::string current_name;

        // If swap selection, update current name and fill the buffer with the newly selected node's name
        if (current_name != preview_node->getName()) {
            current_name = preview_node->getName();
            strncpy_s(name_buffer, current_name.c_str(), sizeof(name_buffer));
            name_buffer[sizeof(name_buffer) - 1] = '\0'; // Safety null terminator
        }

        ImGui::Text("Name: ");
        ImGui::SameLine();
        ImGui::PushItemWidth(-1);

        if (ImGui::InputText("##NodeName", name_buffer, sizeof(name_buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
            preview_node->rename(std::string(name_buffer));
            current_name = preview_node->getName();
        }
        ImGui::PopItemWidth();

        ImGui::Separator();

        // ----------------- //

        // Transform section
        static glm::vec3 position_buffer, rotation_buffer, scale_buffer;
        static std::string last_node_id = "";
        auto& T = preview_node->transform;

        if (preview_node->getName() != last_node_id) {

            position_buffer = T.getLocalPosition();
            // Convert quaternion to euler angles
            glm::vec3 euler = glm::degrees(glm::eulerAngles(T.getLocalRotation()));
            rotation_buffer = euler;
            scale_buffer = T.getLocalScale();
            last_node_id = preview_node->id;

        }

        // How wide is a single number field
        const float field_width = 50.0f;

        // --- Position ---
        ImGui::Text("Position: "); ImGui::SameLine();
        ImGui::PushID("pos");
        ImGui::PushItemWidth(field_width);
        if (ImGui::DragFloat("X##pos", &position_buffer.x, 1.0f, -1000000.f, 1000000.f, "%.1f")) {
            T.setLocalPosition(position_buffer);
        }
        ImGui::PopItemWidth(); ImGui::SameLine();
        ImGui::PushItemWidth(field_width);
        if (ImGui::InputFloat("Y##pos", &position_buffer.y, 0.0f, 0.0f, "%.3f")) {
            T.setLocalPosition(position_buffer);
        }
        ImGui::PopItemWidth(); ImGui::SameLine();
        ImGui::PushItemWidth(field_width);
        if (ImGui::InputFloat("Z##pos", &position_buffer.z, 0.0f, 0.0f, "%.3f")) {
            T.setLocalPosition(position_buffer);
        }
        ImGui::PopItemWidth();
        ImGui::PopID();

        // --- Rotation ---
        ImGui::Text("Rotation: "); ImGui::SameLine();
        ImGui::PushID("rot");
        ImGui::PushItemWidth(field_width);
        if (ImGui::InputFloat("X##rot", &rotation_buffer.x, 0.0f, 0.0f, "%.1f")) {
            T.setLocalRotation(glm::vec3(rotation_buffer.x, rotation_buffer.y, rotation_buffer.z));
        }
        ImGui::PopItemWidth(); ImGui::SameLine();
        ImGui::PushItemWidth(field_width);
        if (ImGui::InputFloat("Y##rot", &rotation_buffer.y, 0.0f, 0.0f, "%.1f")) {
            T.setLocalRotation(glm::vec3(rotation_buffer.x, rotation_buffer.y, rotation_buffer.z));
        }
        ImGui::PopItemWidth(); ImGui::SameLine();
        ImGui::PushItemWidth(field_width);
        if (ImGui::InputFloat("Z##rot", &rotation_buffer.z, 0.0f, 0.0f, "%.1f")) {
            T.setLocalRotation(glm::vec3(rotation_buffer.x, rotation_buffer.y, rotation_buffer.z));
        }
        ImGui::PopItemWidth();
        ImGui::PopID();

        // --- Scale ---
        ImGui::Text("Scale:"); ImGui::SameLine();
        ImGui::PushID("scl");
        ImGui::PushItemWidth(field_width);
        if (ImGui::InputFloat("X##scl", &scale_buffer.x, 0.0f, 0.0f, "%.3f")) {
            T.setLocalScale(scale_buffer);
        }
        ImGui::PopItemWidth(); ImGui::SameLine();
        ImGui::PushItemWidth(field_width);
        if (ImGui::InputFloat("Y##scl", &scale_buffer.y, 0.0f, 0.0f, "%.3f")) {
            T.setLocalScale(scale_buffer);
        }
        ImGui::PopItemWidth(); ImGui::SameLine();
        ImGui::PushItemWidth(field_width);
        if (ImGui::InputFloat("Z##scl", &scale_buffer.z, 0.0f, 0.0f, "%.3f")) {
            T.setLocalScale(scale_buffer);
        }
        ImGui::PopItemWidth();
        ImGui::PopID();

        ImGui::Separator();

        ImGui::Checkbox("Physic collider active:", &preview_node->is_physic_active);
        ImGui::Checkbox("Logic collider active:", &preview_node->is_logic_active);

        ImGui::Separator();
        // Parent and reparent section

        // Get viable parent options
        std::set<Node*> parent_options = root->root->getAllChildren();
        parent_options.erase(preview_node);

        parent_options.insert(root->root);

        static std::string last_preview_name;
        static std::string selected_parent_name;
        if (last_preview_name != preview_node->getName()) {
            selected_parent_name = preview_node->parent ? preview_node->parent->getName() : root->root->getName();
            last_preview_name = preview_node->getName();
        }

        ImGui::Text("Parent: "); ImGui::SameLine();
        ImGui::PushItemWidth(-1);
        if (ImGui::BeginCombo("##ParentCombo", selected_parent_name.c_str())) {
            for (auto potential_parent : parent_options) {
                bool is_sel = (potential_parent->getName() == selected_parent_name);
                if (ImGui::Selectable(potential_parent->getName().c_str(), is_sel)) {
                    if (preview_node != root->root) {
                        // reparent
                        if (preview_node->parent) {
                            preview_node->parent->children.erase(preview_node);
                        }
                        potential_parent->children.insert(preview_node);
                        preview_node->parent = potential_parent;
                        selected_parent_name = potential_parent->getName();
                    }
                }
                if (is_sel)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::PopItemWidth();

		ImGui::Separator();

		ImGui::Text("Tag: ");
		ImGui::SameLine();

		std::vector<const char*> items;

        int current_tag = 0, i = 0;

        for (auto& s : TagLayerManager::Instance().getTags()) {
            
			items.push_back(s->name.c_str());
			if (preview_node->getTagName() == s->name) {
				current_tag = i;
			}
            i++;
        }

        if (ImGui::Combo("Tag", &current_tag, items.data(), items.size())) {
			preview_node->setTag(TagLayerManager::Instance().getTag(items[current_tag]));
        }

		items.clear();

        current_tag = 0, i = 0;

        for (auto& s : TagLayerManager::Instance().getLayers()) {

            items.push_back(s->name.c_str());
            if (preview_node->getLayerName() == s->name) {
                current_tag = i;
            }
            i++;
        }

        if (ImGui::Combo("Layer", &current_tag, items.data(), items.size())) {
            preview_node->setLayer(TagLayerManager::Instance().getLayer(items[current_tag]));
        }

		ImGui::Separator();

        if (dynamic_cast<Light*>(preview_node)) {

            Light* temp = dynamic_cast<Light*>(preview_node);

            ImGui::Separator();
            ImGui::Text("Light Properties");

            ImGui::ColorEdit3("Ambient", glm::value_ptr(temp->ambient));
            ImGui::ColorEdit3("Diffuse", glm::value_ptr(temp->diffuse));
            ImGui::ColorEdit3("Specular", glm::value_ptr(temp->specular));

            if (dynamic_cast<DirectionalLight*>(preview_node)) {

                DirectionalLight* temp = dynamic_cast<DirectionalLight*>(preview_node);
                ImGui::Separator();

                ImGui::Text("Directional Light Properties");
                ImGui::DragFloat3("Direction##direction", glm::value_ptr(temp->direction), 0.1f, -1.f, 1.f, "%.1f");

            }
            else if (dynamic_cast<PointLight*>(preview_node)) {

                PointLight* temp = dynamic_cast<PointLight*>(preview_node);
                ImGui::Separator();

                ImGui::Text("Point Light Properties");

                ImGui::PushID("quad");

                ImGui::PushItemWidth(field_width);
                ImGui::DragFloat("Quadratic##q", &temp->quadratic, 0.001f, 0.001f, 1.0f);
                ImGui::PopItemWidth();

                ImGui::PopID();

                ImGui::PushID("lin");

                ImGui::PushItemWidth(field_width);
                ImGui::DragFloat("Linear##l", &temp->linear, 0.001f, 0.001f, 1.0f);
                ImGui::PopItemWidth();

                ImGui::PopID();

                ImGui::PushID("const");

                ImGui::PushItemWidth(field_width);
                ImGui::DragFloat("Constant##c", &temp->constant, 0.001f, 0.f, 1.0f);
                ImGui::PopItemWidth();

                ImGui::PopID();

            }

        }

        ImGui::Separator();

		ImGui::Text("Physical Collider: ");

        ImGui::Checkbox("Change AABB: ", &AABB_changing);

        ImVec2 cursorPos = ImGui::GetCursorScreenPos();
        int depth = 10.f;
        ImVec2 fieldPos = ImVec2(cursorPos.x + depth, cursorPos.y + 20.f);

        if (preview_node->AABB) {

            ImGui::SetCursorScreenPos(fieldPos);
            ImGui::InputFloat3("##Min_phys", glm::value_ptr(preview_node->AABB->min_point_local));
            ImGui::SameLine();
            if (ImGui::Button("Min_phys_button")) {
                local_point = &preview_node->AABB->min_point_local;
            }
            cursorPos = ImGui::GetCursorScreenPos();
            fieldPos = ImVec2(cursorPos.x + depth, cursorPos.y + 20.f);
            ImGui::SetCursorScreenPos(fieldPos);
            ImGui::InputFloat3("##Max_phys", glm::value_ptr(preview_node->AABB->max_point_local));
            ImGui::SameLine();
            if (ImGui::Button("Max_phys_button")) {
                local_point = &preview_node->AABB->max_point_local;
            }
        }
        if (preview_node->AABB_logic) {
            ImGui::Text("Logic Collider: ");

            cursorPos = ImGui::GetCursorScreenPos();
            fieldPos = ImVec2(cursorPos.x + depth, cursorPos.y + 20.f);
            ImGui::SetCursorScreenPos(fieldPos);
            ImGui::InputFloat3("##Min_logic", glm::value_ptr(preview_node->AABB_logic->min_point_local));
            ImGui::SameLine();
            if (ImGui::Button("Min_logic_button")) {
                local_point = &preview_node->AABB_logic->min_point_local;
            }
            cursorPos = ImGui::GetCursorScreenPos();
            fieldPos = ImVec2(cursorPos.x + depth, cursorPos.y + 20.f);
            ImGui::InputFloat3("##Max_logic", glm::value_ptr(preview_node->AABB_logic->max_point_local));
            ImGui::SameLine();
            if (ImGui::Button("Max_logic_button")) {
                local_point = &preview_node->AABB_logic->max_point_local;
            }

        }
        preview_node->forceUpdateSelfAndChild();
		ImGui::Separator();

        if (!AABB_changing) {
			local_point = nullptr;
        }

        for (auto component = preview_node->components.begin(); component != preview_node->components.end(); ) {
            
            ImGui::Text((*component)->name.c_str());

            ImGui::SameLine();
			ImGui::PushID((*component)->name.c_str());
            if (ImGui::Button("-")) {
                
                preview_node->deleteComponent(component);
                component = preview_node->components.begin();
            }
            else {

                Script* script = dynamic_cast<Script*>(component->get());

                //jeśli jest pochodną Skryptu
                if (script) {

                    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
                    int depth = 10.f, _i = 0;
                    for (Variable* field : script->getFields()) {

                        ImVec2 fieldPos = ImVec2(cursorPos.x + depth, cursorPos.y + _i * 20.f);

                        ImGui::SetCursorScreenPos(fieldPos);
                        _i++;
                        void* ptr = reinterpret_cast<char*>(script) + field->offset;
                        if (field->type == "float") {
                            float* f = reinterpret_cast<float*>(ptr);
                            ImGui::Text(field->name.c_str());
                            ImGui::SameLine();
                            ImGui::DragFloat(("##" + field->name).c_str(), f, 0.1f);
                        }
                        else if (field->type == "int") {
                            int* i = reinterpret_cast<int*>(ptr);
                            ImGui::Text(field->name.c_str());
                            ImGui::SameLine();
                            ImGui::DragInt(("##" + field->name).c_str(), i);
                        }
                        else if (field->type == "bool") {
                            bool* b = reinterpret_cast<bool*>(ptr);
                            ImGui::Text(field->name.c_str());
                            ImGui::SameLine();
                            ImGui::Checkbox(("##" + field->name).c_str(), b);
                        }
                        else if (field->type == "string") {
                            std::string* s = reinterpret_cast<std::string*>(ptr);
                            char* buffer = new char[128];
                            ImGui::Text(field->name.c_str());
                            ImGui::SameLine();
                            ImGui::InputText(("##" + field->name).c_str(), buffer, 128);
                            *s = std::string(buffer);
                            delete[] buffer;
                        }
                        else if (field->type == "Node*") {
                            Node** n = reinterpret_cast<Node**>(ptr);
                            ImGui::Text(field->name.c_str());
                            ImGui::SameLine();
                            ImGui::PushItemWidth(-1);

                            std::string combo_id = "##NodeCombo_" + field->name;

                            if (ImGui::BeginCombo(combo_id.c_str() , (*n) ? (*n)->getName().c_str() : "brak")) {

                                bool is_none_selected = (*n == nullptr);
                                if (ImGui::Selectable("<brak>", is_none_selected)) {
                                    *n = nullptr;
                                }

                                if (is_none_selected)
                                    ImGui::SetItemDefaultFocus();

                                for (Node* potential_parent : parent_options) {
                                    bool is_sel = (*n == potential_parent);
                                    if (ImGui::Selectable(potential_parent->getName().c_str(), is_sel)) {
                                        *n = potential_parent;
                                    }
                                    if (is_sel)
                                        ImGui::SetItemDefaultFocus();
                                }
                                ImGui::EndCombo();
                            }
                            ImGui::PopItemWidth();
                        }


                    }


                
                }
                else if (component->get()->name == "Rigidbody") {
					Rigidbody* rb = dynamic_cast<Rigidbody*>(component->get());
                    
                    int depth = 10.f;

                    // Mass
                    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
                    ImVec2 fieldPos = ImVec2(cursorPos.x + depth, cursorPos.y);

                    ImGui::SetCursorScreenPos(fieldPos);
					ImGui::Text("Mass: ");
					ImGui::SameLine();
					ImGui::DragFloat("##mass", &rb->mass, 0.1f);

                    // Gravity
                    cursorPos = ImGui::GetCursorScreenPos();
                    fieldPos = ImVec2(cursorPos.x + depth, cursorPos.y);

                    ImGui::SetCursorScreenPos(fieldPos);
					ImGui::Text("Gravity: ");
					ImGui::SameLine();
					ImGui::DragFloat("##gravity", &rb->gravity, 0.1f);

                    // Static
                    cursorPos = ImGui::GetCursorScreenPos();
                    fieldPos = ImVec2(cursorPos.x + depth, cursorPos.y);
                    ImGui::SetCursorScreenPos(fieldPos);
					ImGui::Text("Static: ");
					ImGui::SameLine();
					ImGui::Checkbox("##static", &rb->is_static);

                    // Lock position X
					cursorPos = ImGui::GetCursorScreenPos();
					fieldPos = ImVec2(cursorPos.x + depth, cursorPos.y);
					ImGui::SetCursorScreenPos(fieldPos);
					ImGui::Text("Lock position X: ");
					ImGui::SameLine();
					ImGui::Checkbox("##lockPositionX", &rb->lockPositionX);

					// Lock position Y
					cursorPos = ImGui::GetCursorScreenPos();
					fieldPos = ImVec2(cursorPos.x + depth, cursorPos.y);
					ImGui::SetCursorScreenPos(fieldPos);
					ImGui::Text("Lock position Y: ");
					ImGui::SameLine();
					ImGui::Checkbox("##lockPositionY", &rb->lockPositionY);

					// Lock position Z
					cursorPos = ImGui::GetCursorScreenPos();
					fieldPos = ImVec2(cursorPos.x + depth, cursorPos.y);
					ImGui::SetCursorScreenPos(fieldPos);
					ImGui::Text("Lock position Z: ");
					ImGui::SameLine();
					ImGui::Checkbox("##lockPositionZ", &rb->lockPositionZ);   
                }


                
            }
            if (component != preview_node->components.end()) {
                ++component;
            }
            
            ImGui::PopID();

            

        }

        ImGui::Separator();
        ImGui::Text("Available animations: ");
        depth = 10.f;
        cursorPos = ImGui::GetCursorScreenPos();
        fieldPos = ImVec2(cursorPos.x + depth, cursorPos.y);
        if (preview_node->pModel && preview_node->pModel->has_animations) {

            for (auto& a : preview_node->pModel->animations) {
                cursorPos = ImGui::GetCursorScreenPos();
                fieldPos = ImVec2(cursorPos.x + depth, cursorPos.y);
                ImGui::SetCursorScreenPos(fieldPos);
                ImGui::Text(a->name.c_str());
            }

        }
        else {
            ImGui::SetCursorScreenPos(fieldPos);
            ImGui::Text("--none--");
        }
    }

    /// --- POST - PROCESS --- ///

    else {

        ImGui::Text("GUI EDITOR");
        ImGui::Checkbox("GUI Show Enable/Disable", &show_gui);

        std::vector<const char*> items;
        std::vector<const char*> sprites;
        std::vector<const char*> texts;

        items.push_back("Text");
        items.push_back("Sprite");

        ImGui::Combo("Choose object type: ", &text_sprite, items.data(), items.size());

        auto& _t = GuiManager::Instance().getText();
        for (auto& o : _t) {
            texts.push_back(o.second->name.c_str());
        }

        auto& _s = GuiManager::Instance().getSprites();
        for (auto& o : _s) {
            sprites.push_back(o.second->name.c_str());
        }
        
        if (text_sprite == 0) { //Text


            ImGui::Combo("Choose font and size: ", &text_id, texts.data(), texts.size());
            ImGui::DragFloat("Pos x ", &pos_x);
            ImGui::DragFloat("Pos y ", &pos_y);
            ImGui::DragInt("Order id ", &order_id, 1, 0, 1000);
            float colors[3] = { color.x, color.y, color.z };
            if (ImGui::ColorEdit3("Text Color ", colors)) {
                color.x = colors[0];
                color.y = colors[1];
                color.z = colors[2];
            }
            ImGui::InputText("Text value ", UI_text, 256);

            if (ImGui::Button("ADD Text")) {
                GuiManager::Instance().text(string(UI_text), pos_x, pos_y, (Text_names) text_id, color, order_id);
            }
        }
        else { //Sprite


            ImGui::Combo("Choose font and size: ", &sprite_id, sprites.data(), sprites.size());
            ImGui::DragFloat("Pos x ", &pos_x);
            ImGui::DragFloat("Pos y ", &pos_y);
            ImGui::DragFloat("Size ", &size, 1.f, 0.001f, 100.f);
            ImGui::DragInt("Order id ", &order_id, 1, 0, 1000);

            if (ImGui::Button("ADD Sprite")) {
                GuiManager::Instance().sprite(pos_x, pos_y, size, (Sprite_names)sprite_id, order_id);
            }
        }
        ImGui::Separator();
        
        auto& objects = GuiManager::Instance().getObjects();

        float before = -1, depth = 20.f;
        int _size = objects.size();
        for (int i = 0; i < _size; i++) {
            ImGui::PushID(i);

            if (objects[i]->order_id > before) {
                ImGui::Text(("Layer " + std::to_string(objects[i]->order_id)).c_str());
                before = objects[i]->order_id;
            }

            ImVec2 cursorPos = ImGui::GetCursorScreenPos();
            ImVec2 itemPos = ImVec2(cursorPos.x + depth, cursorPos.y);
            ImGui::SetCursorScreenPos(itemPos);

            if (objects[i]->getType() == TextType) {
                TextObject* t = static_cast<TextObject*>(objects[i]);
                ImGui::Text(("ID: " + std::to_string(t->id) + " : Text").c_str());

                ImGui::SameLine();
                if (ImGui::SmallButton("Delete")) {
                    GuiManager::Instance().deleteText(t->id);
                    i--;
                    _size--;
                    ImGui::PopID();
                    continue;
                }
                
                cursorPos = ImGui::GetCursorScreenPos();
                itemPos = ImVec2(cursorPos.x + depth, cursorPos.y);
                ImGui::SetCursorScreenPos(itemPos);
                int index = t->text_id, order_index = t->order_id;
                glm::vec2 pos = t->pos;
                if (ImGui::Combo("Font and size: ", &index, texts.data(), texts.size())) {
                    t->text_id = index;
                    t->text = GuiManager::Instance().getText()[index];
                }

                cursorPos = ImGui::GetCursorScreenPos();
                itemPos = ImVec2(cursorPos.x + depth, cursorPos.y);
                ImGui::SetCursorScreenPos(itemPos);

                ImGui::DragFloat("Pos x ", &pos.x);

                cursorPos = ImGui::GetCursorScreenPos();
                itemPos = ImVec2(cursorPos.x + depth, cursorPos.y);
                ImGui::SetCursorScreenPos(itemPos);

                ImGui::DragFloat("Pos y ", &pos.y);

                cursorPos = ImGui::GetCursorScreenPos();
                itemPos = ImVec2(cursorPos.x + depth, cursorPos.y);
                ImGui::SetCursorScreenPos(itemPos);

                ImGui::InputInt("Order id ", &order_index);

                cursorPos = ImGui::GetCursorScreenPos();
                itemPos = ImVec2(cursorPos.x + depth, cursorPos.y);
                ImGui::SetCursorScreenPos(itemPos);

                float colors[3] = { t->color.x, t->color.y, t->color.z };
                if (ImGui::ColorEdit3("Text Color ", colors)) {
                    t->color.x = colors[0];
                    t->color.y = colors[1];
                    t->color.z = colors[2];
                }

                cursorPos = ImGui::GetCursorScreenPos();
                itemPos = ImVec2(cursorPos.x + depth, cursorPos.y);
                ImGui::SetCursorScreenPos(itemPos);

                char value[256];
                strncpy_s(value, t->value.c_str(), sizeof(value));
                value[sizeof(value) - 1] = '\0';
                ImGui::InputText("Text value ", value, 256);

                t->value = string(value);
                t->pos = pos;
                t->order_id = order_index;
            }
            else {
                SpriteObject* s = static_cast<SpriteObject*>(objects[i]);
                ImGui::Text(("ID: " + std::to_string(s->id) + " : Sprite").c_str());

                ImGui::SameLine();
                if (ImGui::SmallButton("Delete")) {
                    GuiManager::Instance().deleteSprite(s->id);
                    i--;
                    _size--;
                    ImGui::PopID();
                    continue;
                }

                cursorPos = ImGui::GetCursorScreenPos();
                itemPos = ImVec2(cursorPos.x + depth, cursorPos.y);
                ImGui::SetCursorScreenPos(itemPos);
                int index = s->sprite_id, order_index = s->order_id;
                glm::vec2 pos = s->pos;
                float size_s = s->size;
                if (ImGui::Combo("Sprite: ", &index, sprites.data(), sprites.size())) {
                    s->sprite_id = index;
                    s->sprite = GuiManager::Instance().getSprites()[index];
                }

                cursorPos = ImGui::GetCursorScreenPos();
                itemPos = ImVec2(cursorPos.x + depth, cursorPos.y);
                ImGui::SetCursorScreenPos(itemPos);

                ImGui::DragFloat("Pos x ", &pos.x);

                cursorPos = ImGui::GetCursorScreenPos();
                itemPos = ImVec2(cursorPos.x + depth, cursorPos.y);
                ImGui::SetCursorScreenPos(itemPos);

                ImGui::DragFloat("Pos y ", &pos.y);

                cursorPos = ImGui::GetCursorScreenPos();
                itemPos = ImVec2(cursorPos.x + depth, cursorPos.y);
                ImGui::SetCursorScreenPos(itemPos);

                ImGui::InputInt("Order id ", &order_index);

                cursorPos = ImGui::GetCursorScreenPos();
                itemPos = ImVec2(cursorPos.x + depth, cursorPos.y);
                ImGui::SetCursorScreenPos(itemPos);

                ImGui::DragFloat("Size ", &size_s, 1.f, 0.001f, 100.f);

                s->size = size_s;
                s->pos = pos;
                s->order_id = order_index;
            }

            ImGui::PopID();
            ImGui::Separator();
        }

        //ImGui::Separator();
        ImGui::Text("POSTPROCESS");

        ImGui::Checkbox("Post-Process Enable/Disable", &postProcessData.is_post_process);
        
        if (postProcessData.is_post_process) {

            ImGui::Separator();

            ImGui::Checkbox("SSAO Enable/Disable", &postProcessData.is_ssao);

            if (postProcessData.is_ssao) {

                ImGui::Separator();

                ImGui::Text("SSAO Kernel Samples:");
                if (ImGui::RadioButton("16", postProcessData.ssao_kernel_samples == 16)) postProcessData.ssao_kernel_samples = 16;
                if (ImGui::RadioButton("32", postProcessData.ssao_kernel_samples == 32)) postProcessData.ssao_kernel_samples = 32;
                if (ImGui::RadioButton("64", postProcessData.ssao_kernel_samples == 64)) postProcessData.ssao_kernel_samples = 64;
                if (ImGui::RadioButton("128", postProcessData.ssao_kernel_samples == 128)) postProcessData.ssao_kernel_samples = 128;

                ImGui::SliderFloat("SSAO Radius", &postProcessData.ssao_radius, 0.01f, 5.0f, "%.2f");

                ImGui::SliderFloat("SSAO Bias", &postProcessData.ssao_bias, 0.001f, 0.5f, "%.3f");

                ImGui::SliderFloat("SSAO Intensity", &postProcessData.ssao_intensity, 0.0f, 5.0f, "%.1f");

                ImGui::SliderFloat("SSAO Noise Scale X", &postProcessData.ssao_noise_scale.x, 0.1f, 1000.0f, "%.1f");
                ImGui::SliderFloat("SSAO Noise Scale Y", &postProcessData.ssao_noise_scale.y, 0.1f, 1000.0f, "%.1f");

                ImGui::Separator();

            }

            ImGui::Checkbox("Bloom Enable/Disable", &postProcessData.is_bloom);

            if (postProcessData.is_bloom) {

                ImGui::Separator();

                ImGui::SliderFloat("Bloom Treshold", &postProcessData.bloom_treshold, 0.0f, 1.0f, "%.2f");

                ImGui::Separator();

            }

            ImGui::Checkbox("CRT Enable/Disable", &postProcessData.is_crt_curved);

            if (postProcessData.is_crt_curved) {

                ImGui::Separator();

                ImGui::Text("CRT Curvature:");
                ImGui::DragFloat("X", &postProcessData.crt_curvature.x, 0.1f, 1.0f, 15.0f, "%.1f");
                ImGui::DragFloat("Y", &postProcessData.crt_curvature.y, 0.1f, 1.0f, 10.0f, "%.1f");

                ImGui::ColorEdit3("CRT Outline", glm::value_ptr(postProcessData.crt_outline_color));

                ImGui::Separator();

                ImGui::Text("CRT Lines:");
                ImGui::DragFloat("X Resolution", &postProcessData.crt_screen_resolution.x, 1.0f, 0.0f, 1000.0f, "%.1f");
                ImGui::DragFloat("Y Resolution", &postProcessData.crt_screen_resolution.y, 1.0f, 0.0f, 1000.0f, "%.1f");

                ImGui::DragFloat("X Factor", &postProcessData.crt_lines_sinusoid_factor.x, 0.1f, 0.01f, 3.0f, "%.2f");
                ImGui::DragFloat("Y Factor", &postProcessData.crt_lines_sinusoid_factor.y, 0.1f, 0.01f, 3.0f, "%.2f");

                ImGui::Separator();

                ImGui::Text("Vignette:");

                ImGui::DragFloat("X Radius", &postProcessData.crt_vignette_radius, 1.0f, 0.0f, 1000.0f, "%.1f");

                ImGui::DragFloat("Power Factor", &postProcessData.crt_vignette_factor, 0.1f, 0.01f, 6.0f, "%.2f");

                ImGui::Separator();

                ImGui::ColorEdit3("CRT Brightness", glm::value_ptr(postProcessData.crt_brightness));

            }
        }
    }

    ImGui::End();

}

void Editor::init()
{
    is_initialized = true;
    IMGUI_CHECKVERSION();                          // (1) wersja ImGui
    ImGui::CreateContext();                        // (2) tworzy globalny kontekst
    ImGuiIO& io = ImGui::GetIO(); (void)io;        // (3) dostęp do konfiguracji
    ImGui::StyleColorsDark();                      // (4) styl

    ImGui_ImplGlfw_InitForOpenGL(
        ServiceLocator::getWindow()->window, true  // (5) backend platformy
    );
    ImGui_ImplOpenGL3_Init("#version 330");        // (6) backend renderera

    GuiManager::Instance().init();

    loadScene("res/scene/scene.json", editor_sceneGraph, prefabs, puzzle_prefabs);

    loadPostProcessData("res/scene/postprocess_data.json", postProcessData);

    editor_sceneGraph->forcedUpdate();

    int fbWidth = previewWidth;
    int fbHeight = previewHeight;

    camera->FarPlane = 100.f;

    glfwSwapInterval(0); // Ustawienie V-Sync w GLFW bo gra swiruje na kompie

    // 1. Tekstura kolorów
    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fbWidth, fbHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // 2. Bufor głębokości
    glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, fbWidth, fbHeight);

    // 3. Framebuffer
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer nie jest kompletny!" << std::endl;
    }
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    Texture _texture;

    _texture.id = textureFromFile("res/textures/raindrop.png");
    //_texture.path = "res/textures/snowflake.png";
    _texture.path = "res/textures/raindrop.png";
    _texture.type = "diffuse";

    //ParticleEmitter* ziom = new ParticleEmitter(_texture.id, 10000);
    //ParticleEmitter* ziom = new ParticleEmitter(_texture, 20000);
    //ziom->transform.setLocalScale({ 0.1f, .1f, .1f });
    //editor_sceneGraph->addChild(ziom);
    glm::vec3 pos = glm::vec3(0.f, 1.f, 0.f);
    glm::mat4 model = glm::translate(glm::mat4(1.f), pos);
    glm::mat4 quat = glm::rotate(glm::mat4(1.f), glm::radians(45.f), glm::vec3(0.f, 0.f, 1.f));
    model = model * quat;
    Ray ray;
    ray.direction = { 1.f, 0.f, 0.f };
    emit = editor_sceneGraph->root->getChildByName("LASER");
        // new LaserEmitterNode(ResourceManager::Instance().getModel(4), "LASER", ray);

    //mirror = new MirrorNode(ResourceManager::Instance().getModel(13), "mirror");
    //editor_sceneGraph->addChild(emit);



    glGenTextures(1, &debugColorTex);
    glBindTexture(GL_TEXTURE_2D, debugColorTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, debugColorTex, 0);


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

}

void Editor::run() {
    if (!is_initialized) {
        init();
    }
    while (!glfwWindowShouldClose(ServiceLocator::getWindow()->window) && !play) {
        
            sceneGraph = (scene_editor || prefabs.empty())
                ? editor_sceneGraph
                : (puzz ? puzzle_prefabs[current_puzzle]->prefab_scene_graph : prefabs[current_prefab]->prefab_scene_graph);

            GLfloat currentFrame = glfwGetTime();
            ServiceLocator::getWindow()->deltaTime = currentFrame - ServiceLocator::getWindow()->lastFrame;
            ServiceLocator::getWindow()->lastFrame = currentFrame;

            if (ServiceLocator::getWindow()->deltaTime > 0.25f) {
                ServiceLocator::getWindow()->deltaTime = 0.0f;
            }

            
            

            frames++;
            fps_timer += ServiceLocator::getWindow()->deltaTime;
            if (fps_timer >= 1.0f) {
                fps = frames / fps_timer;
                fps_timer = 0.0f;
                frames = 0;
            }

            input();
            update(ServiceLocator::getWindow()->deltaTime);
            draw();

            ServiceLocator::getWindow()->updateWindow();
        

    }
    if (glfwWindowShouldClose(ServiceLocator::getWindow()->window)) engine_work = false;
}

void Editor::shutdown()
{
    saveScene("res/scene/scene.json", editor_sceneGraph);
    savePrefabs(prefabs, puzzle_prefabs);

    savePostProcessData("res/scene/postprocess_data.json", postProcessData);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();


	glDeleteTextures(1, &colorTexture);
	glDeleteRenderbuffers(1, &depthRenderbuffer);
	glDeleteFramebuffers(1, &framebuffer);
	//audioEngine.Shutdown();
	//audioEngine.Release();
	//glfwTerminate();
}

// private
void Editor::input()
{
    if (ServiceLocator::getInputManager()) {
        ServiceLocator::getInputManager()->processInput();
    }

	auto* window = ServiceLocator::getWindow();

    changeMouse(window->window);


    if (glfwGetKey(window->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        engine_work = false;
        glfwSetWindowShouldClose(window->window, true);
    }

    ServiceLocator::getWindow()->mouse_pressed = glfwGetMouseButton(window->window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS;

    // Kamera zmiana trybu
    if (glfwGetKey(window->window, GLFW_KEY_C) == GLFW_PRESS && camera->mode != FRONT_ORTO) {
        if (ServiceLocator::getWindow()->is_camera == ServiceLocator::getWindow()->is_camera_prev) {
            ServiceLocator::getWindow()->is_camera = !ServiceLocator::getWindow()->is_camera;
            camera->changeMode(camera->mode == FREE ? FIXED : FREE);
        }
    }
    else {
        ServiceLocator::getWindow()->is_camera_prev = ServiceLocator::getWindow()->is_camera;
    }

    // Dodawanie nowego obiektu (CTRL + D)
    if (glfwGetKey(window->window, GLFW_KEY_D) == GLFW_PRESS && glfwGetKey(window->window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        if (!pressed_add) {
            if (sceneGraph->marked_object != nullptr) {
                if (dynamic_cast<DirectionalLight*>(sceneGraph->marked_object)) {
                    DirectionalLight* temp = dynamic_cast<DirectionalLight*>(sceneGraph->marked_object);
                    DirectionalLight* newNode = new DirectionalLight(temp->pModel, temp->getName() + "_copy", temp->is_shining,
                        temp->direction, temp->ambient, temp->diffuse, temp->specular);
                    newNode->transform.setLocalPosition(temp->transform.getLocalPosition());
                    newNode->transform.setLocalRotation(temp->transform.getLocalRotation());
                    newNode->transform.setLocalScale(temp->transform.getLocalScale());
                    sceneGraph->addDirectionalLight(newNode);
                }
                else if (dynamic_cast<PointLight*>(sceneGraph->marked_object)) {
                    PointLight* temp = dynamic_cast<PointLight*>(sceneGraph->marked_object);
                    PointLight* newNode = new PointLight(temp->pModel, temp->getName() + "_copy", temp->is_shining,
                        temp->quadratic, temp->linear, temp->constant, temp->ambient, temp->diffuse, temp->specular);
                    newNode->transform.setLocalPosition(temp->transform.getLocalPosition());
                    newNode->transform.setLocalRotation(temp->transform.getLocalRotation());
                    newNode->transform.setLocalScale(temp->transform.getLocalScale());
                    sceneGraph->addPointLight(newNode);
                }
                else {
                    Node* newNode = sceneGraph->marked_object->clone("clone", sceneGraph);
                    newNode->setVariablesNodes("clone", newNode, sceneGraph);
                    newNode->transform.setLocalPosition(sceneGraph->marked_object->transform.getLocalPosition() + glm::vec3(1.f, 0.f, 0.f));
                    
                    if (sceneGraph->marked_object->parent == sceneGraph->root) {
                        sceneGraph->addChild(newNode);
                    }
                    else {
                        sceneGraph->addChild(newNode, sceneGraph->marked_object->parent);
                    }
                    sceneGraph->marked_object = newNode;
                }
            }
        }
        pressed_add = true;
    }
    else {
        pressed_add = false;
    }

    // Kierunki
    direction = 0;
    if (glfwGetKey(window->window, GLFW_KEY_W) == GLFW_PRESS) direction += 1;
    if (glfwGetKey(window->window, GLFW_KEY_S) == GLFW_PRESS) direction += 2;
    if (glfwGetKey(window->window, GLFW_KEY_A) == GLFW_PRESS) direction += 4;
    if (glfwGetKey(window->window, GLFW_KEY_D) == GLFW_PRESS && glfwGetKey(window->window, GLFW_KEY_LEFT_CONTROL) != GLFW_PRESS) direction += 8;
    if (glfwGetKey(window->window, GLFW_KEY_SPACE) == GLFW_PRESS) direction += 16;
    if (glfwGetKey(window->window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) direction += 32;
    if (glfwGetKey(window->window, GLFW_KEY_Q) == GLFW_PRESS) direction += 64;
    if (glfwGetKey(window->window, GLFW_KEY_E) == GLFW_PRESS) direction += 128;

    // ImGuizmo zmiany
    if (glfwGetKey(window->window, GLFW_KEY_T) == GLFW_PRESS) currentOperation = ImGuizmo::TRANSLATE;
    if (glfwGetKey(window->window, GLFW_KEY_R) == GLFW_PRESS) currentOperation = ImGuizmo::ROTATE;
    if (glfwGetKey(window->window, GLFW_KEY_Y) == GLFW_PRESS) { currentOperation = ImGuizmo::SCALE; uniformScale = false; }
    if (glfwGetKey(window->window, GLFW_KEY_U) == GLFW_PRESS) { currentOperation = ImGuizmo::SCALE; uniformScale = true; }
    

}

void Editor::update(float deltaTime) {
    // Audio
    //audioEngine.Update();

    auto* window = ServiceLocator::getWindow();

    // Kamera
    camera->ProcessKeyboard(deltaTime, direction);

    LineManager::Instance().clearLines();
    GuiManager::Instance().update(ServiceLocator::getWindow()->deltaTime);
    //PhysicsSystem::instance().updateColliders(sceneGraph);
    //PhysicsSystem::instance().updateCollisions();


    // Scena
    sceneGraph->update(deltaTime);

    sceneGraph->clearDeleteVector();

    //ziom->update(deltaTime, sceneGraph->root, 100);

	//ParticleEmitter* emitter = dynamic_cast<ParticleEmitter*>(sceneGraph->root->getChildByName("ParticleEmitter"));

    //if (emitter)  emitter->update(deltaTime, sceneGraph->root, 400);

  
    // HUD
    if (isHUD) {
        //background->update(deltaTime);
        //sprite->update(deltaTime);
        //sprite3->update(deltaTime);
    }

    if (glfwGetKey(window->window, GLFW_KEY_N) == GLFW_PRESS) angle++;
    if (glfwGetKey(window->window, GLFW_KEY_M) == GLFW_PRESS) angle--;

    //glm::vec4 direction = glm::rotate(glm::mat4(1.f), glm::radians(angle), glm::vec3(0.f, 0.f, 1.f)) * glm::vec4(1.f, 0.f, 0.f, 1.f);
    


    /*Ray ray;
    ray.direction = direction;
    ray.origin = emit->transform.getGlobalPosition();
    ray.length = 100.f;

    std::vector<RayCastHit> nodes;
    Node* lastNode = nullptr;
    std::vector<glm::vec3> points;
    bool checkNext = true;
    while (checkNext) {
        checkNext = false;
        nodes.clear();
        
        if (PhysicsSystem::instance().rayCast(ray, nodes)) {
            int i = 1;

            if (nodes.size() > 1) {
                points.push_back(ray.origin);
                if (lastNode && lastNode == nodes[1].node) i = 2;
                if (dynamic_cast<MirrorNode*>(nodes[i].node)) {
                    MirrorNode* m = dynamic_cast<MirrorNode*>(nodes[i].node);
                    checkNext = true;
                    lastNode = m;
                    ray.direction = m->reflectDirection(ray);
                    if (!nodes[i].is_phys)
                        ray.origin = nodes[i].endPoint;
                    else
                        ray.origin = nodes[i + 1].endPoint;
                    ray.length = 100.f;
                }
                if (!checkNext) points.push_back(nodes[i].endPoint);
            }

            cout << "Przecina i parametr t: " << endl;
            cout << direction.x << ", " << direction.y << ", " << direction.z << endl;
            cout << "size: " << nodes.size() << endl;
        }
    }

    

    emit->forceUpdateSelfAndChild();

    LineManager::Instance().addVertices(points);*/
    sceneGraph->mark(getRayWorld(window->window, camera->GetView(), camera->GetProjection()));

    if (ServiceLocator::getWindow()->mouse_pressed && glfwGetKey(window->window, GLFW_KEY_LEFT_CONTROL)) {
        if (sceneGraph->marked_object != nullptr)
            sceneGraph->marked_object->is_marked = false;

        sceneGraph->marked_object = sceneGraph->new_marked_object;

        if (sceneGraph->marked_object != nullptr)
            sceneGraph->marked_object->is_marked = true;
    }
}

void Editor::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(0.f, 0.f, 0.f, 0.f);

    glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0x00);


    sceneGraph->draw(previewWidth, previewHeight, framebuffer, true);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    ParticleEmitter* emitter = dynamic_cast<ParticleEmitter*>(sceneGraph->root->getChildByName("ParticleEmitter"));

    if (emitter)  emitter->draw(camera->GetView(), camera->GetProjection());

    sceneGraph->drawMarkedObject();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    ResourceManager::Instance().shader_outline->use();
    ResourceManager::Instance().shader_outline->setVec3("color", glm::vec3(0.f, 0.f, 0.8f));
    camera->AABB->draw(*ResourceManager::Instance().shader_outline);

    LineManager::Instance().drawLines();
    if (show_gui) {
        GuiManager::Instance().draw();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	ResourceManager::Instance().shader_debug->use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	ResourceManager::Instance().shader_debug->setInt("depthMap", 0);
    for (PointLight* p : sceneGraph->point_lights) {
        glBindTexture(GL_TEXTURE_CUBE_MAP, p->depthCubemap);
        break;
    }

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ImGui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    DrawHierarchyWindow(sceneGraph->root, 0, 0, WINDOW_WIDTH / 6, 2 * WINDOW_HEIGHT / 3);
    previewDisplay();
    assetBarDisplay(0, 2 * WINDOW_HEIGHT / 3, 2 * WINDOW_WIDTH / 3, WINDOW_HEIGHT / 3);
    operationBarDisplay(2 * WINDOW_WIDTH / 3, 2 * WINDOW_HEIGHT / 3, WINDOW_WIDTH / 3, WINDOW_HEIGHT / 3);
    propertiesWindowDisplay(sceneGraph, sceneGraph->marked_object, 5 * WINDOW_WIDTH / 6, 0, WINDOW_WIDTH / 6, 2 * WINDOW_HEIGHT / 3);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
