/*****************************************************************//**
 * \file   DroneComputer.hpp
 * \brief
 *
 * \author Muhammad Syamim
 * \date   11/02/2024
 *********************************************************************/

#pragma once

#include <Utility.hpp>
#include <bitset>
#include <iostream>
#include <map>
#include <mavsdk.h>
#include <plugins/action/action.h>
#include <plugins/telemetry/telemetry.h>
#include <thread>

using namespace std;
using namespace mavsdk;
using namespace chrono_literals;

constexpr size_t maxBits = 0xFF;
constexpr unsigned char ReadyBit = 0x00;
constexpr unsigned char ConnectionBit = 0x01;
constexpr unsigned char SystemBit = 0x02;
constexpr unsigned char RateBit = 0x03;

using BitRegister = bitset<maxBits>;

class DroneComputer {
public:
  DroneComputer() = delete;
  DroneComputer(Config params);
  ~DroneComputer();

  bool Ready() const { return bitRegister[ReadyBit]; }

  unique_ptr<Mavsdk> mavsdk;
  unique_ptr<Action> action;
  unique_ptr<Telemetry> telemetry;

private:
  void Reset();
  bool Init();

  BitRegister bitRegister;

  double rate;
  double timeout;
  string connection;

  Mavsdk::Configuration config;

  optional<shared_ptr<mavsdk::System>> system;
};

