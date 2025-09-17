#pragma once

#include <string>
#include <vector>
#include <optional>
#include <variant>

enum class CommandType {
    CREATE_TABLE,
    INSERT,
    SELECT,
    UNRECOGNIZED
};

struct WhereClause {
    std::string column;
    std::string value;
};

struct Command {
    CommandType type = CommandType::UNRECOGNIZED;
    std::string tableName;
    std::vector<std::string> columns;
    std::vector<std::string> values;
    std::optional<WhereClause> whereClause;
};