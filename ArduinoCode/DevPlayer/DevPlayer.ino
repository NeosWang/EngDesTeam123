// if object in the dectecting range of an ultrasonic sensor, 
// the corresponding vibration motor shal vibrate intermittently(0.25s on 0.25s off)
// if receive radio signal that referee whistled, all vibration motors vibrate together last 3 second

// VirtualWire changes TIMER1 setting, pwm pin only 5,6 won't get conflicted under default configation.
// change configration on pin of rx from default 11 to 12, then offer 11 as pwm

#include <VirtualWire.h>  //library for RF kit

byte message[VW_MAX_MESSAGE_LEN];  // come in message as byte array
byte messageLength=VW_MAX_MESSAGE_LEN;  // length of array

const int trigPin[3]={2,4,8};      // trigger pin of 3 ultrasensors
const int echoPin[3]={3,7,9};      // echo pin of 3 ultrasensors

const int vibPin[3]={5,6,11};      // power source pin of 3 vibrations wz pwm func

const int rfRcv=12;                // pin number of RF receiver

unsigned long previousMillis = 0;  // record timing
const long interval = 250;         // interval of vibration on and off

int vibRun[3]={0,0,0};             // whehter turn on the vibraton

bool vibState=false;

void setup() {
  Serial.begin (9600);
  pinMode(trigPin[0],OUTPUT);
  pinMode(trigPin[1],OUTPUT);
  pinMode(trigPin[2],OUTPUT);
  
  pinMode(echoPin[0], INPUT);
  pinMode(echoPin[1], INPUT);
  pinMode(echoPin[2], INPUT);
  
  pinMode(vibPin[0], OUTPUT);
  pinMode(vibPin[1], OUTPUT);
  pinMode(vibPin[2], OUTPUT);

  vw_set_rx_pin(rfRcv); // change configration
  vw_setup(2000);     // initialize RF receiver,Bits per sec
  vw_rx_start();

}

void loop(){
  // while no message come in through RF, keep 3 ultrasonic sensors running 
 while(!vw_get_message(message,&messageLength)){
  MeasureAndVibrate();
 }
 //convert byt array to unsigned char to string
 String rfSign=String((char*)message);
 // check if received string is from transmitter
 if(rfSign=="Defo_Fouls"){
  Serial.println(rfSign);
  // all vibrations works 3 sec 
  VibrateAll();
 }  
}

void MeasureAndVibrate(){
  unsigned long currentMillis = millis();  //get current timing

  //3 ultrasonic measuring, if object inside range, keep vibration working
  Measurement(0); 
  Measurement(1);
  Measurement(2);

  //after each interval, change the state of vibration
  if (currentMillis - previousMillis >= interval)  
  {
    previousMillis = currentMillis;
    vibState=!vibState;
    
    //if specific vibration should be working, then intermittently vibrate
    if(vibState){
      for(int i=0; i<3; i++){
        analogWrite(vibPin[i],vibRun[i]);     
      }
    }else{
      for(int i=0; i<3; i++){
        analogWrite(vibPin[i],0);     
      }
    }
  }
}



void Measurement(int nr){
  long duration, distance;
  //trigger wave sending
  digitalWrite(trigPin[nr], LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin[nr], HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin[nr], LOW);
  //how long to get reflect wave
  duration = pulseIn(echoPin[nr], HIGH);
  
  // time(trip round)*velocity of sound/2
  distance = (duration*0.034/2);

  // distance to be adjust later
  if (distance <= 50 && distance >10){
    vibRun[nr]=255;
    Serial.print(nr);
    Serial.print(":");
    Serial.print(distance);
    Serial.println("cm");
  }
  else if(distance<=100 && distance >50)
  {
    vibRun[nr]=200;
    Serial.print(nr);
    Serial.print(":");
    Serial.print(distance);
    Serial.println("cm");
  }
  else{
    vibRun[nr]=0;
  }
  delay(50);
}

void VibrateAll(){
  for(int i=0; i<3; i++){
    analogWrite(vibPin[i],255);
  }
  delay(3000);
  for(int i=0; i<3; i++){
    analogWrite(vibPin[i],0);
  }
}
