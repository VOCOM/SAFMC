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
#include <mavsdk/plugins/offboard/offboard.h>
#include <mavsdk/plugins/telemetry/telemetry.h>

#include <connection_result.h>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

bool ConfigContains(Config param, std::string key) {
  return param.find(key) == param.end();
}

DroneComputer::DroneComputer(Config params)
  : config(Mavsdk::ComponentType::CompanionComputer), state(Idle) {
  // Create MAVSDK Instance
  mavsdk = make_unique<Mavsdk>(config);

  // Load Configuration parameters
  if (ConfigContains(params, "Type")) {
    if (params["Type"] == "serial")
      connection = params["Type"] + "://" + params["Host"] + params["Port"] + ':' + params["Baud"];
    else
      connection = params["Type"] + "://" + params["Host"] + ':' + params["Port"];
  }
  if (ConfigContains(params, "Timeout")) {
    timeout = stod(params["Timeout"]);
} 
  if (ConfigContains(params, "Rate")) {
    rate = stod(params["Rate"]);
  }
  if (ConfigContains(params, "Tries")) {
    tries = stoi(params["Tries"]);
  }

  // Initialize Systems
  int i = 1;
  while (!Init() && i < tries) {
    cerr << "Initialization Failed!!!\n";
    this_thread::sleep_for(3s);
    cerr << "Retrying... Tried: " << i << " times.\n";
    i++;
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
  //mission = make_unique<Mission>(system.value());

  // Establish Offboard Handler
  offboard = make_unique<Offboard>(system.value());

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
  this_thread::sleep_for(5s);
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

void DroneComputer::StartOffboard() {
  Offboard::VelocityBodyYawspeed stay {};
  offboard->set_velocity_body(stay);
  offboard->start();
}
void DroneComputer::Track(float x, float y, float depth) {
  Offboard::VelocityBodyYawspeed to_target {};
  to_target.forward_m_s = depth;
  to_target.right_m_s = x;
  to_target.down_m_s = y;
  offboard->set_velocity_body(to_target);
}
void DroneComputer::SetVelocity(float forward, float right, float down, float yaw) {
  Offboard::VelocityBodyYawspeed velocity {};
  velocity.forward_m_s = forward;
  velocity.right_m_s = right;
  velocity.down_m_s = down;
  velocity.yawspeed_deg_s = yaw;
  offboard->set_velocity_body(velocity);
}
void DroneComputer::StopOffboard() {
  offboard->stop();
}
