#include "ScriptFactory.h"

ScriptFactory& ScriptFactory::instance() {
    static ScriptFactory instance;
    return instance;
}

void ScriptFactory::registerScript(const std::string& name, Creator creator) {
    creators[name] = creator;  // Rejestracja skryptu
}

std::unique_ptr<Script> ScriptFactory::create(const std::string& name) {
    auto it = creators.find(name);
    if (it != creators.end()) {
        std::unique_ptr<Script> script = it->second();  // Tworzymy skrypt przy pomocy lambdy
        script->name = it->first;  // Przypisanie nazwy skryptu
        return script;  // Zwrócenie skryptu (przekazywanego przez smart pointer)
    }
    return nullptr;  // Jeśli nie znaleziono skryptu
}

const std::unordered_map<std::string, ScriptFactory::Creator>& ScriptFactory::getAllScripts() const {
    return creators;
}

std::vector<std::string> ScriptFactory::getScriptNames() const {
    std::vector<std::string> names;
    for (const auto& [name, _] : creators) {
        names.push_back(name);  // Przechodzimy po mapie i zbieramy nazwy skryptów
    }
    return names;
}
