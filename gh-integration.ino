FirebaseEspGh firebase_esp_gh;

void gh_on_device_state_request(
    FirebaseJson *state,
    FirebaseJson *gh_state,
    FirebaseJson *gh_notifications
) {
  // custom state
  state->add("local_ip", machine_local_ip);
  state->add("power_status", machine.current_power_status);
  state->add("machine_status", machine.current_machine_status);
  state->add("brew", machine.current_brew);
  state->add("strength_level", machine.current_strength_level);
  state->add("grinder_type", machine.current_grinder_type);
  state->add("water_level", machine.current_water_level);
  state->add("milk_level", machine.current_milk_level);
  state->add("job_repeat", job_repeat);
  state->set("timestamp/.sv", "timestamp");

  // gh state
  gh_state->add("on", machine.current_power_status);
  gh_state->add("currentCookingMode",
    machine.current_machine_status == "brewing" ? "BREW" : "NONE");
  gh_state->add("currentFoodPreset",
    machine.current_brew == "none" ? "NONE" : machine.current_brew);
  gh_state->add("currentFoodQuantity", job_repeat + 1);

  FirebaseJson current_run_cycle_en;
  current_run_cycle_en.add(
      "currentCycle",
      _gh_get_cycle_label(machine.current_machine_status)
  );
  current_run_cycle_en.add("lang", "en");
  FirebaseJsonArray current_run_cycle;
  current_run_cycle.add(current_run_cycle_en);
  gh_state->add("currentRunCycle", current_run_cycle);

  gh_state->add("currentFoodUnit", "CUPS");
  gh_state->add("currentTotalRemainingTime", 1);
  gh_state->add("currentCycleRemainingTime", 1);

  // gh notifications
  if (machine.current_machine_status == "error_no_water") {
    FirebaseJson notification;
    notification.add("priority", 0);
    notification.add("status", "FAILURE");
    notification.add("errorCode", "needsWater");
    gh_notifications->add("RunCycle", notification);
  } else if (machine.current_machine_status == "error_grounds_container") {
    FirebaseJson notification;
    notification.add("priority", 0);
    notification.add("status", "FAILURE");
    notification.add("errorCode", "binFull");
    gh_notifications->add("RunCycle", notification);
  }
}

void gh_on_command(
  FirebaseJson *result,
  std::string &command,
  FirebaseJson &params
) {
  if (command == "action.devices.commands.OnOff") {
    return _gh_on_onoff(result, params);
  }
  if (command == "action.devices.commands.Cook") {
    return _gh_on_cook(result, params);
  }

  result->add("error_code", "hardError");
}

inline void _gh_on_onoff(FirebaseJson *result, FirebaseJson &params) {
  FirebaseJsonData _on;
  params.get(_on, "on");
  if (!_on.success) {
    result->add("error_code", "hardError");
    return;
  }
  bool on = _on.to<bool>();

  if ((on && !machine.current_power_status) ||
      (!on && machine.current_power_status)) {

    machine.send_cmd(on ? "power_on" : "power_off");

    result->add("on", on);
    return;
  }

  result->add("error_code", on ? "alreadyOn" : "alreadyOff");
}

inline void _gh_on_cook(FirebaseJson *result, FirebaseJson &params) {
  FirebaseJsonData _start;
  params.get(_start, "start");
  if (!_start.success) {
    result->add("error_code", "hardError");
    return;
  }
  bool start = _start.to<bool>();
  if (!start) {
    result->add("error_code", "hardError");
    return;
  }

  FirebaseJsonData _food_preset;
  params.get(_food_preset, "foodPreset");
  if (!_food_preset.success) {
    result->add("error_code", "unknownFoodPreset");
    return;
  }
  std::string food_preset = _food_preset.to<std::string>();

  uint8_t quantity;
  FirebaseJsonData _quantity;
  params.get(_quantity, "quantity");
  if (_quantity.success) {
    quantity = _quantity.to<uint8_t>();
  } else {
    quantity = 1;
  }

  // hardcoded levels for drinks
  uint8_t strength_level = 1;
  uint8_t water_level = 1;
  uint8_t milk_level = 0;
  if (food_preset == "cappuccino" || food_preset == "latte") {
    strength_level = 2;
    water_level = 2;
    milk_level = 2;
  }
  if (food_preset == "hot_water") {
    strength_level = 0;
  }

  const std::string schedule_job_error = jobs_schedule_job(
      quantity,
      food_preset,
      strength_level,
      water_level,
      milk_level
  );

  if (!schedule_job_error.empty()) {
    result->add("error_code", schedule_job_error);
    return;
  }

  result->add("currentCookingMode", "BREW");
  result->add("currentFoodPreset", food_preset);
  result->add("currentFoodQuantity", quantity);
  result->add("currentFoodUnit", "CUPS");
}

inline std::string _gh_get_cycle_label(std::string &machine_status) {
  if (machine_status == "off") {
    return "Off";
  }
  if (machine_status == "error") {
    return "Error";
  }
  if (machine_status == "error_grounds_container") {
    return "Error: Grounds container is full";
  }
  if (machine_status == "error_no_water") {
    return "Error: No water";
  }
  if (machine_status == "selected") {
    return "Brew is selected";
  }
  if (machine_status == "heating") {
    return "Heating";
  }
  if (machine_status == "brewing") {
    return "Brewing";
  }
  if (machine_status == "ready") {
    return "Ready";
  }
  if (machine_status == "ready_aqua_clean") {
    return "Ready - check AquaClean";
  }
  return "Unknown";
}

inline void gh_report_device_state() {
  firebase_esp_gh.report_device_state();
}

inline void gh_setup() {
  firebase_esp_gh.begin(
      firebase_api_key,
      firebase_user_email,
      firebase_user_password,
      firebase_db_url,
      firebase_device_id,
      gh_on_command,
      gh_on_device_state_request
  );
}

inline void gh_loop() {
  firebase_esp_gh.loop();
}
