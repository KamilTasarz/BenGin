#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include <memory>
#include <vector>
#include "Script.h"

class ScriptFactory {
public:
    using Creator = std::function<std::unique_ptr<Script>()>;

    // Singleton dla fabryki skryptów
    static ScriptFactory& instance();

    // Rejestracja nowego skryptu
    void registerScript(const std::string& name, Creator creator);

    // Tworzenie skryptu na podstawie jego nazwy
    std::unique_ptr<Script> create(const std::string& name);

    // Pobranie wszystkich skryptów
    const std::unordered_map<std::string, Creator>& getAllScripts() const;

    // Pobranie nazw wszystkich zarejestrowanych skryptów
    std::vector<std::string> getScriptNames() const;

private:
    ScriptFactory() = default;  // Konstruktor prywatny dla singletona

    // Przechowywanie mapy nazw skryptów do funkcji tworzącej
    std::unordered_map<std::string, Creator> creators;
};
