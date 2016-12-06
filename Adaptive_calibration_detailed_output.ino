/* 
 This function can read values up to 121360 lux with adaptive calibration for best possible results.
 It first makes a reading with lowest sensitivity (with highest range,up to 121360 lux)
 Then determines the best sensitivity/range according to the initial reading.
 Then makes a reading using obtained sensitivity
 The resulting reading is made with the best possible sensitivity according to the current light condition.

 This example shows the steps with detailed output
*/

#include <Wire.h>
#include <BH1750FVI.h>

BH1750FVI bh1750;

long    TotalDuration;
float   CalcRatio = 0.45;  
float   NewCalcRatio = 0.45; 
float   FirstReading = 0;
float   SecondReading = 0; 
int     CalcDuration = 0;  
long    duration = 0;
String  reading = "";


void setup() {
  Wire.begin();
  Serial.begin(115200);
  bh1750.begin();
}

void loop() 
  {
  FirstReading = 0;
  SecondReading = 0; 
  CalcDuration = 0;  
  TotalDuration = 0;
  TotalDuration = millis();
  
  bh1750.Reset();
  CalcDuration = bh1750.setSensitivity(CalcRatio*69);    // set sensitivity using ratio
  bh1750.startOngoingSamples(BH_ContL);                  // set mode
  delay(CalcDuration+2);                                 // just to avoid 0 values since while loop sometimes does not wait properly
  
  duration = millis();
  while(!bh1750.sampleIsFresh()) {}                      // wait for sensor to produce a new reading 
  duration = millis() - duration;
  FirstReading = (bh1750.getLightLevel('c')/CalcRatio);  //read the calibration-adjusted value, according to the sensitivity setting

  //Prepare output
  reading = "Initial low sensitivity reading= "+String(FirstReading)+"        ";
  reading = reading.substring(0,43) + "| r= x" + String(CalcRatio) + " | Calc= " + String(CalcDuration+2)+"ms | Wait= " + String(duration)+"ms           ";

  NewCalcRatio = 54612/(FirstReading * 1.05);            // calculate new ratio   
  if (NewCalcRatio < 0.45) NewCalcRatio = 0.45;
  if (NewCalcRatio > 3.68) NewCalcRatio = 3.68;

  //make a new reading using obtained sensitivity --------------------------------------------------------------------------------------------------
  bh1750.Reset();
  CalcDuration = bh1750.setSensitivity(NewCalcRatio*69); // set sensitivity using new ratio (adaptive best sensitivity) and obtain necessary duration for calculation
  bh1750.startOngoingSamples(BH_ContL);                  // set mode
  delay(CalcDuration+2);                                 // just to avoid 0 values since while loop sometimes does not wait properly
  
  duration = millis();
  while(!bh1750.sampleIsFresh()) {}                      // wait for sensor to produce a new reading
  duration =   millis() - duration;
  
  //read the calibration-adjusted value, according to the sensitivity setting
  SecondReading = (bh1750.getLightLevel('c')/NewCalcRatio);
  
  reading = reading.substring(0,81) + "||  Secondary best sensitivity reading= "+  String(SecondReading) + "          ";
  reading = reading.substring(0,130) +" | r= x" + String(NewCalcRatio) + " | Calc= " + String(CalcDuration+2)+"ms | Wait= " + String(duration)+"ms  || Total duration= "+ String(millis() - TotalDuration)+"ms ";

  Serial.println(reading);

  bh1750.powerDown();                                    // power off to save energy, it will start working again in next function call 
  }


