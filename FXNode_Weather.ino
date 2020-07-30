#include <M5StickC.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <ArduinoJson.h>
#include "img_ico.c"
#include "img_clk.c"

#define TFT_ORANGE 0xFF8C
#define WiFi_ID_Size 1                // Total WiFi AP List to connect
#define Loop_Update 1000              // Time cycle for functions (except button click)
#define Screen_Update 10000           // Time cycle for screen display info
#define WebAPI_reload 1000 * 60 * 10  // 10 minutes reload

//Comodo Cert used at OpenWeather SSL.
const char* webcert = \ 
"-----BEGIN CERTIFICATE-----\n" \  
"MIIDSjCCAjKgAwIBAgIQRK+wgNajJ7qJMDmGLvhAazANBgkqhkiG9w0BAQUFADA/\n" \  
"MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n" \  
"DkRTVCBSb290IENBIFgzMB4XDTAwMDkzMDIxMTIxOVoXDTIxMDkzMDE0MDExNVow\n" \  
"PzEkMCIGA1UEChMbRGlnaXRhbCBTaWduYXR1cmUgVHJ1c3QgQ28uMRcwFQYDVQQD\n" \  
"Ew5EU1QgUm9vdCBDQSBYMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n" \  
"AN+v6ZdQCINXtMxiZfaQguzH0yxrMMpb7NnDfcdAwRgUi+DoM3ZJKuM/IUmTrE4O\n" \  
"rz5Iy2Xu/NMhD2XSKtkyj4zl93ewEnu1lcCJo6m67XMuegwGMoOifooUMM0RoOEq\n" \  
"OLl5CjH9UL2AZd+3UWODyOKIYepLYYHsUmu5ouJLGiifSKOeDNoJjj4XLh7dIN9b\n" \  
"xiqKqy69cK3FCxolkHRyxXtqqzTWMIn/5WgTe1QLyNau7Fqckh49ZLOMxt+/yUFw\n" \  
"7BZy1SbsOFU5Q9D8/RhcQPGX69Wam40dutolucbY38EVAjqr2m7xPi71XAicPNaD\n" \  
"aeQQmxkqtilX4+U9m5/wAl0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNV\n" \  
"HQ8BAf8EBAMCAQYwHQYDVR0OBBYEFMSnsaR7LHH62+FLkHX/xBVghYkQMA0GCSqG\n" \  
"SIb3DQEBBQUAA4IBAQCjGiybFwBcqR7uKGY3Or+Dxz9LwwmglSBd49lZRNI+DT69\n" \  
"ikugdB/OEIKcdBodfpga3csTS7MgROSR6cz8faXbauX+5v3gTt23ADq1cEmv8uXr\n" \  
"AvHRAosZy5Q6XkjEGB5YGV8eAlrwDPGxrancWYaLbumR9YbK+rlmM6pZW87ipxZz\n" \  
"R8srzJmwN0jP41ZL9c8PDHIyh8bwRLtTcm1D9SZImlJnt1ir/md2cXjbDaJWFBM5\n" \  
"JDGFoqgCWjBH4d1QB7wCCZAA62RjYJsWvIjJEubSfZGL+T0yjWW06XyxV3bqxbYo\n" \  
"Ob8VZRzI9neWagqNdwvYkQsEjgfbKbYK7p2CNTUQ\n" \  
"-----END CERTIFICATE-----\n";

const uint8_t* clocks[] = { 
  vfd_35x67_0,vfd_35x67_1,vfd_35x67_2,vfd_35x67_3,vfd_35x67_4,
  vfd_35x67_5,vfd_35x67_6,vfd_35x67_7,vfd_35x67_8,vfd_35x67_9
};

const uint8_t* icons[] = {
  icon_01d, icon_01n, icon_02d, icon_02n, icon_03d, icon_03n,
  icon_04d, icon_04n, icon_09d, icon_09n, icon_10d, icon_10n,
  icon_11d, icon_11n, icon_13d, icon_13n, icon_50d, icon_50n
};

const char* icons_names[] = {
  "01d", "01n", "02d", "02n", "03d", "03n",
  "04d", "04n", "09d", "09n", "10d", "10n",
  "11d", "11n", "13d", "13n", "50d", "50n",
};

typedef struct {
  char* ssid;
  char* pass;
  char* host;
}WiFi_DataTypeDef;

WiFi_DataTypeDef WiFi_Data[WiFi_ID_Size];
int WiFi_Status = WL_IDLE_STATUS;
int WiFi_ID = 0;                // If you use multiple WiFi AP, you can chose the default from the list here.

