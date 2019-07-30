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

M5StickC @ https://m5stack.com/collections/m5-core/products/stick-c <br/>
<img src="https://github.com/DarknessFX/FXNode_Weather/blob/master/.git_img/M5Stick-C.jpg" width="250"/>

### Features

- Display presents current Clock (10secs) and Weather (20secs).
- Every 10 minutes the M5StickC updates the Weather API data.
- Click Button A (M5) to manually force the Weather API refresh.

## Getting Started

### Prerequisites

- Have a M5StickC (or adapt the code to any ESP32 + LCD).
- Have M5StickC / ESP32 development environment. <br/> (example: ArduinoIDE + ESP32 @ Board Manager + ESP32 Pico @ Libraries Manager).

### Requisites

Before upload the code, change the following code lines :
```
String weather_cityID = "";        // Your OpenWeather CityID. 
String weather_APIKEY = "";        // Your OpenWeather API Key
...
WiFi_Data[0].ssid = "";            // Your WiFi AP SSID
WiFi_Data[0].pass = "";            // Your WiFi AP Password
```

### Installing

Compile and upload the project files to your M5StickC using your preferred development environment. 

## Versioning

v1.0 - Released.

## Acknowledgments

Thanks to :
- McOrts for the Nixie Tube Clock.<br/>
  Nixie tube clock simulated on ESP32 using the device M5Stick-C <br/>
  @ https://github.com/McOrts/M5StickC_Nixie_tube_Clock

- OpenWeather for the API Data and Weather Icons. <br/>
  @ https://openweathermap.org/weather-conditions

## License

@Copyleft all wrongs reserved. <br/><br/>
DarknessFX @ <a href="https://dfx.lv" target="_blank">https://dfx.lv</a> | Twitter: <a href="https://twitter.com/DrkFX" target="_blank">@DrkFX</a> <br/><br/>
https://github.com/DarknessFX/FXNode_Weather
