#include "MiniDB/Parser.hpp"
#include <sstream>
#include <algorithm>

// Función auxiliar para dividir un string en tokens
std::vector<std::string> tokenize(const std::string& query) {
    std::stringstream ss(query);
    std::string token;
    std::vector<std::string> tokens;
    while (ss >> token) {
        // Eliminar punto y coma final si existe
        if (token.back() == ';') {
            token.pop_back();
        }
        tokens.push_back(token);
    }
    return tokens;
}

// Función auxiliar para limpiar y separar columnas/valores de paréntesis
std::vector<std::string> parseParenthesizedList(const std::string& listStr) {
    std::vector<std::string> items;
    // Eliminar paréntesis
    std::string content = listStr.substr(1, listStr.size() - 2);
    std::stringstream ss(content);
    std::string item;
    while (std::getline(ss, item, ',')) {
        // Quitar espacios en blanco
        item.erase(std::remove_if(item.begin(), item.end(), isspace), item.end());
        items.push_back(item);
    }
    return items;
}

Command Parser::parse(const std::string& query) {
    auto tokens = tokenize(query);
    if (tokens.empty()) {
        return Command{CommandType::UNRECOGNIZED};
    }

    if (tokens[0] == "CREATE" && tokens.size() > 2 && tokens[1] == "TABLE") {
        return parseCreate(tokens);
    }
    if (tokens[0] == "INSERT" && tokens.size() > 3 && tokens[1] == "INTO") {
        return parseInsert(tokens);
    }
    if (tokens[0] == "SELECT" && tokens.size() > 2) {
        return parseSelect(tokens);
    }

    return Command{CommandType::UNRECOGNIZED};
}

Command Parser::parseCreate(std::vector<std::string>& tokens) {
    // CREATE TABLE table_name (col1,col2,...)
    if (tokens.size() != 4) return Command{CommandType::UNRECOGNIZED};
    
    Command cmd;
    cmd.type = CommandType::CREATE_TABLE;
    cmd.tableName = tokens[2];
    
    std::string columnList = tokens[3];
    if (columnList.front() != '(' || columnList.back() != ')') {
        return Command{CommandType::UNRECOGNIZED};
    }
    cmd.columns = parseParenthesizedList(columnList);
    return cmd;
}

Command Parser::parseInsert(std::vector<std::string>& tokens) {
    // INSERT INTO table_name VALUES (val1,val2,...)
    if (tokens.size() != 5 || tokens[3] != "VALUES") return Command{CommandType::UNRECOGNIZED};

    Command cmd;
    cmd.type = CommandType::INSERT;
    cmd.tableName = tokens[2];

    std::string valueList = tokens[4];
    if (valueList.front() != '(' || valueList.back() != ')') {
        return Command{CommandType::UNRECOGNIZED};
    }
    cmd.values = parseParenthesizedList(valueList);
    return cmd;
}

Command Parser::parseSelect(std::vector<std::string>& tokens) {
    // SELECT * FROM table_name
    // SELECT col1,col2 FROM table_name
    if (tokens.size() < 4 || tokens[tokens.size() - 2] != "FROM") {
        return Command{CommandType::UNRECOGNIZED};
    }

    Command cmd;
    cmd.type = CommandType::SELECT;
    cmd.tableName = tokens.back();

    // Juntar las columnas en un solo string
    std::string columnStr;
    for(size_t i = 1; i < tokens.size() - 2; ++i) {
        columnStr += tokens[i];
    }

    std::stringstream ss(columnStr);
    std::string col;
    while(std::getline(ss, col, ',')) {
        cmd.columns.push_back(col);
    }
    return cmd;
}