typedef struct {
  const char* main;
  const char* description;
  const char* icon;
  float temp;
  float pressure;
  float humidity;
  float temp_min;
  float temp_max;
  float wind_speed;
  float wind_deg;
  float clouds_all;
}WeatherAPI_DataTypeDef;

WeatherAPI_DataTypeDef weatherAPI;
String weather_cityID = "";        // Your OpenWeather CityID. Example : 2267057 = Lisbon, PT
String weather_APIKEY = "";        // Your OpenWeather API Key
String weather_APIData = "";
StaticJsonDocument<1024> json_doc;

int displayMode = 1;
unsigned long display_Timer = 0;

RTC_TimeTypeDef RTC_TimeStruct;
unsigned long lastMinute = 0;

bool web_success = false;
bool btnDetected;
unsigned long m_Timer = 0;
unsigned long m_Timer_WebRefresh = 0;

//Miscellaneous / Not Used
int hallSensor = 0;

void setup() {
  M5.begin();
  M5.Lcd.setRotation(3);
  M5.Axp.ScreenBreath(10);
  
  ClearScreen();

  //Set WiFi AP Connections
  WiFi_Data[0].ssid = "";     // Your WiFi AP SSID
  WiFi_Data[0].pass = "";     // Your WiFi AP Password
  
  /*
  // You can add more WiFI AP SSID so the M5StickC can try to connect to different APs.
  // Change the WiFi_ID_Size to the total number of APs list. (example WiFi_ID_Size = 3)
  WiFi_Data[1].ssid = "";
  WiFi_Data[1].pass = "";

  WiFi_Data[2].ssid = "";
  WiFi_Data[2].pass = "";
  */

  //Connect to WiFi AP List
  while ( WiFi_Status != WL_CONNECTED ) { 
    WiFi_Connect(WiFi_ID);
    if ( WiFi_Status == WL_CONNECT_FAILED ) {
      WiFi_ChangeID();
    }
  }

  //Update NTP DateTime
  UpdateNTP();

  M5.Lcd.setTextSize(1);
  
  m_Timer = millis();
  m_Timer_WebRefresh = m_Timer;
}

void loop() {
  //Refresh API Data
  if ( m_Timer_WebRefresh < millis() ) {
    WiFi_Status = WiFi.status();
    if ( WiFi_Status != WL_CONNECTED) {
      setup();
      return;
    }
    
    WebRefresh();
    m_Timer_WebRefresh = millis() + WebAPI_reload;
    if ( web_success == true ) {
      ReadAPIData();
    }
  }

  //DisplayMode logic
  if ( m_Timer < millis() ) {
    if ( displayMode == 0 ) {
      M5.Rtc.GetTime(&RTC_TimeStruct);
      if (lastMinute != RTC_TimeStruct.Minutes ) {
        display_clock();
      }
    }
    if ( display_Timer < millis() ) {
      switch (displayMode) {
        case 0:
          display_weather();
          display_Timer = millis() + Screen_Update * 2;
          displayMode = 1;
          break;
        case 1:
          ClearScreen();
          display_clock();
          display_Timer = millis() + Screen_Update;
          displayMode = 0;
          break;
      }
    }
    m_Timer = millis() + Loop_Update;
  }

  M5.update();
  btnDetected = false;
  if (M5.BtnA.pressedFor(10) && btnDetected == false) {
    ClearScreen();
    M5.Lcd.println("Refreshing API Web Data.");
    m_Timer_WebRefresh = 0;
    btnDetected = true;
    delay(500); 
  }
  
  CheckMillisReset();
  hallSensor = hallRead();
  delay(200);  
}

void display_weather() {
  ClearScreen();
    
  M5.Lcd.setTextColor( TFT_ORANGE , TFT_BLACK);
  M5.Lcd.drawString(String(round(weatherAPI.temp), 0), 46, 8, 7);

  M5.Lcd.drawString(weatherAPI.main, 6, 1, 2);
  M5.Lcd.pushImage( 2, 17, 42, 42, (uint16_t *)icons[getIndexByKey(weatherAPI.icon)]);
  M5.Lcd.drawString(weatherAPI.description, 2, 62, 2);

  M5.Lcd.drawFastVLine(110, 0, 80, TFT_WHITE);
  M5.Lcd.drawFastVLine(110, 0, 81, TFT_WHITE);

  M5.Lcd.drawString("Mi", 114, 0, 2);
  M5.Lcd.drawString("Ma", 114, 16, 2);
  M5.Lcd.drawString("Hu", 114, 32, 2);
  M5.Lcd.drawString("Wn", 114, 48, 2);
  M5.Lcd.drawString("Cl", 114, 64, 2);
  
  M5.Lcd.drawString(String(round(weatherAPI.temp_min), 0), 136, 0, 2);
  M5.Lcd.drawString(String(round(weatherAPI.temp_max), 0), 136, 16, 2);
  M5.Lcd.drawString(String(round(weatherAPI.humidity), 0), 136, 32, 2);
  M5.Lcd.drawString(String(round(weatherAPI.wind_speed), 0), 136, 48, 2);
  M5.Lcd.drawString(String(round(weatherAPI.clouds_all), 0), 136, 64, 2);

  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
}

