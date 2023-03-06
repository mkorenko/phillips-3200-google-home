#if defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

#include <ArduinoOTA.h>
#include <string>

#include "src/esp-firebase-gh/esp-firebase-gh.h"
#include "src/esp-phillips-3200/esp-phillips-3200.h"

#include "credentials.h"

Phillips3200 machine;

/* jobs global variables */
uint8_t job_repeat = 0;
std::string job_brew = "none";
uint8_t job_strength_level = 0;
uint8_t job_water_level = 0;
uint8_t job_milk_level = 0;

void on_machine_state_changed() {
  jobs_schedule_next_action();
  gh_report_device_state();
}

void setup() {
  delay(500);
  machine.setup(on_machine_state_changed);
  wifi_setup();
  gh_setup();
}

void loop() {
  machine.loop();
  gh_loop();
  jobs_loop();
  wifi_loop();
}
