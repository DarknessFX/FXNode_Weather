     .----------------.  .----------------.  .----------------. 
    | .--------------. || .--------------. || .--------------. |
    | |  ________    | || |  _________   | || |  ____  ____  | |
    | | |_   ___ `.  | || | |_   ___  |  | || | |_  _||_  _| | |
    | |   | |   `. \ | || |   | |_  \_|  | || |   \ \  / /   | |
    | |   | |    | | | || |   |  _|      | || |    > `' <    | |
    | |  _| |___.' / | || |  _| |_       | || |  _/ /'`\ \_  | |
    | | |________.'  | || | |_____|      | || | |____||____| | |
    | |              | || |              | || |              | |
    | '--------------' || '--------------' || '--------------' |
     '----------------'  '----------------'  '----------------' 

           DarknessFX @ https://dfx.lv | Twitter: @DrkFX

# FXNode_Weather For M5StickC

Connect to your WiFi AP SSID List, gather data from OpenWeather and NTP, display cycles the current Weather and Time.
<img src="https://github.com/DarknessFX/FXNode_Weather/blob/master/.git_img/img1.jpg" width="200"/><img src="https://github.com/DarknessFX/FXNode_Weather/blob/master/.git_img/img2.jpg" width="200"/>

M5StickC @ https://m5stack.com/collections/m5-core/products/stick-c

## Getting Started

Before upload the code, change the following code lines :
```
String weather_cityID = "";        // Your OpenWeather CityID. 
String weather_APIKEY = "";        // Your OpenWeather API Key
...
WiFi_Data[0].ssid = "";            // Your WiFi AP SSID
WiFi_Data[0].pass = "";            // Your WiFi AP Password
```

### Prerequisites

- Have a M5StickC (or adapt the code to any ESP32 + LCD).
- Have M5StickC / ESP32 development environment. (example: ArduinoIDE + ESP32 @ Board Manager + ESP32 Pico @ Libraries Manager).

### Installing

Compile and upload the project files to your M5StickC using your preferred development environment. 

## Versioning
v1.0 - Released.

## Acknowledgments

Thanks to :
- McOrts for the Nixie Tube Clock.
  Nixie tube clock simulated on ESP32 using the device M5Stick-C 
  @ https://github.com/McOrts/M5StickC_Nixie_tube_Clock

- OpenWeather for the API Data and Weather Icons.
  @ https://openweathermap.org/weather-conditions

## License
@Copyleft all wrongs reserved.
