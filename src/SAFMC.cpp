/*****************************************************************//**
 * \file   SAFMC.cpp
 * \brief
 *
 * \author Muhammad Syamim
 * \date   12/02/2024
 *********************************************************************/

#include <DroneComputer.hpp>
#include <Utility.hpp>
#include <chrono>
#include <iostream>
#include <thread>

using namespace mavsdk;

int main() {
  Config config;
  if (!LoadConfig(config, "config.txt")) {
    std::cerr << "Failed to load configuration file!!!\n";
    return 1;
  }

  DroneComputer master(config);

  // Pre-flight checks
  master.telemetry->subscribe_position([](Telemetry::Position position) {
    std::cout << "Altitude: " << position.relative_altitude_m << "m\n";
  });

  // Cleared for takeoff
  master.action->arm();
  master.action->takeoff();

  // Payload 1 [Manual]

  // Target 1

  this_thread::sleep_for(chrono::seconds(10));


  master.action->land();

  while (master.telemetry->in_air());

  return 0;
}


