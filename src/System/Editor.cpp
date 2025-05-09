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

#include <glm/gtx/matrix_decompose.hpp>



Editor::Editor(std::vector<std::shared_ptr<Prefab>>& prefabsref) : prefabs(prefabsref) {
    icon = new Sprite(1920.f, 1080.f, "res/sprites/icon.png", 0.f, 0.f, 1.f);
    eye_icon = new Sprite(1920.f, 1080.f, "res/sprites/eye.png", 0.f, 0.f, 1.f);
    eye_slashed_icon = new Sprite(1920.f, 1080.f, "res/sprites/eye_slashed.png", 0.f, 0.f, 1.f);

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

    ImVec2 icon_size = ImVec2(lineHeight, lineHeight);
    std::string img_id = "##eye_" + node->name;

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    //ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0)); 
    //ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));

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

            sceneGraph->to_delete = node;

            sceneGraph->marked_object = nullptr;
        }

    }

    if (node == sceneGraph->root && sceneGraph->to_delete) {
        sceneGraph->deleteChild(sceneGraph->to_delete);
        sceneGraph->to_delete = nullptr;
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
            //Model
            Node* p = new Node(ResourceManager::Instance().getModel(id), "entity", colliders);


            sceneGraph->addChild(p);
            sceneGraph->marked_object = p;
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


    ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);

    ImGui::Begin("Operations", nullptr, window_flags);


    if (ImGui::Button("ORTO", ImVec2(100, 24))) {
        camera->mode != FRONT_ORTO ? camera->changeMode(FRONT_ORTO) : camera->changeMode(FREE);
        sceneGraph->grid->gridType = camera->mode == FRONT_ORTO ? GRID_XY : GRID_XZ;
        sceneGraph->grid->Update();
    }
    

    if (ImGui::Button("SAVE", ImVec2(100, 24))) {
        saveScene("res/scene/scene.json", sceneGraph);
    }

    if (ImGui::Button("EDITING_VIEW", ImVec2(100, 24))) {
        sceneGraph->is_editing = !sceneGraph->is_editing;
    }


    if (sceneGraph->marked_object) {
        std::vector<const char*> items;

		items.push_back("Script");

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

		if (ImGui::Button("ADD_COMPONENT", ImVec2(100, 24))) {
			if (current_component == 0) {
				sceneGraph->marked_object->addComponent(ScriptFactory::instance().create(scripts[current_script]));
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


    // wyświetl combo box tylko jeśli jest coś do wyboru
    if (ImGui::Combo("Choose prefab", &current_prefab, items.data(), items.size())) {
        if (current_prefab == size) {
            std::cout << "Wybrano: brak\n";
        }
        else {
            std::cout << "Wybrano: " << items[current_prefab] << "\n";
        }
    }

    

    if (scene_editor) {

        if (ImGui::Button("ADD_PREFAB_INSTANCE", ImVec2(100, 24))) {
            if (size > 0) {
                Node* inst = new PrefabInstance(prefabs[current_prefab], colliders);
                sceneGraph->addChild(inst);
                sceneGraph->marked_object = inst;
            }

        }

        ImGui::Separator();

        const char* opt1[] = { "HORIZONTAL", "VERTICAL" };

        if (ImGui::Combo("Choose direction", &current_opt1, opt1, 2)) {

        }
        ImGui::SameLine();

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


        if (ImGui::Button("CREATE_PREFAB", ImVec2(100, 24))) {

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

        if (ImGui::Button("EDIT_PREFAB", ImVec2(100, 24))) {
            scene_editor = !scene_editor;
        }
    }
    else {
        if (ImGui::Button("RETURN", ImVec2(100, 24))) {
            scene_editor = !scene_editor;
            
        }
    }

    ImGui::Separator();
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
        if (ImGui::InputFloat("X##pos", &position_buffer.x, 0.0f, 0.0f, "%.3f")) {
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

        for (auto component = preview_node->components.begin(); component != preview_node->components.end(); ) {
            ImGui::Text((*component)->name.c_str());

            ImGui::SameLine();

            if (ImGui::Button("-")) {
                
                preview_node->deleteComponent(component);
                component = preview_node->components.begin();
            }
            else {
                ++component;
            }
        }
    }
    else {

        ImGui::Text("Not selected");

    }

    ImGui::End();

}

void Editor::init()
{
	is_initialized = true;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(ServiceLocator::getWindow()->window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    loadScene("res/scene/scene.json", editor_sceneGraph, prefabs, colliders);

    editor_sceneGraph->forcedUpdate();

    int fbWidth = previewWidth;
    int fbHeight = previewHeight;

    // 1. Tekstura kolorów
    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, fbWidth, fbHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
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
}

void Editor::run() {
    if (!is_initialized) {
        init();
    }
    while (!glfwWindowShouldClose(ServiceLocator::getWindow()->window)) {
        
            sceneGraph = (scene_editor || prefabs.empty())
                ? editor_sceneGraph
                : prefabs[current_prefab]->prefab_scene_graph;

            GLfloat currentFrame = glfwGetTime();
            ServiceLocator::getWindow()->deltaTime = currentFrame - ServiceLocator::getWindow()->lastFrame;
            ServiceLocator::getWindow()->lastFrame = currentFrame;

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
}

void Editor::shutdown()
{
    saveScene("res/scene/scene.json", editor_sceneGraph);
    savePrefabs(prefabs);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glDeleteTextures(1, &colorTexture);
	glDeleteRenderbuffers(1, &depthRenderbuffer);
	glDeleteFramebuffers(1, &framebuffer);
	//audioEngine.Shutdown();
	//audioEngine.Release();
	glfwTerminate();
}

// private
void Editor::input()
{
    if (ServiceLocator::getInputManager()) {
        ServiceLocator::getInputManager()->processInput();
    }

	auto* window = ServiceLocator::getWindow();

    changeMouse(window->window);

    if (glfwGetKey(window->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window->window, true);

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
                Node* newNode = new Node(sceneGraph->marked_object->pModel, sceneGraph->marked_object->getName(), colliders, sceneGraph->marked_object->id);
                newNode->transform = sceneGraph->marked_object->transform; // zakładam przeciążenie operatora =
                sceneGraph->addChild(newNode);
                sceneGraph->marked_object = newNode;
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

    // Scena
    sceneGraph->update(deltaTime);

    // HUD
    if (isHUD) {
        //background->update(deltaTime);
        //sprite->update(deltaTime);
        //sprite3->update(deltaTime);
    }

    // Zaznaczanie obiektów myszką
    sceneGraph->root->checkIfInFrustrum();
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

    sceneGraph->draw(previewWidth, previewHeight, framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    sceneGraph->drawMarkedObject();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (isHUD) {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        //background->render(*ResourceManager::Instance().shader_background);
        //sprite2->render(*ResourceManager::Instance().shader_background);
        //sprite3->render(*ResourceManager::Instance().shader_background);
        //sprite->render(*ResourceManager::Instance().shader_background);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        float fpsValue = 1.f / ServiceLocator::getWindow()->deltaTime;
        //text->renderText("Fps: " + to_string(fpsValue), 4.f * WINDOW_WIDTH / 5.f, WINDOW_HEIGHT - 100.f, *ResourceManager::Instance().shader_text, glm::vec3(1.f, 0.3f, 0.3f));
        //text->renderText("We have text render!", 200, 200, *ResourceManager::Instance().shader_text, glm::vec3(0.6f, 0.6f, 0.98f));
    }

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    ResourceManager::Instance().shader_outline->use();
    ResourceManager::Instance().shader_outline->setVec3("color", glm::vec3(0.f, 0.f, 0.8f));
    camera->AABB->draw(*ResourceManager::Instance().shader_outline);
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
