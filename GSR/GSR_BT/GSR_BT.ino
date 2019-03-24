#include <stdio.h>
#include <stdlib.h>

const int GSR=A0;
int sensorValue=0;
int gsrAvg=0;
int sum = 0;
int gsrDisFlag = 0;
int gsrReadyFlag = 0;
/*double gsrAvgD=0;
double resistance=0;
double resistanceRevert=0;
long sumSum=0;
double sumAvg=0;
int sumCount=0;
long sum=0;
double sumAvgRevert=0;*/


void setup(){
  Serial.begin(9600);
}

void loop(){
  if(gsrReadyFlag = 0){
    sum = 0;
    for(int i=0;i<512;i++)           //Average the 10 measurements to remove the glitch
        {
        sensorValue=analogRead(GSR);
        sum += sensorValue;
        delay(9);
        }
    gsrAvg = sum/512;
    if(gsrAvg > 505)
        gsrDisFlag = 1;
    else gsrDisFlag = 0;
    
    gsrReadyFlag = 1;
    
   /*sumCount++;
   resistanceRevert = resistance;
   
   if (gsrAvg >= 512)
      resistance = 0;
   else{
      gsrAvgD = (double)gsrAvg;
      resistance = ((1024+(2*gsrAvgD))*10)/(512-gsrAvgD);
      sumSum += resistance;
      sumAvgRevert = sumAvg;
      sumAvg = sumSum / sumCount;
     }

    if(resistance > (sumAvg + sumAvg*0.2)  || resistance < (sumAvg - sumAvg*0.2)){
      resistance = resistanceRevert;
      sumAvg = sumAvgRevert;
      Serial.print("Output out of range");
      Serial.print(", ");
      Serial.println(gsrAvg);
     }
     else{*/
    Serial.print(gsrAvg);
    Serial.print(", ");
    Serial.println(gsrDisFlag);
    Serial.print(", ");
    Serial.println(gsrReadyFlag);
      /*}*/
  }
}