int getIndexByKey( const char * key ) {
  for ( uint8_t i = 0; i < sizeof( icons_names ) / sizeof( char * ); i++ ) {
    if ( !strcmp( key, icons_names[i] ) ) {
      return i;
    }
  };
  return -1;
}

void display_clock(){
  M5.Rtc.GetTime(&RTC_TimeStruct);
  int h1 = int(RTC_TimeStruct.Hours / 10 );
  int h2 = int(RTC_TimeStruct.Hours - h1*10 );
  int i1 = int(RTC_TimeStruct.Minutes / 10 );
  int i2 = int(RTC_TimeStruct.Minutes - i1*10 );
  int s1 = int(RTC_TimeStruct.Seconds / 10 );
  int s2 = int(RTC_TimeStruct.Seconds - s1*10 );
  
  M5.Lcd.pushImage(  2,8,35,67, (uint16_t *)clocks[h1]);
  M5.Lcd.pushImage( 41,8,35,67, (uint16_t *)clocks[h2]);
  M5.Lcd.drawPixel( 79,30, ORANGE); M5.Lcd.drawPixel( 79,56,ORANGE); 
  M5.Lcd.drawPixel( 79,29, YELLOW); M5.Lcd.drawPixel( 79,55,YELLOW); 
  M5.Lcd.pushImage( 83,8,35,67, (uint16_t *)clocks[i1]);
  M5.Lcd.pushImage(121,8,35,67, (uint16_t *)clocks[i2]);

  lastMinute = RTC_TimeStruct.Minutes;
  
  if ( s1 == 0 && s2 == 0 ){ fade();}
}

void fade(){
  for (int i=7;i<16;i++){M5.Axp.ScreenBreath(i);delay(25);}
  for (int i=15;i>7;i--){M5.Axp.ScreenBreath(i);delay(25);}
  M5.Axp.ScreenBreath(10);
}

void ReadAPIData() {
  //ClearScreen();

 // Deserialize the JSON document
  DeserializationError json_error = deserializeJson(json_doc, weather_APIData);

  // Test if parsing succeeds.
  if (json_error) {
    //M5.Lcd.println("Parsing failed...\n API Data error.");
    //M5.Lcd.println(json_error.c_str());
    //delay(2000);
    return;
  }

  JsonObject node; 
  node = json_doc["weather"][0];
  weatherAPI.main = node["main"]; // "Clouds"
  weatherAPI.description = node["description"]; // "broken clouds"
  weatherAPI.icon = node["icon"]; // "04n"

  node = json_doc["main"];
  weatherAPI.temp = node["temp"]; // 18.52
  weatherAPI.pressure = node["pressure"]; // 1017
  weatherAPI.humidity = node["humidity"]; // 82
  weatherAPI.temp_min = node["temp_min"]; // 16.67
  weatherAPI.temp_max = node["temp_max"]; // 20
  weatherAPI.wind_speed = json_doc["wind"]["speed"]; // 5.1
  weatherAPI.wind_deg = json_doc["wind"]["deg"]; // 330
  weatherAPI.clouds_all = json_doc["clouds"]["all"]; // 75
}

void WebRefresh() {
  HTTPClient webclient;

  //ClearScreen();
    
  weather_APIData = "";
  web_success = false;
  webclient.setReuse(false);

  //TODO: expand to forecast? 
  webclient.begin("http://api.openweathermap.org/data/2.5/weather?id=" + weather_cityID + "&units=metric&appid=" + weather_APIKEY);
  //For SSL Connection:
  //webclient.begin("https://api.openweathermap.org/data/2.5/weather?id=" + weather_cityID + "&units=metric&appid=" + weather_APIKEY, webcert);

  int httpCode = webclient.GET();
  if(httpCode > 0) {
    M5.Lcd.printf("[HTTP] GET code: %d\n", httpCode);

    // file found at server
    if(httpCode == HTTP_CODE_OK) {
      weather_APIData = webclient.getString();
      web_success = true;
      //M5.Lcd.print(weather_APIData.c_str());
      //M5.Lcd.println();
    }
  } else {
    //M5.Lcd.printf("[HTTP] GET failed, error: %s\n", webclient.errorToString(httpCode).c_str());
    m_Timer_WebRefresh = millis() + 60000;
  }
  webclient.end();
}

