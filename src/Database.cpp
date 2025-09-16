#include "MiniDB/Database.hpp"
#include <iostream> 

void Database::set(const std::string& key, const std::string& value) {
    // Por ahora, una simple búsqueda lineal
    for (auto& pair : storage) {
        if (pair.first == key) {
            pair.second = value;
            return;
        }
    }
    // Si no existe, la añade al final
    storage.push_back({key, value});
}

std::string Database::get(const std::string& key) {
    for (const auto& pair : storage) {
        if (pair.first == key) {
            return pair.second;
        }
    }
    return "Key not found"; 
}
