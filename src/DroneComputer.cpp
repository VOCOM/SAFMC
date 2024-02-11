#include <DroneComputer.hpp>

DroneComputer::DroneComputer(Config params) :
  config(Mavsdk::ComponentType::CompanionComputer) {
  // Create MAVSDK Instance
  mavsdk = make_unique<Mavsdk>(config);
  // Load Configuration parameters
  connection = params["Type"] + "://" + params["Host"] + ':' + params["Port"];
  timeout = stod(params["Timeout"]);
  rate = stod(params["Rate"]);
  // Initialize Systems
  Init();
}

DroneComputer::~DroneComputer() {
  // Ensure Disarm
  action->disarm();
}

bool DroneComputer::Init() {
  // Establish Mavlink
  auto result = mavsdk->add_any_connection(connection);
  if (result != ConnectionResult::Success) {
    bitRegister[ConnectionBit] = true;
    return false;
  }
  // Establish Connection to Autopilot
  system = mavsdk->first_autopilot(timeout);
  if (!system) {
    bitRegister[SystemBit] = true;
    return false;
  }
  // Establish Telemetry
  telemetry = make_unique<Telemetry>(system.value());
  // Establish Action package
  action = make_unique<Action>(system.value());
  // Set Update Rate
  auto set_rate_result = telemetry->set_rate_position(rate);
  if (set_rate_result != Telemetry::Result::Success) {
    bitRegister[RateBit] = true;
    return false;
  }
  // Initialization Complete
  bitRegister[ReadyBit] = true;
  return true;
}
