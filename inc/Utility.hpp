/*****************************************************************//**
 * \file   Utility.hpp
 * \brief
 *
 * \author Muhammad Syamim
 * \date   11/02/2024
 *********************************************************************/

#pragma once

#include <algorithm>
#include <fstream>
#include <map>
#include <string>

using namespace std;

using Config = map<string, string>;

bool LoadConfig(Config& config, string filename);