void WiFi_Connect(int _WiFi_ID) {
  ClearScreen();
  M5.Lcd.print("Attempting to connect to SSID: ");
  M5.Lcd.println( WiFi_Data[_WiFi_ID].ssid );

  // WiFi.setTxPower( WIFI_POWER_19_5dBm );
  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);
  
  // Connect to WPA/WPA2 network
  WiFi.disconnect(true); 
  WiFi.begin( WiFi_Data[_WiFi_ID].ssid, WiFi_Data[_WiFi_ID].pass );

  // Connect to Wifi network
  WiFi_Status = WiFi.status();
  int loopcount = 0;
  while ( WiFi_Status != WL_CONNECTED) { 
    M5.Lcd.print(".");

    if ( WiFi_Status == WL_NO_SSID_AVAIL || WiFi_Status == WL_CONNECT_FAILED ) {
      WiFi_Status = WL_CONNECT_FAILED;
      break;
    }
   
    if (loopcount > 100) {
      WiFi_Status = WL_CONNECT_FAILED;
      Serial.print("LoopCount > 100");
      break;
    }
    delay(500);
    WiFi_Status = WiFi.status();
  }

  // Connected to Wifi network
  if ( WiFi_Status == WL_CONNECTED ) {
    M5.Lcd.println("!");
  
    // print your WiFi shield's IP address:
    ClearScreen();
    M5.Lcd.println("Connected to SSID: ");
    M5.Lcd.println( WiFi_Data[_WiFi_ID].ssid );
  
    M5.Lcd.print("IP Address: ");
    M5.Lcd.println(WiFi.localIP()); 

    delay(2000);
  }
}

void WiFi_ChangeID() {
  // Switch WiFi AP auth
  WiFi_ID++;
  if ( WiFi_ID >= WiFi_ID_Size ) {
    WiFi_ID = 0;
  }
}

void UpdateNTP() {
// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

  ClearScreen();
  M5.Lcd.println("Updating Date and Time.");
  
  timeClient.begin();
  timeClient.setTimeOffset(3600); // GMT +1 = 3600

  while(!timeClient.update()) {
    timeClient.forceUpdate();
    M5.Lcd.print(".");
    delay(200);
  }

  // Extract date
  time_t rawtime = timeClient.getEpochTime();
  struct tm * ti;
  ti = localtime (&rawtime);
  uint16_t year = ti->tm_year + 1900;
  String yearStr = String(year);
  uint8_t month = ti->tm_mon + 1;
  String monthStr = month < 10 ? "0" + String(month) : String(month);
  uint8_t day = ti->tm_mday;
  String dayStr = day < 10 ? "0" + String(day) : String(day);

  ClearScreen();
  M5.Lcd.println("Internet:");
  M5.Lcd.print("DATE: ");
  M5.Lcd.printf("%02d/%02d/%04d\n", dayStr.toInt() , monthStr.toInt() , yearStr.toInt() );
  M5.Lcd.print("TIME: ");
  M5.Lcd.println(timeClient.getFormattedTime());
  delay(2000);

  ClearScreen();
  M5.Lcd.println("Saving: ");
  RTC_DateTypeDef myDate;
  myDate.Date = dayStr.toInt();
  myDate.Month = monthStr.toInt();
  myDate.Year = yearStr.toInt();
  myDate.WeekDay = timeClient.getDay();
  M5.Rtc.SetData(&myDate);
  delay(100);
  RTC_TimeTypeDef myTime;
  myTime.Hours = timeClient.getHours();
  myTime.Minutes = timeClient.getMinutes();
  myTime.Seconds = timeClient.getSeconds();
  M5.Rtc.SetTime(&myTime);
  delay(100);

  RTC_DateTypeDef myDate2;
  RTC_TimeTypeDef myTime2;
  M5.Rtc.GetData(&myDate2);
  M5.Rtc.GetTime(&myTime2);
  delay(100);
    
  M5.Rtc.GetBm8563Time();
  M5.Lcd.printf("%02d/%02d/%04d\n", myDate2.Date , myDate2.Month , myDate2.Year );
  M5.Lcd.printf("%02d:%02d:%02d\n", myTime2.Hours , myTime2.Minutes, myTime2.Seconds);
  M5.Lcd.println("Saved!");
  delay(1000);

  timeClient.end();
}

void ClearScreen() {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0, 2);
}

void CheckMillisReset() {
  // After 50 days the millys unsigned long overflow and returns to 0
  // this function check for it and if necessary reset all other timers.
  if ( millis() < 5000 ) {
    m_Timer = 0;
    m_Timer_WebRefresh = 0;
    display_Timer = 0;
  }
}
