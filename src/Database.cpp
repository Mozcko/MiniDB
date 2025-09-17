#include "MiniDB/Database.hpp"
#include <fstream>
#include <iostream>
#include <variant>
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

bool Database::createTable(const std::string& tableName, const std::vector<Column>& columns)
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
    std::string line;
    std::string currentCommand;

    while (std::getline(scriptStream, line)) {
        // Ignorar comentarios
        size_t comment_pos = line.find("--");
        if (comment_pos != std::string::npos) {
            line = line.substr(0, comment_pos);
        }

        // Ignorar líneas vacías o solo con espacios
        if (line.find_first_not_of(" \t\n\r") == std::string::npos) {
            continue;
        }

        currentCommand += line + " ";

        // Si la línea acumulada termina en ';', es un comando completo
        size_t semi_pos = currentCommand.find(';');
        if (semi_pos != std::string::npos) {
            std::string commandToExecute = currentCommand.substr(0, semi_pos);
            Command command = parser.parse(commandToExecute);
            execute(command);
            currentCommand.clear();
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
                const auto& col = tableOpt->getColumns()[i];
                const auto& valStr = command.values[i];
                try {
                    if (col.type == DataType::INTEGER) {
                        newRow[col.name] = std::stoi(valStr);
                    } else { // TEXT
                        newRow[col.name] = valStr;
                    }
                } catch (const std::invalid_argument& e) {
                    std::cout << "Error: Valor '" << valStr << "' no es válido para la columna '" << col.name << "' de tipo INTEGER.\n";
                    return;
                } catch (const std::out_of_range& e) {
                    std::cout << "Error: Valor '" << valStr << "' fuera de rango para tipo INTEGER.\n";
                    return;
                }
            }

            // Re-chequeamos la tabla porque insertInto podría fallar por otras razones
            auto it = tables.find(command.tableName);
            if (it != tables.end() && it->second.insert(newRow)) {
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

            if (command.columnNames.size() == 1 && command.columnNames[0] == "*") {
                for(const auto& col : table.getColumns()) colsToPrint.push_back(col.name);
            } else {
                colsToPrint = command.columnNames;
            }

            // Imprimir cabecera
            for (const auto& col : colsToPrint) { // Ahora col es string
                std::cout << col << "\t";
            }
            std::cout << "\n---------------------------------\n";

            // Imprimir filas
            for (const auto& row : table.getRows()) {
                bool printRow = true;
                if (command.whereClause) {
                    const auto& wc = *command.whereClause;
                    auto it = row.find(wc.column);
                    if (it == row.end()) {
                        printRow = false;
                    } else {
                        // Comparamos string con variant
                        std::string cellValueStr;
                        if (std::holds_alternative<int>(it->second)) {
                            cellValueStr = std::to_string(std::get<int>(it->second));
                        } else {
                            cellValueStr = std::get<std::string>(it->second);
                        }
                        if (cellValueStr != wc.value) {
                            printRow = false;
                        }
                    }
                }

                if (printRow) {
                    for (const auto& colName : colsToPrint) {
                        auto it = row.find(colName);
                        if (it != row.end()) {
                            // Imprimir el valor del variant
                            std::visit([](auto&& arg){
                                std::cout << arg << "\t";
                            }, it->second);
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
        for (size_t i = 0; i < columns.size(); ++i) { // id INTEGER,nombre TEXT
            db_file << columns[i].name << " " << (columns[i].type == DataType::INTEGER ? "INTEGER" : "TEXT") << (i == columns.size() - 1 ? "" : ",");
        }
        db_file << "\n";

        // Escribir filas
        for (const auto& row : table.getRows()) {
            for (size_t i = 0; i < columns.size(); ++i) {
                const auto& colName = columns[i];
                auto it = row.find(colName.name);
                if (it != row.end()) {
                    std::visit([&db_file](auto&& arg){
                        db_file << arg;
                    }, it->second);
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
    std::vector<Column> currentColumns;

    while (std::getline(db_file, line)) {
        if (line.rfind("[TABLE:", 0) == 0) {
            currentTable = line.substr(7, line.size() - 8);
            // Leer cabecera (columnas con tipos)
            if (std::getline(db_file, line)) {
                std::stringstream ss(line);
                std::string columnDef;
                currentColumns.clear();
                while (std::getline(ss, columnDef, ',')) {
                    std::stringstream def_ss(columnDef);
                    std::string name, typeStr;
                    def_ss >> name >> typeStr;
                    if (typeStr == "INTEGER") {
                        currentColumns.push_back({name, DataType::INTEGER});
                    } else {
                        currentColumns.push_back({name, DataType::TEXT});
                    }
                }
                createTable(currentTable, currentColumns);
            }
        } else if (line == "[END_TABLE]") {
            currentTable.clear();
            currentColumns.clear();
        } else if (!currentTable.empty()) {
            Row row;
            std::stringstream row_ss(line);
            std::string value;
            for (const auto& col : currentColumns) {
                std::getline(row_ss, value, ',');
                if (col.type == DataType::INTEGER) {
                    try {
                        row[col.name] = std::stoi(value);
                    } catch(...) { /* Ignorar valor inválido en la carga */ }
                } else {
                    row[col.name] = value;
                }
            }
            insertInto(currentTable, row);
        }
    }
}
