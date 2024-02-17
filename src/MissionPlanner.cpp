/*****************************************************************//**
 * \file   MissionPlanner.cpp
 * \brief
 *
 * \author Muhammad Syamim
 * \date   17/02/2024
 *********************************************************************/

#include <MissionPlanner.hpp>

MissionPlanner::MissionPlanner() { }

void MissionPlanner::AddPoint(Position position, float speed, VehicleAction action, float radius, bool flyThrough) {
  MissionItem item;

  item.longitude_deg = position.longitude_deg;
  item.latitude_deg = position.latitude_deg;
  item.relative_altitude_m = position.relative_altitude_m;
  item.speed_m_s = speed;
  item.acceptance_radius_m = radius;
  item.is_fly_through = flyThrough;
  item.vehicle_action = action;

  plan.mission_items.push_back(item);
}

MissionPlanner::MissionPlan MissionPlanner::GetPlan() {
  return plan;
}

void MissionPlanner::Clear() {
  plan.mission_items.clear();
}
