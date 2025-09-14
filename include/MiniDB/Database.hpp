#pragma once

#include <string>
#include <vector>
#include <utility>

class Database{
  public: 
    void set(const std::string& key, const std::string& value);
    std::string get(const std::string& key);

  private:
    std::vector<std::pair<std::string, std::string>> storage; 
};

