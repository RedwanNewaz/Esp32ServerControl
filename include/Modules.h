#pragma once 
#include "Core.h"

class TemperatureMonitor{
public:
    TemperatureMonitor(int pin):pin_(pin)
    {
        analogReadResolution(12);
        state_ = read();
    }
    void update()
    {
        double alpha = 0.5; 
        state_  = alpha * state_ + (1 - alpha) * read();
    }

    double get()
    {
        return state_;
    }
private:
    double read()
    {
        return analogRead(pin_);
    }
    int pin_; 
    double state_; 
};


class SpeedControl{
public:
  SpeedControl(int pin):pin_(pin)
  {
    // configure LED PWM functionalitites
    ledcSetup(pwmChannel_, freq_, resolution_);
    // attach the channel to the GPIO to be controlled
    ledcAttachPin(pin, pwmChannel_);
  }
  void setDutyCycle(double val)
  {
    if(val < 0 || val > 100) return;

    auto cmd = dutyCyleToValue(val);
    ledcWrite(pwmChannel_, cmd);
    Serial.printf("PWM value for pin = %d set = %d \n", pin_, cmd);
  }
protected:
  int dutyCyleToValue(double x)
  {
    // convert into pwm number 
    return pow(2, resolution_) * x / 100.0;
  }
private:
  const int pin_;
  // setting PWM properties
  const int freq_ = 5000;
  const int pwmChannel_ = 0;
  const int resolution_ = 16;

};




class ControlManager: public CommunicationClient
{

public:
  ControlManager(TemperatureMonitor *temp):temp_(temp)
  {
    fanController_ = new SpeedControl(16);// 16 corresponds to GPIO16
  }

  void power_button(int value)
  {
    digitalWrite(LED_BUILTIN, value);
  }

  void fan_speed(int value)
  {
    fanController_->setDutyCycle(value);
  }


  void pub_temperature()
  {
    auto val = double2string(temp_->get(),4);
    // Serial.print("temperature = ");
    // Serial.println(val);
    client.publish("/server/temp", val.c_str());
  }

private:
  SpeedControl *fanController_;
  TemperatureMonitor *temp_; 

protected:
  String double2string(double n, int ndec) {
    String r = "";

    int v = n;
    r += v;     // whole number part
    r += '.';   // decimal point
    int i;
    for (i=0;i<ndec;i++) {
        // iterate through each decimal digit for 0..ndec 
        n -= v;
        n *= 10; 
        v = n;
        r += v;
    }

    return r;
}
  
};
