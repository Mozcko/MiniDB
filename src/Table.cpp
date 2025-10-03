#include "MiniDB/Table.hpp"
#include <algorithm>

Table::Table(std::vector<Column> cols) : columns(std::move(cols)) {}

bool Table::insert(const Row& row)
{
    rows.push_back(row);
    return true;
}

int Table::deleteRows(std::function<bool(const Row&)> condition)
{
    auto original_size = rows.size();
    auto it = std::remove_if(rows.begin(), rows.end(), condition);
    rows.erase(it, rows.end());
    return original_size - rows.size();
}

int Table::updateRows(std::function<bool(const Row&)> condition, std::function<void(Row&)> updateAction)
{
    int updated_count = 0;
    for (auto& row : rows) {
        if (condition(row)) {
            updateAction(row);
            updated_count++;
        }
    }
    return updated_count;
}

const std::vector<Row>& Table::getRows() const
{
    return rows;
}

const std::vector<Column>& Table::getColumns() const
{
    return columns;
}