#include "MiniDB/UI.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

UI::UI() : db("minidb.db") {}

void UI::displayMenu()
{
    std::cout << "\n--- MiniDB Menú ---\n";
    std::cout << "1. Crear Tabla (Guiado)\n";
    std::cout << "2. Insertar Fila (Guiado)\n";
    std::cout << "3. Seleccionar Datos (Guiado)\n";
    std::cout << "4. Ejecutar Consulta (SQL)\n";
    std::cout << "5. Ejecutar Script SQL desde Archivo\n";
    std::cout << "6. Ayuda\n";
    std::cout << "7. Salir\n";
    std::cout << "-------------------\n";
}

void UI::handleCreateTable()
{
    std::string tableName, column_str;
    std::cout << "Nombre de la tabla: ";
    std::getline(std::cin, tableName);
    std::cout << "Columnas (separadas por coma, ej: id,nombre,email): ";
    std::getline(std::cin, column_str);
    
    std::vector<std::string> columns;
    std::stringstream ss(column_str);
    std::string column;
    while(std::getline(ss, column, ',')) {
        columns.push_back(column);
    }
    
    db.createTable(tableName, columns);
    std::cout << "Comando 'CREATE TABLE' ejecutado.\n";
}

void UI::handleInsert()
{
    std::string tableName;
    std::cout << "Nombre de la tabla: ";
    std::getline(std::cin, tableName);

    auto tableOpt = db.selectFrom(tableName);
    if (!tableOpt) {
        std::cout << "Error: La tabla '" << tableName << "' no existe.\n";
        return;
    }

    Row row;
    for (const auto& col : tableOpt->getColumns()) {
        std::cout << col << ": ";
        std::string value;
        std::getline(std::cin, value);
        row[col] = value;
    }

    if (db.insertInto(tableName, row)) {
        std::cout << "Fila insertada.\n";
    } else {
        std::cout << "Error al insertar la fila.\n";
    }
}

void UI::handleSelect()
{
    std::string tableName;
    std::cout << "Nombre de la tabla: ";
    std::getline(std::cin, tableName);

    auto tableOpt = db.selectFrom(tableName);
    if (!tableOpt) {
        std::cout << "Error: La tabla '" << tableName << "' no existe.\n";
        return;
    }

    const auto& table = *tableOpt;
    for (const auto& col : table.getColumns()) {
        std::cout << col << "\t";
    }
    std::cout << "\n---------------------------------\n";

    for (const auto& row : table.getRows()) {
        for (const auto& col : table.getColumns()) {
            std::cout << row.at(col) << "\t";
        }
        std::cout << "\n";
    }
}

void UI::handleExecuteQuery() {
    std::cout << "minidb> ";
    std::string query;
    if (!std::getline(std::cin, query) || query.empty()) {
        return;
    }
    executeQuery(query);
}

void UI::handleExecuteScript() {
    std::cout << "Ruta del archivo SQL: ";
    std::string filePath;
    std::getline(std::cin, filePath);

    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cout << "Error: No se pudo abrir el archivo '" << filePath << "'.\n";
        return;
    }

    std::string scriptContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    db.executeScript(scriptContent);
    std::cout << "Script '" << filePath << "' ejecutado.\n";
}

void UI::executeQuery(const std::string& query) {
    Command command = parser.parse(query);
    db.execute(command);
}

void UI::handleHelp() {
    std::cout << "MiniDB es una base de datos simple que guarda los datos en 'minidb.db'.\n";
    std::cout << "Puedes usar el modo guiado o ejecutar consultas SQL directamente.\n";
    std::cout << "Ejemplos de SQL: \n";
    std::cout << "  CREATE TABLE usuarios (id,nombre);\n";
    std::cout << "  INSERT INTO usuarios VALUES (1,Juan);\n";
    std::cout << "  SELECT * FROM usuarios;\n";
}

void UI::run()
{
    std::cout << "Bienvenido a MiniDB." << std::endl;

    while (true)
    {
        displayMenu();
        std::cout << "Escoge una opción: ";
        std::string choice;
        std::getline(std::cin, choice);

        if (choice == "1") handleCreateTable();
        else if (choice == "2") handleInsert();
        else if (choice == "3") handleSelect();
        else if (choice == "4") handleExecuteQuery(); // Modo interactivo
        else if (choice == "5") handleExecuteScript();
        else if (choice == "6") handleHelp();
        else if (choice == "7") break;
        else std::cout << "Opción no válida. Por favor, intenta de nuevo.\n";
    }
}