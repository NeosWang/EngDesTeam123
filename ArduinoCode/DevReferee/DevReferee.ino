#include <VirtualWire.h>  //library for RF kit
#include <FreqMeasure.h>  //library for frequence measurement

// default transmitter on D12
// sound detection sensor on D8
void setup() {
  Serial.begin(9600);
  FreqMeasure.begin();    //initialize including pbs of processor
}

double sum=0;
int count=0;
float frequency;
int continuity =0;

void loop() {
  // while receive sound, calculate the sine wave occurs during the duration
  // and convert to square wave with frequency
  if (FreqMeasure.available()) {
    sum = sum + FreqMeasure.read();
    count = count + 1;
    if (count > 100) {
      frequency = FreqMeasure.countToFrequency(sum / count);
      Serial.print(frequency); 
      Serial.println("Hz");
      sum = 0;
      count = 0;
    }
  }
  // reach the speific frequecy range of whistle and count the duration
  if (frequency>2700 && frequency<2830)
  { 
    continuity++;
    Serial.println(continuity);
    frequency=0;
  }
  else if((frequency>0 && frequency<=2700)||frequency>=2830)
  {
    continuity=0;
    Serial.println(continuity);
    frequency=0;
  }

  if (continuity >=4)
  { 
    continuity=0; 
    send();
    delay(2000);
  }
}

void send ()
{ 
  // bits per sec, must setup inside function
  // cuz the RF and Frequency converter need initialize different bps on processor
  vw_setup(2000);         
  const char * message="Defo_Fouls";
  Serial.println(message);
  vw_send((uint8_t *)message, strlen(message));
  vw_wait_tx();           // wait until the whole message is sent
  FreqMeasure.begin();
}
