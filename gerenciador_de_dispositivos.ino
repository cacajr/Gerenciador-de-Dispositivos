//=========ESP8266 BIBLIOTECA===== 
#define BLYNK_PRINT Serial  
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include <TimeLib.h>
#include <WidgetRTC.h> //SERVIRA PARA SINCRONIZACAO DAS HORAS DO MICRO COM O HORARIO FORNECIDO PELO SERV BLYNK
#include <ListLib.h>

//==========================================
char ssid[] = "...";//rede wifi        
char pass[] = "..."; //senha wifi
char auth[] = "..."; //token do blynk

boolean hasStartTime;
boolean hasStopTime;
int getStartTimeInSeconds;
int getStopTimeInSeconds;
int getCurrentTimeInSeconds;

List<int> days;

BlynkTimer timer;
WidgetRTC rtc; //declarando a variável RTC (LER O COMENTARIO DA BIBLIOTECA ACIMA)

void setup()
{
  // Debug console
  Serial.begin(115200);
  pinMode(13,OUTPUT); // NODEMCU PIN D7
  pinMode(5,OUTPUT); // NODEMCU PIN D1
  pinMode(4,OUTPUT); // NODEMCU PIN D2
  Serial.println(auth);
  Serial.println(ssid);
  Serial.println(pass);
  
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8442);
  Blynk.begin(auth, ssid, pass);

  //inicia o RTC
  rtc.begin();
  
}
//LINCAR A VARIAVEL VIRTUAL COM A DIGITAL
//========LED VERMELHO=======================================
BLYNK_WRITE(V0)
{   
    if (param.asInt())
    {       
        digitalWrite(13, HIGH); //liga pino d7
    } 
    else 
    {
        digitalWrite(13, LOW);  //desliga pin d7
    }
}
//========LED AMARELO=========================================
BLYNK_WRITE(V1)
{
    if (param.asInt())
    {       
        digitalWrite(4, HIGH); //liga pino d2
    } 
    else 
    {
        digitalWrite(4, LOW); //desliga pin d2 
    }
}
//======LED VERDE===========================================
BLYNK_WRITE(V2)
{
    if (param.asInt())
    {       
        digitalWrite(5, HIGH); //liga pin d1
    } 
    else 
    {
        digitalWrite(5, LOW);  //desliga pin d1
    }
}
//=======CUSTOMIZAÇÃO======================================
BLYNK_WRITE(V3)
{
    if (param.asInt())
    {       
        digitalWrite(13, HIGH); 
        Blynk.virtualWrite(V0,HIGH);
        digitalWrite(4, HIGH); 
        Blynk.virtualWrite(V1,HIGH); 
        digitalWrite(5, HIGH); 
        Blynk.virtualWrite(V2,HIGH);      
    } 
    else 
    {
        digitalWrite(13, LOW);  
        Blynk.virtualWrite(V0,LOW);
        digitalWrite(4, LOW);  
        Blynk.virtualWrite(V1,LOW);
        digitalWrite(5, LOW);  
        Blynk.virtualWrite(V2,LOW);     
    }
}
//=======INPUT_TIME========================================
BLYNK_WRITE(V4) {
  TimeInputParam t(param);

  // Process start time

  if (t.hasStartTime())
  {
    Serial.println(String("Start: ") +
                   t.getStartHour() + ":" +
                   t.getStartMinute() + ":" +
                   t.getStartSecond());

    hasStartTime = t.hasStartTime();
    getStartTimeInSeconds = ((t.getStartHour()*3600) + (t.getStartMinute()*60) + t.getStartSecond());
    
  }
  else
  {
    // Do nothing
  }

  // Process stop time

  if (t.hasStopTime())
  {
    Serial.println(String("Stop: ") +
                   t.getStopHour() + ":" +
                   t.getStopMinute() + ":" +
                   t.getStopSecond());

    hasStopTime = t.hasStopTime();
    getStopTimeInSeconds = ((t.getStopHour()*3600) + (t.getStopMinute()*60) + t.getStopSecond());
                   
  }
  else
  {
    // Do nothing: no stop time was set
  }

  // Process timezone
  // Timezone is already added to start/stop time

  Serial.println(String("Time zone: ") + t.getTZ());

  // Get timezone offset (in seconds)
  Serial.println(String("Time zone offset: ") + t.getTZ_Offset());

  // Process weekdays (1. Mon, 2. Tue, 3. Wed, ...)

  days.Clear();
  for (int i = 1; i <= 7; i++) {
    if (t.isWeekdaySelected(i)) {
      Serial.println(String("Day ") + i + " is selected");
      
      days.Add(i);     
    }
  }
  Serial.println(String("Today is ")+weekday());
  Serial.println();

}
//=======RUN BLYNK=========================================
void loop()
{ 

  Blynk.run();

  getCurrentTimeInSeconds = ((hour()*3600) + (minute()*60) + second());
  
  if ( (weekday() == 1 and days.Contains(7)) or (days.Contains(weekday()-1)) ) {
    //is it time to turn on and to turn off?
    if (hasStartTime and hasStopTime) {
      if (getCurrentTimeInSeconds < getStartTimeInSeconds) {
        //deixo do jeito que esta
      }
      else if (getCurrentTimeInSeconds >= getStartTimeInSeconds and getCurrentTimeInSeconds < getStopTimeInSeconds) {
        digitalWrite(13, HIGH); //liga pino d7
        Blynk.virtualWrite(V0,HIGH);
        hasStartTime = false;
      }
      else if (getCurrentTimeInSeconds >= getStopTimeInSeconds) {
        digitalWrite(13, LOW); //desliga pino d7
        Blynk.virtualWrite(V0, LOW);
        hasStartTime = false;
        hasStopTime = false;
      }
    }
    //is it time to turn on?
    else if (hasStartTime) {
      if (getCurrentTimeInSeconds >= getStartTimeInSeconds) {
        digitalWrite(13, HIGH); //liga pino d7
        Blynk.virtualWrite(V0,HIGH);
        hasStartTime = false;
      }
    }
    //is it time to turn off?
    else if (hasStopTime) {
      if (getCurrentTimeInSeconds >= getStopTimeInSeconds) {
        digitalWrite(13, LOW); //desliga pino d7
        Blynk.virtualWrite(V0, LOW);
        hasStopTime = false;
      }
    }
  }
 
}
