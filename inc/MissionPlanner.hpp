/*****************************************************************//**
 * \file   MissionPlanner.hpp
 * \brief
 *
 * \author Muhammad Syamim
 * \date   17/02/2024
 *********************************************************************/

#pragma once

#include <mavsdk/plugins/mission/mission.h>
#include <mavsdk/plugins/telemetry/telemetry.h>

class MissionPlanner {
public:
  typedef mavsdk::Telemetry::Position Position;
  typedef mavsdk::Mission::MissionPlan MissionPlan;
  typedef mavsdk::Mission::MissionItem MissionItem;
  typedef mavsdk::Mission::MissionItem::VehicleAction VehicleAction;

  MissionPlanner();

  void AddPoint(Position position, float speed, VehicleAction action = VehicleAction::None, float radius = 1, bool flyThrough = false);
  MissionPlan GetPlan();
  void Clear();

  MissionPlan plan;
private:
};
