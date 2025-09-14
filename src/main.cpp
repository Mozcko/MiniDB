#include <iostream>
#include "MiniDB/Database.hpp"

int main() {
    Database db;

    db.set("nombre", "juan");
    db.set("ciudad", "CDMX");

    std::cout << "nombre: " << db.get("nombre") << std::endl;
    std::cout << "Ciudad: " << db.get("ciudad") << std::endl;
    std::cout << "Pais: " << db.get("pais") << std::endl;
    return 0;
}

