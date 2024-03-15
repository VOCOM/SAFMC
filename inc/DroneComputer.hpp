/*****************************************************************//**
 * \file   DroneComputer.hpp
 * \brief
 *
 * \author Muhammad Syamim
 * \date   11/02/2024
 *********************************************************************/

#pragma once

#include <Utility.hpp>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/manual_control/manual_control.h>
#include <mavsdk/plugins/mission/mission.h>
#include <mavsdk/plugins/offboard/offboard.h>
#include <mavsdk/plugins/telemetry/telemetry.h>

#include <bitset>
#include <iostream>
#include <map>
#include <thread>

using namespace std;
using namespace mavsdk;
using namespace chrono_literals;

class DroneComputer {
public:
  typedef unsigned char const bit;
  static bit maxBits = 0xFF;
  static bit ReadyBit = 0x00;
  static bit ConnectionErrorBit = 0x01;
  static bit SystemErrorBit = 0x02;
  static bit RateErrorBit = 0x03;
  static bit MissionCompleteBit = 0x04;
  using BitRegister = bitset<maxBits>;

  DroneComputer() = delete;
  DroneComputer(Config params);
  ~DroneComputer();

  // Status
  bool Ready() const { return bitRegister[ReadyBit]; }

  // Utility
  void SetState(State newState);
  void LoadMissionPlan(Mission::MissionPlan missionPlan);
  void AttachTelemetryCallback(void(*callback)(Telemetry::Position position));
  void AttachMissionCallback(void(*callback)(Mission::Result result));

  // Flight Controls
  void Takeoff();
  void Land();
  void ExecutePlan();

  void StartOffboard();
  void Track(float x, float y, float z);
  void SetVelocity(float forward, float right, float down, float yaw);
  void StopOffboard();

private:
  void Reset();
  bool Init();

  float velocity = 1;

  State state;

  double rate;
  double timeout;
  string connection;
  BitRegister bitRegister;
  Mavsdk::Configuration config;

  unique_ptr<Action> action;
  unique_ptr<Mavsdk> mavsdk;
  unique_ptr<Telemetry> telemetry;
  unique_ptr<Mission> mission;
  unique_ptr<Offboard> offboard;

  void(*missionCallback)(Mission::Result);

  optional<shared_ptr<mavsdk::System>> system;
};

