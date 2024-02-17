/*****************************************************************//**
 * \file   Utility.cpp
 * \brief
 *
 * \author Muhammad Syamim
 * \date   11/02/2024
 *********************************************************************/

#include <Utility.hpp>

#include <iosfwd>
#include <iostream>
#include <string>

bool LoadConfig(Config& config, std::string filename) {
  std::fstream f(filename);
  if (!f.is_open()) {
    std::cerr << "Failed to load configuration file!!!\n";
    return false;
  }

  std::string line;
  while (!f.eof()) {
    getline(f, line);
    if (line.empty()) continue;
    std::string param(line.begin(), line.begin() + line.find_first_of(':'));
    std::string value(line.begin() + line.find_first_of('"') + 1, line.begin() + line.find_last_of('"'));

    config[param] = value;
  }

  f.close();
  return true;
}