#pragma once
#include <memory>
#include <vector>
#include <string>

class Prefab; // forward declaration tylko, jeśli nie wywołujesz metod Prefab

class PrefabRegistry {
public:
    //using PrefabList = std::vector<std::shared_ptr<Prefab>>;

    static void Init(std::vector<std::shared_ptr<Prefab>>* prefabs, std::vector<std::shared_ptr<Prefab>>* prefabs_puzzles);
    static const std::vector<std::shared_ptr<Prefab>>& GetRooms();
    static const std::vector<std::shared_ptr<Prefab>>& GetPuzzles();

    static std::shared_ptr<Prefab> FindRoomByName(const std::string& name);
    static std::shared_ptr<Prefab> FindPuzzleByName(const std::string& name);

private:
    static std::vector<std::shared_ptr<Prefab>>* s_Prefabs;
    static std::vector<std::shared_ptr<Prefab>>* s_Prefabs_puzzles;
};


