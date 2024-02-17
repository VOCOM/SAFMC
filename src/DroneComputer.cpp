/*****************************************************************//**
 * \file   DroneComputer.cpp
 * \brief
 *
 * \author Muhammad Syamim
 * \date   17/02/2024
 *********************************************************************/

#include <DroneComputer.hpp>
#include <Utility.hpp>

#include <mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/mission/mission.h>
#include <mavsdk/plugins/telemetry/telemetry.h>

#include <connection_result.h>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

DroneComputer::DroneComputer(Config params)
  : config(Mavsdk::ComponentType::CompanionComputer), state(Idle) {
  // Create MAVSDK Instance
  mavsdk = make_unique<Mavsdk>(config);

  // Load Configuration parameters
  if (params["Type"] == "serial")
    connection = params["Type"] + "://" + params["Host"] + params["Port"] + ':' + params["Baud"];
  else
    connection = params["Type"] + "://" + params["Host"] + ':' + params["Port"];
  timeout = stod(params["Timeout"]);
  rate = stod(params["Rate"]);

  // Initialize Systems
  while (!Init()) {
    cerr << "Initialization Failed!!!\n";
    this_thread::sleep_for(3s);
    cerr << "Retrying...\n";
  }
}

DroneComputer::~DroneComputer() {
  // Ensure Disarm
  action->disarm();
}

bool DroneComputer::Init() {
  // Establish Mavlink
  auto result = mavsdk->add_any_connection(connection);
  if (result != ConnectionResult::Success) {
    bitRegister[ConnectionErrorBit] = true;
    return false;
  }

  // Establish Connection to Autopilot
  system = mavsdk->first_autopilot(timeout);
  if (!system) {
    bitRegister[SystemErrorBit] = true;
    return false;
  }

  // Establish Telemetry Tracker
  telemetry = make_unique<Telemetry>(system.value());

  // Establish Action Handler
  action = make_unique<Action>(system.value());

  // Establish Mission Handler
  mission = make_unique<Mission>(system.value());

  // Set Update Rate
  auto set_rate_result = telemetry->set_rate_position(rate);
  if (set_rate_result != Telemetry::Result::Success) {
    bitRegister[RateErrorBit] = true;
    return false;
  }

  // Initialization Complete
  bitRegister[ReadyBit] = true;
  state = State::Ready;
  return true;
}

void DroneComputer::SetState(State newState) {
  state = newState;
}

void DroneComputer::LoadMissionPlan(Mission::MissionPlan missionPlan) {
  mission->upload_mission(missionPlan);
  bitRegister[MissionCompleteBit] = false;
}

void DroneComputer::AttachTelemetryCallback(void(*callback)(Telemetry::Position position)) {
  telemetry->subscribe_position(callback);
}

void DroneComputer::AttachMissionCallback(void(*callback)(Mission::Result result)) {
  missionCallback = callback;
}

void DroneComputer::Takeoff() {
  action->arm();
  action->takeoff();
}

void DroneComputer::Land() {
  action->land();
  while (telemetry->in_air());
}

void DroneComputer::ExecutePlan() {
  // Execute in Blocking mode
  auto result = mission->start_mission();
  if (result == Mission::Result::Success) bitRegister[MissionCompleteBit] = true;
}
