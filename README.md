# UES
Interconnected embedded systems (via internet or not) - IoT projects

NB-IoT project - Watermelon Guard System (WGS)

  Autonomous system for monitoring environment parameters and controling parameters of interest with a goal of providing best conditions for watermelon growth in the field.
  Implemented using Atmel SAM L21 microcontroller and Quectel BC68 NB-IoT module for communication with CLOUD.
  Microcontroller mostly sleeps, and wakes up every hour to check environment temperature, humidity and ambiental light.
  Temperature and humidity is measured with SHTC3 sensor and acquired via I2C bus on address 0x70.
  Ambiental light is measured with BH1750FVI sensor and acquired via I2C bus on address 0x23.
  If humidity is too high servo SG90 opens the hatch and controller stays awake until it drops after which it closes the hatch and goes to sleep.
  If temperature is too low, controller closes the hatch, turns on the heater and waits until it rises after which it goes to sleep.
  Every time before goint to sleep message is uplink message is sent via NB-IoT network to server which gathers data, put it into SQL database table, and draws graph on website for monitoring.

  ![image](https://github.com/user-attachments/assets/85a25d31-28c0-4a0e-b4fa-9bf8f25c043d)

