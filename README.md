# Sunrise Alarm Clock
Code for ESP32 DIY alarm clock that wakes you by simulating a sunrise

## Setup/Files
 - Rename `sunrise-alarm-clock.example` to `sunrise-alarm-clock.ino` and insert your Wi-Fi credentials
 - Using the Arduino IDE, `sunrise-alarm-clock.ino` is automatically recognized as the main file, but most of the code is in `server.ino`
 - Upload the code, the ESP32 starts blinking until it is connected to the Wi-Fi
 - Insert the IP of the ESP32 into your browser and set a time

## API Spec
```
  - GET /set
    Set a rising start time and duration.
    Params:
      in : In how many seconds the light should start rising
      for : Rising duration in seconds
      stay : How long (seconds) to stay on after reaching 100%
    Example: /set?in=28800&for=1800&stay=3600
    
  - GET /off
    Turn off the light.
```