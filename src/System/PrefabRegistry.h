#pragma once
#include <memory>
#include <vector>
#include <string>

class Prefab; // forward declaration tylko, jeśli nie wywołujesz metod Prefab

class PrefabRegistry {
public:
    //using PrefabList = std::vector<std::shared_ptr<Prefab>>;

    static void Init(std::vector<std::shared_ptr<Prefab>>* prefabs);
    static const std::vector<std::shared_ptr<Prefab>>& Get();

    static std::shared_ptr<Prefab> FindByName(const std::string& name);

private:
    static std::vector<std::shared_ptr<Prefab>>* s_Prefabs;
};


