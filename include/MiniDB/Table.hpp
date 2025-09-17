#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <variant>

enum class DataType {
    INTEGER,
    TEXT
};

struct Column {
    std::string name;
    DataType type;
};

// Representa una fila como un mapa de nombre de columna a valor
// Usamos std::variant para poder almacenar diferentes tipos de datos.
using CellValue = std::variant<int, std::string>;
using Row = std::unordered_map<std::string, CellValue>;

class Table
{
public:
    Table() = default;
    explicit Table(std::vector<Column> columns);

    bool insert(const Row& row);
    const std::vector<Row>& getRows() const;
    const std::vector<Column>& getColumns() const;

private:
    std::vector<Column> columns;
    std::vector<Row> rows;
};