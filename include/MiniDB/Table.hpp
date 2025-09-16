#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>

// Representa una fila como un mapa de nombre de columna a valor
using Row = std::unordered_map<std::string, std::string>;

class Table
{
public:
    Table() = default;
    explicit Table(std::vector<std::string> columns);

    void insert(const Row& row);
    const std::vector<Row>& getRows() const;
    const std::vector<std::string>& getColumns() const;

private:
    std::vector<std::string> columns;
    std::vector<Row> rows;
};