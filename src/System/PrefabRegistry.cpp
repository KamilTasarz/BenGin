#include "PrefabRegistry.h"

#include "../Basic/Node.h"

std::vector<std::shared_ptr<Prefab>>* PrefabRegistry::s_Prefabs = nullptr;

void PrefabRegistry::Init(std::vector<std::shared_ptr<Prefab>>* prefabs) {
    s_Prefabs = prefabs;
}

const std::vector<std::shared_ptr<Prefab>>& PrefabRegistry::Get() {
    return *s_Prefabs;
}

std::shared_ptr<Prefab> PrefabRegistry::FindByName(const std::string& name) {
    for (auto& prefab : *s_Prefabs) {
        if (prefab->prefab_scene_graph->root->name == name)
            return prefab;
    }
    return nullptr;
}
