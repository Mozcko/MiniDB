#pragma once

#include <string>
#include "Command.hpp"
#include <unordered_map>
#include "Table.hpp" // Incluimos nuestra nueva clase Table

class Database
{
public:
  // El constructor ahora tomará el nombre del archivo de la BD
  explicit Database(const std::string &db_name);
  // El destructor se asegurará de guardar al final
  ~Database();

  bool createTable(const std::string& tableName, const std::vector<std::string>& columns);
  bool insertInto(const std::string& tableName, const Row& row);
  std::optional<Table> selectFrom(const std::string& tableName);
  void execute(const Command& command);

private:
  void load(); // Carga la BD desde el archivo
  void save(); // Guarda la BD en el archivo

  std::string db_name;
  std::unordered_map<std::string, Table> tables;
};
