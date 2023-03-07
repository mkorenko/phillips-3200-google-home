## Phillips-3200 coffee machine Google Home integration

Phillips-3200 coffee machine Google Home integration using ESP32/ESP8266 microcontrollers and [firebase-google-home framework](https://github.com/mkorenko/esp-firebase-gh).

Based on:
- [esp-phillips-3200](https://github.com/mkorenko/esp-phillips-3200)
- [esp-firebase-gh](https://github.com/mkorenko/esp-firebase-gh)
- [firebase-google-home](https://github.com/mkorenko/firebase-google-home)


#### Features
- Turn the coffee machine on / off
- Display current coffee machine state (implemented as [RunCycle](https://developers.home.google.com/cloud-to-cloud/traits/runcycle))
- (Voice control) Make drinks: "Make {n} cups of {brew}"

#### Firebase backup
The backup file [philips-3200-default-rtdb-export.json](https://github.com/mkorenko/phillips-3200-google-home/blob/main/philips-3200-default-rtdb-export.json) contains full Firebase RTDB backup for the coffee machine, including the device config, [OnOff](https://developers.home.google.com/cloud-to-cloud/traits/onoff), [RunCycle](https://developers.home.google.com/cloud-to-cloud/traits/runcycle) and [Cook](https://developers.home.google.com/cloud-to-cloud/traits/cook) (w/ all 3200 supported brews) traits.
