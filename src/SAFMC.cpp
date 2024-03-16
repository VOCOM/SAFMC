/*****************************************************************//**
 * \file   SAFMC.cpp
 * \brief
 *
 * \author Muhammad Syamim
 * \date   12/02/2024
 *********************************************************************/

#pragma once

#include <DroneComputer.hpp>
#include <Utility.hpp>

#include <mavsdk/plugins/manual_control/manual_control.h>
#include <mavsdk/plugins/mission/mission.h>
#include <mavsdk/plugins/mocap/mocap.h>
#include <mavsdk/plugins/telemetry/telemetry.h>

#include <MissionPlanner.hpp>
#include <cstdlib>
#include <iostream>
#include <thread>

#define Wait(x) this_thread::sleep_for(x)

using namespace mavsdk;
using namespace chrono_literals;

typedef mavsdk::Telemetry::Position Position;
typedef mavsdk::Mission::MissionItem::VehicleAction VehicleAction;

void PositionStatus(Position position) {
  std::cout << "Altitude: " << position.relative_altitude_m << "m\n";
}
void MissionStatus(Mission::Result result) {
  if (result != Mission::Result::Success) {

  }
}

void AutoLoad() {
  for (unsigned char i {}; i < 5; i++) {
    std::cout << "Waiting for auto load...\n";
    this_thread::sleep_for(1s);
  }
}
void ManualLoad() {
  // TODO: Replace with manual trigger
  for (unsigned char i {}; i < 5; i++) {
    std::cout << "Waiting for manual load...\n";
    this_thread::sleep_for(1s);
  }
}
void AlignAruco(DroneComputer& drone) {
  // Load Velocities
  double x {}, y {}, z {};

  // Success Condition
  bool inRange {};

  double tolerance { 0.5 };

  while (!inRange) {
    // Get Vector
    drone.Track(x, y, z);

    Wait(100ms);

    // Check for tolerance
    if (x < tolerance && y < tolerance && z < tolerance) inRange = true;
  }

  // Halt
  drone.SetVelocity(0, 0, 0, 0);
}
void ReleasePayload() {
  std::cout << "Releasing payload!\n";
  this_thread::sleep_for(5s);
}

/**
 * @brief Main Control Loop
 * @return
*/
int main() {
  // Load Configurations
  Config config;
  if (!LoadConfig(config, "config.txt")) exit(EXIT_FAILURE);

  // Initialise Leader
  DroneComputer leader(config);

  // Missing System Guard
  if (!leader.Ready())exit(EXIT_FAILURE);

  // Attach Callbacks
  leader.AttachTelemetryCallback(PositionStatus);

  while (!leader.telemetry->health_all_ok()) {
    std::cout << "Waiting for system to be ready\n";
    Wait(1s);
  }

#ifdef MANUAL
  ManualControl manual(leader.system.value());

  manual.set_manual_control_input(0.f, 0.f, 0.5f, 0.f);
  manual.start_position_control();

  cout << "Arming!!!\n";
  leader.action->arm();

  cout << "Taking off!!!\n";
  for (int i {}; i < 250; i++) {
    manual.set_manual_control_input(0.f, 0.f, 1.f, 0.f);
    Wait(100ms);
  }
  //leader.action->takeoff();

  Wait(5s);

  cout << "Disarming!!!\n";
  leader.action->disarm();


  //cout << "Taking Off\n";
  // Takeoff [5s]
  //leader.Takeoff();

  //cout << "Starting Offboard Control\n";
  // Start Manual Control
  //leader.StartOffboard();

  // Find 1st Aruco [Loading point]
  // #TODO: INSERT ARUCO CODE HERE

  // Manual Load
  // #TODO: INSERT MANUAL LOAD TRIGGER CODE HERE

  // Fly to 1st Drop point [Dead Reckoning]
  //leader.SetVelocity(0, 0, 0, 0);

  // Find 2nd Aruco [Drop point]
  // #TODO: INSERT ARUCO CODE HERE

  // Drop Load
  // #TODO: INSERT DROP TRIGGER CODE HERE

  //cout << "Stopping Offboard Control\n";
  // Stop Manual Control
  //leader.StopOffboard();

  //cout << "Landing\n";
  // Land
  //leader.Land();
#endif

#define AUTO
#ifdef AUTO
  leader.Takeoff();

  // Initialise Mission Planner
  MissionPlanner planner;
  Position pickup, dropoff;

  // First mission (Manual Loading)
  leader.SetState(Pickup1);
  pickup = { 0,0,0,0 };
  planner.AddPoint(pickup, 1, VehicleAction::Takeoff);
  leader.LoadMissionPlan(planner.GetPlan());
  planner.Clear();

  // Go to First Loading Point #Blocking
  leader.ExecutePlan();

  // Reached Loading Point
  ManualLoad();

  // Deliver first parcel
  leader.SetState(Target1);
  dropoff = { 1,1,1,1 };
  planner.AddPoint(dropoff, 1);
  leader.LoadMissionPlan(planner.GetPlan());
  planner.Clear();

  // Go to First Drop Point #Blocking
  leader.ExecutePlan();

  // Reached Drop Point
  //AlignAruco();
  ReleasePayload();

  // Second mission
  leader.SetState(Pickup2);
  pickup = { 2,2,2,2 };
  planner.AddPoint(pickup, 1);
  leader.LoadMissionPlan(planner.GetPlan());
  planner.Clear();

  // Go to Pickup
  leader.ExecutePlan();

  // Reached Loading Point
  AutoLoad();

  // Deliver parcel
  leader.SetState(Target2);
  dropoff = { 3,3,3,3 };
  planner.AddPoint(dropoff, 1);
  leader.LoadMissionPlan(planner.GetPlan());
  planner.Clear();

  // Go to Drop Point #Blocking
  leader.ExecutePlan();

  // Land
  leader.Land();
#endif // AUTO

  return 0;
}

