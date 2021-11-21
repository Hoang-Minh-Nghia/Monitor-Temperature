#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "Wire.h"
#include"FirebaseESP8266.h"
#include"Adafruit_MLX90614.h"
#include<time.h>
#include <stdlib.h>
#include <stdio.h>


#include<NTPClient.h>
#include<WiFiUdp.h>



#define FIREBASE_HOST "temperature-ad243-default-rtdb.firebaseio.com" //Thay bằng địa chỉ firebase của bạn
#define FIREBASE_AUTH "8l7NhiunAFT3Dk6aAymLLpKFrAFti2nNi0pHLH1o"   //Không dùng xác thực nên không đổi
#define WIFI_SSID "LAB5"   // Tên wifi và mật khẩu
#define WIFI_PASSWORD "12345678"
#define MAXX 50

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
FirebaseData fbdo;


char * dayy = new char[MAXX];
char * timee= new char[MAXX];

char ch[10]={'/', '\0'};
char amb[MAXX]= "Ambient";
char obj[MAXX]= "Object";
char nodeObject[MAXX];
char nodeAmbient[MAXX];


String formattedDate;
String formattedTime;
String dayStamp;
String timeStamp;

int i=0;
float Object;
float Ambient;



const long utcOffsetInSeconds = 3600;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);


void setup()
{

  Serial.begin(9600);
  mlx.begin();  // khoi tạo mlx

  pinMode(LED_BUILTIN, OUTPUT); // bat leb esp
  digitalWrite(LED_BUILTIN,LOW);//

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }

  timeClient.begin();
  timeClient.setTimeOffset(+7*60*60);

  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); // khoi tao database
  Firebase.reconnectWiFi(true);
}

void loop()
{
  while (!timeClient.update())
  {
    timeClient.forceUpdate();
  }
  

  Object=mlx.readObjectTempC();
  Object=roundf(Object*100)/100;
  
  Ambient=mlx.readAmbientTempC();
  Ambient=roundf(Ambient*100)/100;


  Firebase.setFloat(fbdo,"/Temperature/Ambient",Ambient);
  Firebase.setFloat(fbdo,"/Temperature/Object",Object);


  formattedDate=timeClient.getFormattedDate();
  formattedTime=timeClient.getFormattedTime();

  Serial.println(formattedDate);
  Serial.println(formattedTime);

  strcpy(dayy,formattedDate.c_str());
  strcpy(timee,formattedTime.c_str());
  strcat(dayy,ch);
  strcat(dayy,timee);

  strcat(nodeAmbient,dayy);
  strcat(nodeAmbient,ch);
  strcat(nodeAmbient,amb);

  strcat(nodeObject,dayy);
  strcat(nodeObject,ch);
  strcat(nodeObject,obj);

  Firebase.setFloat(fbdo, nodeAmbient,Ambient ); 
  Firebase.setFloat(fbdo, nodeObject,Object );
  

  memset(nodeObject,'\0',MAXX);
  memset(nodeAmbient,'\0',MAXX);
  memset(dayy,'\0',MAXX);
  memset(timee,'\0',MAXX);
  delay(500);
}