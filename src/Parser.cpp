#include "MiniDB/Parser.hpp"
#include <sstream>
#include <algorithm>

// Función auxiliar para dividir un string en tokens
std::vector<std::string> tokenize(const std::string& singleCommand) {
    std::stringstream ss(singleCommand);
    std::string token;
    std::vector<std::string> tokens;
    while (ss >> token) {
        // Eliminar punto y coma final si existe
        // Pero lo dejamos por si se usa en modo interactivo con punto y coma.
        if (!token.empty() && token.back() == ';') {
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
    // Esta función ahora actúa como un despachador para la primera consulta.
    // La lógica de múltiples comandos se manejará en la UI o en un nivel superior.
    // Por simplicidad, aquí procesaremos la primera instrucción que encontremos.
    std::string firstCommand;
    size_t semi_pos = query.find(';');
    if (semi_pos != std::string::npos) {
        firstCommand = query.substr(0, semi_pos);
    } else {
        firstCommand = query;
    }

    // Limpiar espacios en blanco al inicio/final
    firstCommand.erase(0, firstCommand.find_first_not_of(" \t\n\r"));
    firstCommand.erase(firstCommand.find_last_not_of(" \t\n\r") + 1);

    auto tokens = tokenize(firstCommand);
    if (tokens.empty()) return Command{CommandType::UNRECOGNIZED};

    if (tokens[0] == "CREATE" && tokens.size() > 2 && tokens[1] == "TABLE") {
        return parseCreate(tokens);
    }
    if (tokens[0] == "INSERT" && tokens.size() > 2 && tokens[1] == "INTO") {
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
    // SELECT * FROM table_name WHERE col = value
    // SELECT col1,col2 FROM table_name WHERE col = value
    auto fromIt = std::find(tokens.begin(), tokens.end(), "FROM");
    if (fromIt == tokens.end() || fromIt + 1 == tokens.end()) {
        return Command{CommandType::UNRECOGNIZED};
    }

    Command cmd;
    cmd.type = CommandType::SELECT;

    // Columnas
    std::string columnStr;
    for (auto it = tokens.begin() + 1; it != fromIt; ++it) {
        columnStr += *it;
    }

    std::stringstream ss(columnStr);
    std::string col;
    while(std::getline(ss, col, ',')) {
        cmd.columns.push_back(col);
    }

    // Tabla y cláusula WHERE
    auto whereIt = std::find(fromIt, tokens.end(), "WHERE");
    if (whereIt == tokens.end()) { // No hay WHERE
        if (fromIt + 1 >= tokens.end()) return Command{CommandType::UNRECOGNIZED};
        cmd.tableName = *(fromIt + 1);
        if (fromIt + 2 != tokens.end()) return Command{CommandType::UNRECOGNIZED}; // Tokens extra
    } else { // Hay WHERE
        if (fromIt + 1 >= whereIt) return Command{CommandType::UNRECOGNIZED}; // No hay nombre de tabla
        cmd.tableName = *(fromIt + 1);

        // WHERE col = val (espera 4 tokens: WHERE, col, =, val)
        if (std::distance(whereIt, tokens.end()) != 4 || *(whereIt + 2) != "=") {
            return Command{CommandType::UNRECOGNIZED};
        }
        cmd.whereClause = WhereClause{*(whereIt + 1), *(whereIt + 3)};
    }

    return cmd;
}