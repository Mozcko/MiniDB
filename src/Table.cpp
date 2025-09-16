#include "MiniDB/Table.hpp"

Table::Table(std::vector<std::string> cols) : columns(std::move(cols)) {}

void Table::insert(const Row& row)
{
    // Opcional: Se podría añadir validación para asegurar que la fila
    // contiene todas las columnas esperadas.
    rows.push_back(row);
}

const std::vector<Row>& Table::getRows() const
{
    return rows;
}

const std::vector<std::string>& Table::getColumns() const
{
    return columns;
}