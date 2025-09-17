#pragma once

#include "Database.hpp"
#include "Parser.hpp"
#include <string>

class UI
{
public:
    UI();
    void run();

private:
    void displayMenu();
    void handleCreateTable();
    void handleInsert();
    void handleSelect();
    void handleExecuteQuery(); // Para una sola línea
    void handleExecuteScript(); // Para un archivo
    void executeQuery(const std::string& query); // Lógica común
    void handleHelp();

    Database db;
    Parser parser;
};