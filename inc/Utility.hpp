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

using Config = std::map<std::string, std::string>;

bool LoadConfig(Config& config, std::string filename);

typedef enum State {
  Uninitialized,
  Idle, Ready,
  Pickup1, Target1,
  Pickup2, Target2,
  Pickup3, Target3,
  Pickup4, Target4,
  Pickup5, Target5,
  Pickup6, Target6,
};