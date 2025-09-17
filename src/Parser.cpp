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
    std::string segment;

    while (std::getline(ss, segment, ',')) {
        bool in_string = false;
        if (!items.empty()) {
            const auto& last_item = items.back();
            if (!last_item.empty() && last_item.front() == '\'' && (last_item.back() != '\'' || last_item.length() == 1)) {
                in_string = true;
            }
        }

        if (in_string) {
            items.back() += "," + segment;
        } else {
            // Limpiar espacios en blanco
            size_t first = segment.find_first_not_of(" \t");
            if (std::string::npos != first) {
                segment = segment.substr(first);
            }
            items.push_back(segment);
        }
    }
    return items;
}

Command Parser::parse(const std::string& query) {
    std::string commandStr = query;

    // Limpiar espacios en blanco al inicio/final
    commandStr.erase(0, commandStr.find_first_not_of(" \t\n\r"));
    commandStr.erase(commandStr.find_last_not_of(" \t\n\r") + 1);

    auto tokens = tokenize(commandStr);
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
    if (tokens.size() < 4) return Command{CommandType::UNRECOGNIZED};
    
    Command cmd;
    cmd.type = CommandType::CREATE_TABLE;
    cmd.tableName = tokens[2];
    
    // Juntar el resto de los tokens para formar la lista de columnas
    std::string columnList;
    for (size_t i = 3; i < tokens.size(); ++i) {
        columnList += tokens[i] + " ";
    }
    // Quitar el último espacio extra
    if (!columnList.empty()) columnList.pop_back();

    if (columnList.front() != '(' || columnList.back() != ')') {
        return Command{CommandType::UNRECOGNIZED};
    }

    auto columnDefs = parseParenthesizedList(columnList);
    for (const auto& def : columnDefs) {
        std::stringstream def_ss(def);
        std::string name, typeStr;
        def_ss >> name >> typeStr;

        if (name.empty() || typeStr.empty()) return Command{CommandType::UNRECOGNIZED};

        DataType type;
        if (typeStr == "INTEGER") {
            type = DataType::INTEGER;
        } else if (typeStr == "TEXT") {
            type = DataType::TEXT;
        } else {
            return Command{CommandType::UNRECOGNIZED}; // Tipo no soportado
        }
        cmd.columns.push_back({name, type});
    }
    return cmd;
}

Command Parser::parseInsert(std::vector<std::string>& tokens) {
    // INSERT INTO table_name VALUES (val1,val2,...)
    if (tokens.size() < 5 || tokens[3] != "VALUES") return Command{CommandType::UNRECOGNIZED};

    Command cmd;
    cmd.type = CommandType::INSERT;
    cmd.tableName = tokens[2];

    // Juntar el resto de los tokens para formar la lista de valores
    std::string valueList;
    for (size_t i = 4; i < tokens.size(); ++i) {
        valueList += tokens[i] + " ";
    }
    // Quitar el último espacio extra
    if (!valueList.empty()) valueList.pop_back();

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
        cmd.columnNames.push_back(col);
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