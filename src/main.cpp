#include "Modules.h"

ControlManager *comm; 
TemperatureMonitor *temp; 
Timer<3> timer; 

const int adcPin = 34;
static const uint32_t loop_delay = 1; // ms delay
TaskHandle_t mqtt;
TaskHandle_t sensor;

bool sensorUpdate(void *arg=nullptr)
{
  temp->update();
  return true; 
}

bool pubTemp(void *arg)
{
  comm->pub_temperature();
  return true;
}

void mqttCommTask( void * parameter )
{
  do{
    comm->loop();
    vTaskDelay(loop_delay / portTICK_PERIOD_MS);
  }while(true);
  
}

void periodicMonitoringTask( void * parameter )
{
  do{
    
    timer.tick();
    vTaskDelay(loop_delay / portTICK_PERIOD_MS);
    sensorUpdate();

  }while(true);
  
}

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.begin(115200);
  
  temp = new TemperatureMonitor(adcPin);

  comm = new ControlManager(temp);
  delay(500);
  timer.every(250, pubTemp);

  xTaskCreatePinnedToCore(mqttCommTask,"mqtt",10000,NULL,1,&mqtt,0);                         
  delay(500); 

  xTaskCreatePinnedToCore(periodicMonitoringTask,"sensor",10000,NULL,1,&sensor,1);          
  delay(500); 
  

}

void loop() 
{
  
} 
