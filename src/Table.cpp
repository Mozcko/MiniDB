#include "MiniDB/Table.hpp"

Table::Table(std::vector<Column> cols) : columns(std::move(cols)) {}

bool Table::insert(const Row& row)
{
    rows.push_back(row);
    return true;
}

const std::vector<Row>& Table::getRows() const
{
    return rows;
}

const std::vector<Column>& Table::getColumns() const
{
    return columns;
}