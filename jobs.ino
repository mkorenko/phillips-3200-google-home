#define MACHINE_BUTTON_PRESS_INTERVAL_MS 600

unsigned long _jobs_last_action_at = 0;

inline void jobs_schedule_next_action() {
  _jobs_last_action_at = millis();
}

inline void jobs_loop() {
  if (job_repeat == 0) { return; }

  if (machine.current_machine_status.rfind("ready", 0) != 0 &&
      machine.current_machine_status != "selected") {
    return;
  }

  if (millis() - _jobs_last_action_at < MACHINE_BUTTON_PRESS_INTERVAL_MS) {
    return;
  }
  _jobs_last_action_at = millis();

  if (machine.current_brew != job_brew) {
    if (job_brew == "2x_espresso") {
      return machine.send_cmd("espresso");
    }
    if (job_brew == "2x_coffee") {
      return machine.send_cmd("coffee");
    }
    if (job_brew == "2x_americano") {
      return machine.send_cmd("americano");
    }
    return machine.send_cmd(job_brew);
  }

  if (machine.current_strength_level != job_strength_level) {
    return machine.send_cmd("coffee_strength_level");
  }
  if (machine.current_water_level != job_water_level) {
    return machine.send_cmd("coffee_water_level");
  }
  if (machine.current_milk_level != job_milk_level) {
    return machine.send_cmd("coffee_milk_level");
  }

  job_repeat--;
  machine.send_cmd("start_pause");
}

inline std::string jobs_schedule_job(
    const uint8_t &repeat,
    const std::string &brew,
    const uint8_t &strength_level,
    const uint8_t &water_level,
    const uint8_t &milk_level
) {
  if (job_repeat > 0) {
    return "deviceBusy";
  }

  if (!machine.current_power_status) {
    return "deviceTurnedOff";
  }

  if (machine.current_machine_status.rfind("ready", 0) != 0 &&
      machine.current_machine_status != "selected") {
    return "deviceBusy";
  }

  if (repeat <= 0) {
    return "valueOutOfRange";
  }

  if (repeat > 3) {
    return "amountAboveLimit";
  }

  if (brew != "espresso" &&
      brew != "2x_espresso" &&
      brew != "coffee" &&
      brew != "2x_coffee" &&
      brew != "americano" &&
      brew != "2x_americano" &&
      brew != "cappuccino" &&
      brew != "latte" &&
      brew != "hot_water") {
    return "unknownFoodPreset";
  }

  if (strength_level > 3) {
    return "valueOutOfRange";
  }

  if (water_level == 0 || water_level > 3) {
    return "valueOutOfRange";
  }

  if (milk_level > 3) {
    return "valueOutOfRange";
  }

  if (brew != "cappuccino" &&
      brew != "latte" &&
      milk_level != 0) {
    return "valueOutOfRange";
  }

  if (brew == "hot_water" &&
      strength_level != 0) {
    return "valueOutOfRange";
  }

  job_repeat = repeat;
  job_brew = brew;
  job_strength_level = strength_level;
  job_water_level = water_level;
  job_milk_level = milk_level;

  jobs_schedule_next_action();

  return "";
}
