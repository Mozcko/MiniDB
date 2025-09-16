#pragma once

#include <string>
#include <vector>
#include <variant>

enum class CommandType {
    CREATE_TABLE,
    INSERT,
    SELECT,
    UNRECOGNIZED
};

struct Command {
    CommandType type = CommandType::UNRECOGNIZED;
    std::string tableName;
    std::vector<std::string> columns;
    std::vector<std::string> values;
};