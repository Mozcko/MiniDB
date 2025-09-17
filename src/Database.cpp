#include "MiniDB/Database.hpp"
#include <fstream>
#include <iostream>
#include "MiniDB/Parser.hpp"
#include <sstream>

// El constructor carga la base de datos al ser creado
Database::Database(const std::string &name) : db_name(name)
{
    // Ahora 'name' es un nombre de archivo, no un directorio.
    load();
}

// El destructor guarda la base de datos antes de que el objeto se destruya
Database::~Database()
{
    save();
}

bool Database::createTable(const std::string& tableName, const std::vector<std::string>& columns)
{
    if (tables.find(tableName) != tables.end()) {
        return false; // La tabla ya existe
    }
    tables.emplace(tableName, Table(columns));
    return true;
}

bool Database::insertInto(const std::string& tableName, const Row& row)
{
    auto it = tables.find(tableName);
    if (it == tables.end()) {
        return false; // La tabla no existe
    }
    it->second.insert(row);
    return true;
}

std::optional<Table> Database::selectFrom(const std::string& tableName)
{
    auto it = tables.find(tableName);
    if (it != tables.end()) {
        return it->second;
    }
    return std::nullopt;
}

void Database::executeScript(const std::string& scriptContent) {
    Parser parser;
    std::stringstream scriptStream(scriptContent);
    std::string commandText;

    // Separa el script por punto y coma
    while (std::getline(scriptStream, commandText, ';')) {
        // Ignorar líneas vacías o solo con espacios
        if (commandText.find_first_not_of(" \t\n\r") == std::string::npos) {
            continue;
        }

        Command command = parser.parse(commandText);
        if (command.type != CommandType::UNRECOGNIZED) {
            std::cout << "minidb> " << commandText << ";" << std::endl;
            execute(command);
        }
    }
}

void Database::execute(const Command& command) {
    switch (command.type) {
        case CommandType::CREATE_TABLE: {
            if (createTable(command.tableName, command.columns)) {
                std::cout << "Tabla '" << command.tableName << "' creada.\n";
            } else {
                std::cout << "Error: La tabla '" << command.tableName << "' ya existe.\n";
            }
            break;
        }
        case CommandType::INSERT: {
            auto tableOpt = selectFrom(command.tableName);
            if (!tableOpt) {
                std::cout << "Error: La tabla '" << command.tableName << "' no existe.\n";
                return;
            }
            if (tableOpt->getColumns().size() != command.values.size()) {
                std::cout << "Error: El número de valores no coincide con el número de columnas.\n";
                return;
            }
            Row newRow;
            for (size_t i = 0; i < command.values.size(); ++i) {
                newRow[tableOpt->getColumns()[i]] = command.values[i];
            }
            if (insertInto(command.tableName, newRow)) {
                std::cout << "Fila insertada.\n";
            } else {
                std::cout << "Error al insertar la fila.\n";
            }
            break;
        }
        case CommandType::SELECT: {
            auto tableOpt = selectFrom(command.tableName);
            if (!tableOpt) {
                std::cout << "Error: La tabla '" << command.tableName << "' no existe.\n";
                return;
            }

            const auto& table = *tableOpt;
            std::vector<std::string> colsToPrint;

            if (command.columns.size() == 1 && command.columns[0] == "*") {
                colsToPrint = table.getColumns();
            } else {
                colsToPrint = command.columns;
            }

            // Imprimir cabecera
            for (const auto& col : colsToPrint) {
                std::cout << col << "\t";
            }
            std::cout << "\n---------------------------------\n";

            // Imprimir filas
            for (const auto& row : table.getRows()) {
                bool printRow = true;
                if (command.whereClause) {
                    const auto& wc = *command.whereClause;
                    auto it = row.find(wc.column);
                    if (it == row.end() || it->second != wc.value) {
                        printRow = false;
                    }
                }

                if (printRow) {
                    for (const auto& col : colsToPrint) {
                        auto it = row.find(col);
                        if (it != row.end()) {
                            std::cout << it->second << "\t";
                        }
                    }
                    std::cout << "\n";
                }
            }
            break;
        }
        case CommandType::UNRECOGNIZED:
            std::cout << "Error: Comando no reconocido o sintaxis incorrecta.\n";
            break;
    }
}

// Nueva implementación de guardado
void Database::save()
{
    std::ofstream db_file(db_name);
    if (!db_file.is_open()) {
        // Opcional: Manejar error si no se puede abrir el archivo.
        return;
    }

    for (const auto& pair : tables) {
        const auto& tableName = pair.first;
        const auto& table = pair.second;

        db_file << "[TABLE:" << tableName << "]\n";

        // Escribir cabecera (columnas)
        const auto& columns = table.getColumns();
        for (size_t i = 0; i < columns.size(); ++i) {
            db_file << columns[i] << (i == columns.size() - 1 ? "" : ",");
        }
        db_file << "\n";

        // Escribir filas
        for (const auto& row : table.getRows()) {
            for (size_t i = 0; i < columns.size(); ++i) {
                const auto& colName = columns[i];
                // Usamos find para evitar excepción si la clave no existe en la fila
                auto it = row.find(colName);
                if (it != row.end()) {
                    db_file << it->second;
                }
                db_file << (i == columns.size() - 1 ? "" : ",");
            }
            db_file << "\n";
        }
        db_file << "[END_TABLE]\n";
    }
}

// Nueva implementación de carga
void Database::load()
{
    std::ifstream db_file(db_name);
    if (!db_file.is_open()) {
        return; // El archivo no existe, no hay nada que cargar.
    }

    std::string line;
    std::string currentTable;
    std::vector<std::string> currentColumns;

    while (std::getline(db_file, line)) {
        if (line.rfind("[TABLE:", 0) == 0) {
            currentTable = line.substr(7, line.size() - 8);
            // Escribir cabecera (columnas)
            if (std::getline(db_file, line)) {
                std::stringstream ss(line);
                std::string column;
                currentColumns.clear();
                while (std::getline(ss, column, ',')) {
                    currentColumns.push_back(column);
                }
                createTable(currentTable, currentColumns);
            }
        } else if (line != "[END_TABLE]" && !currentTable.empty()) {
            Row row;
            std::stringstream row_ss(line);
            std::string value;
            for (const auto& colName : currentColumns) {
                std::getline(row_ss, value, ',');
                row[colName] = value;
            }
            insertInto(currentTable, row);
        }
    }
}
