// src/Database.cpp
#include "MiniDB/Database.hpp"
#include <iostream> // Solo para mensajes de error si quieres

void Database::set(const std::string& key, const std::string& value) {
    // Por ahora, una simple búsqueda lineal
    for (auto& pair : storage) {
        if (pair.first == key) {
            pair.second = value; // Actualiza si la clave ya existe
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
    return "Key not found"; // Opcional: manejar el error de otra forma
}
