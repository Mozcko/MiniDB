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
    void handleExecuteQuery();
    void handleHelp();

    Database db;
    Parser parser;
};