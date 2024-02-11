/*****************************************************************//**
 * \file   Utility.cpp
 * \brief
 *
 * \author Muhammad Syamim
 * \date   11/02/2024
 *********************************************************************/

#include <Utility.hpp>

bool LoadConfig(Config& config, string filename) {
  fstream f(filename);
  if (!f.is_open()) return false;

  string line;
  while (!f.eof()) {
    getline(f, line);
    if (line.empty()) continue;
    string param(line.begin(), line.begin() + line.find_first_of(':'));
    string value(line.begin() + line.find_first_of('"') + 1, line.begin() + line.find_last_of('"'));
    transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {return std::tolower(c); });
    config[param] = value;
  }

  f.close();
  return true;
}