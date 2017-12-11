#include <DallasTemperature.h>
#include <OneWire.h>

//Relay + 18DS20B def
int R1 = 2;
int ONE_WIRE_BUS = 4;
double temp;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
int relayState = HIGH;

//time variables
int long now = 0;
int long relayStartTime = 0;
int relayInterval = 10000;
int long lastTempTime = 0;
int tempInterval = 1000;
int long lastDataTime = 0;
int dataInterval = 1000;


//PID
double input, output, setpoint;
double errSum, lastErr;
double kp, ki, kd;
double P, I, D;
int iLimit = relayInterval;
double errorArray[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int tInt = 5;

///////////////////////////////////////////
void setup() {
  // define Relay pin, set to off (= HIGH)
  pinMode(R1, OUTPUT);
  digitalWrite(R1, relayState);

  //clicker relay to confirm working
  digitalWrite(R1, LOW);
  delay(250);
  digitalWrite(R1, HIGH);
  delay(250);
  digitalWrite(R1, LOW);
  delay(250);
  digitalWrite(R1, relayState);

 //start serial port
 Serial.begin(9600);  // begin serial transmission
 sensors.begin();  // begin sensor capture
}

////////////////////////////////////////////
void loop() {
  // put your main code here, to run repeatedly:
  
  setpoint = 30.00;
  kp = 4;
  ki = 0.5;
  kd = 20;
  
  

//TEMPERATURE MEASUREMENT/////////////////////////////////////////////////////////
  //check if delta current time vs previous time is greater interval, if so run code
  now = millis();
  if(now - lastTempTime > tempInterval) {
    lastTempTime = now;
    sensors.requestTemperatures();
    temp = sensors.getTempCByIndex(0);
  }
    //////////////////////////////////////////////////////////////////////////////
  
  //Drive Relay using "output" in ms as part of relayInterval
  now = millis();
//  Serial.println(String(relayTime));
  if((now - relayStartTime) > relayInterval){  //if the time since last loop is greater than interval do
    
  
//    Serial.println(String("in the loop"));
    //PID CONTROL///

    input = temp;
    /*how long since we last calculated*/
    int timeChange = (double)(now - relayStartTime);
    
    /*compute the working error variables*/
    double error = setpoint - input;

    int i;
    for(i = sizeof(errorArray); i > 0; i--){
      if(i = 1){
        errorArray[i-1] = error * timeChange;
      }
      else{
        errorArray[i-1] = errorArray[i-2];
      }
    }
    
    errSum = 0;  // reset errSum so it only contains the last tInt number of errors
    for(i = 0; i < tInt; i++){
      errSum += errorArray[i];
    }
    
//    errSum += (error * timeChange); removed due to errorArray handling of integral over a number of cycles
    double dErr = (error - lastErr) / timeChange;

//    Serial.println(String("err: ") + String(error) + String(" errSum: ") + String(errSum) + String(" dErr: ") + String(dErr));

    /*compute PID output*/
    P = kp * error * 1000;
    I = ki * errSum;
    if(I > iLimit){
      I = iLimit;
    }
    if(I < (-1 * iLimit)){
      I = (-1 * iLimit);
    }
    D = kd*dErr*10000000;
    output = P + I + D;

    //limit output to full interval lenght
    if(output > relayInterval){
      output = relayInterval;
    }

    //if above target temperature (negative error), set output to 0
    if(output < 0){
      output = 0;
    }

    /*rememer some vars for next time*/
    lastErr = error;

    /*increase old relaystarttime by interval duration*/
    relayStartTime += relayInterval;
  }
  ///////////////

  /*Relay operation independent from the PID checks at fixed intervals*/
  if((output > 100) && (output > (now - relayStartTime))){  // if the PWM has not passed yet
    relayState = LOW; //R ON
    digitalWrite(R1, relayState);
  }
  else{
    relayState = HIGH;  // R off
    digitalWrite(R1, relayState);
  }


  ///////////////////////////////

  //DATA OUTPUT/////////////////
  now = millis();
  if(now - lastDataTime > dataInterval){
    lastDataTime = now;
   Serial.println(String(now/1000)+String(';')+String(temp)+String(';')+String(relayState)+String(';')+String(P)+String(';')+String(I)+String(';')+String(D)+String(';')+String(output)+String(';')+String(setpoint));
  }
  /////////////////////////////
}
