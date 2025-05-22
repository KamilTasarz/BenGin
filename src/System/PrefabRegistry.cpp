#include "PrefabRegistry.h"

#include "../Basic/Node.h"

std::vector<std::shared_ptr<Prefab>>* PrefabRegistry::s_Prefabs = nullptr;
std::vector<std::shared_ptr<Prefab>>* PrefabRegistry::s_Prefabs_puzzles = nullptr;

void PrefabRegistry::Init(std::vector<std::shared_ptr<Prefab>>* prefabs, std::vector<std::shared_ptr<Prefab>>* prefabs_puzzles) {
    s_Prefabs = prefabs;
    s_Prefabs_puzzles = prefabs_puzzles;
}

const std::vector<std::shared_ptr<Prefab>>& PrefabRegistry::GetRooms() {
    return *s_Prefabs;
}

std::shared_ptr<Prefab> PrefabRegistry::FindRoomByName(const std::string& name) {
    for (auto& prefab : *s_Prefabs) {
        if (prefab->prefab_scene_graph->root->name == name)
            return prefab;
    }
    return nullptr;
}

const std::vector<std::shared_ptr<Prefab>>& PrefabRegistry::GetPuzzles() {
    return *s_Prefabs_puzzles;
}

std::shared_ptr<Prefab> PrefabRegistry::FindPuzzleByName(const std::string& name) {
    for (auto& prefab : *s_Prefabs_puzzles) {
        if (prefab->prefab_scene_graph->root->name == name)
            return prefab;
    }
    return nullptr;
}