#pragma once

#include "Command.hpp"
#include <string>

class Parser {
public:
    Command parse(const std::string& query);

private:
    Command parseCreate(std::vector<std::string>& tokens);
    Command parseInsert(std::vector<std::string>& tokens);
    Command parseSelect(std::vector<std::string>& tokens);
    Command parseDelete(std::vector<std::string>& tokens);
    Command parseUpdate(std::vector<std::string>& tokens);
};