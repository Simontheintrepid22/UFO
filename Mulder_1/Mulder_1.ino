//Input pins
int ledPins[] = {6,5,10,3,11,12,8,7}; //Define LEDs. 2-6 are yellow, 7-9 are Blue, Green, Red
int sensorPin = A0; // select the input  pin for  the sound sensor potentiometer 

//Sensor variables
int sensorValue = 0;  // variable to  store  the value  coming  from  the sound sensor
int sensorHigh = 0; //stores the highest (quietest) ssound sensor value
int sensorLow = 1000; //stores the lowest (loudest) ssound sensor value
unsigned long decayLow = 0; //variable to store time since loudest noise was recorded and decay it towards ambient
unsigned long decayHigh = 0; //variable to store time since quietest noise was recorded and decay it towards ambient
int decaySpeed = 1000; //Speed of loudest noise decay
int soundRange = 0;
int currentVolume = 0;
int volumePercent = 0;

//Change function inputs
unsigned long functionDelay = 0; //Records millis to track time since function change
unsigned long functionSpeed = 30000; //Time between change in functions
int functionNo = 0; //Integers to number individual functions

//Chaser inputs
unsigned long chaserDelay = 1; //variable to record time since chaser light change
unsigned long chaserSpeed = 10; //variable for speed of chaser loop - higher is slower
int chaserOff = 0; //Integers to turn LEDs off
int chaserOn = 1;

//PWM fader inputs
int yellBright = 0; //PWM brightness of yellow LEDs
int pulseState = 0; //State 0 to go brighter, State 1 to go dimmer
int pulseSpeedControl = 1; //variabel to modify pulse speed
int pulseSpeed = 1; //Speed of pulse cycle, higher is faster

//RGB inputs
unsigned long RGBDelay = 0; //variable to record time since Rgb sequence began
int RGBSpeedControl = 1000; // variable to modify RGB soeed
int RGBSpeed = 1000; //RGB speed control but modified by volume

void setup()
{
Serial.begin(9600);
int index;

for(index = 0; index <= 7; index++){
  pinMode(ledPins[index],OUTPUT);
  }
}

void loop(){

//Sound sensor
  sensorValue = analogRead(sensorPin); //Reads sound sensor and saves value
  if (sensorValue < sensorLow){ //Records loudest noise
    sensorLow = sensorValue;
    decayLow = millis();
  }
  if (sensorValue > sensorHigh){ //Records quietest noise
    sensorHigh = sensorValue;
    decayHigh = millis();
  }

  if (millis() - decayLow >= decaySpeed){ //Reduces loudest recorded noise back towards ambient over time to make noise-based programs more context specific
    sensorLow = sensorLow + 1;
    decayLow = millis();
  }
  if (millis() - decayHigh >= decaySpeed){ //Reduces quietest recorded noise back towards ambient over time to make noise-based programs more context specific
    sensorHigh = sensorHigh - 1;
    decayHigh = millis();
  }
  
  soundRange = sensorHigh - sensorLow; //What the current range of volumes are
  currentVolume = sensorValue - sensorLow; //How loud the current noise is in context
  volumePercent = (100 / soundRange) * currentVolume; //Percentage of how lound the current noise is in context
  if (volumePercent <= 0){ //This just stops % going into negative values
    volumePercent = 1;
  }

  Serial.print ("Current noise: "); Serial.print(sensorValue, DEC); //DEC means decimal, alternatives being BIN binary, OCT octal or HEX hexadecimal. DEC seems to be the default and could be omitted
  Serial.print ("  Loudest: "); Serial.print(sensorLow); Serial.print ("  Quietest: "); Serial.print (sensorHigh); Serial.print ("  Range (max volume): "); Serial.print (soundRange);
  Serial.print ("  Current relative volume (within range): "); Serial.print (currentVolume);  Serial.print ("  Volume %: "); Serial.print (volumePercent); Serial.println ("% ");

//RGB Lights colours in sequence
RGBSpeed = RGBSpeedControl / volumePercent;

  if (millis() - RGBDelay < RGBSpeed){ //Red
    digitalWrite(ledPins[5], HIGH); 
    digitalWrite(ledPins[6], LOW);
    digitalWrite(ledPins[7], LOW); 
  }
  if (millis() - RGBDelay >= RGBSpeed && millis() - RGBDelay < RGBSpeed * 2){ //Yellow
    digitalWrite(ledPins[5], HIGH); 
    digitalWrite(ledPins[6], HIGH);
    digitalWrite(ledPins[7], LOW); 
  }
  if (millis() - RGBDelay >= RGBSpeed * 2 && millis() - RGBDelay < RGBSpeed * 3){ //Green
    digitalWrite(ledPins[5], LOW); 
    digitalWrite(ledPins[6], HIGH); 
    digitalWrite(ledPins[7], LOW);
  }
  if (millis() - RGBDelay >= RGBSpeed * 3 && millis() - RGBDelay < RGBSpeed * 4){ //Cyan
    digitalWrite(ledPins[5], LOW); 
    digitalWrite(ledPins[6], HIGH); 
   digitalWrite(ledPins[7], HIGH);
  }
  if (millis() - RGBDelay >= RGBSpeed * 4 && millis() - RGBDelay < RGBSpeed * 5){ //Blue
   digitalWrite(ledPins[5], LOW);
   digitalWrite(ledPins[6], LOW); 
    digitalWrite(ledPins[7], HIGH);
  }
  if (millis() - RGBDelay >= RGBSpeed * 5 && millis() - RGBDelay < RGBSpeed * 6){ //Magenta
   digitalWrite(ledPins[5], HIGH);
   digitalWrite(ledPins[6], LOW); 
  digitalWrite(ledPins[7], HIGH);
  }
  if (millis() - RGBDelay >= RGBSpeed * 6 && millis() - RGBDelay < RGBSpeed * 7){ //White
   digitalWrite(ledPins[5], HIGH);
   digitalWrite(ledPins[6], HIGH); 
    digitalWrite(ledPins[7], HIGH);
  }
  if (millis() - RGBDelay >= RGBSpeed * 7){
  RGBDelay = millis();
  }
  //Serial.print(" "); Serial.print(millis() - RGBDelay); //Debugging
  //Serial.print(" "); Serial.println(millis()); //Debugging

//LED functions
  if (functionNo == 0){
    pulse();
  }
  if (functionNo == 1){
    chaser();
  }
  if (millis() - functionDelay > functionSpeed){
    functionNo = functionNo + 1;
    functionDelay = millis();
  }
  if (functionNo >= 2){
    functionNo = 0;
  }
}


void chaser(){ //LEDs come on in turn anticlockwise
  if (millis() - chaserDelay > chaserSpeed){ //adding / volumePercent makes speed contingent on current noise
    digitalWrite(ledPins[chaserOn], HIGH);
    digitalWrite(ledPins[chaserOff], LOW);
    chaserOn = chaserOn + 1;
    chaserOff = chaserOff + 1;
    chaserDelay = millis();
  }
  if (chaserOn == 5){
    chaserOn = 0;
  }
  if (chaserOff == 5){
    chaserOff = 0;
  }
}

void pulse(){
  pulseSpeed = pulseSpeedControl * volumePercent;

  if (pulseState == 0){
  yellBright = yellBright + pulseSpeed;
  }
  if (pulseState == 1){
  yellBright = yellBright - pulseSpeed;
  }
  if (yellBright == 255 || yellBright <= 0){
    pulseState = 1 - pulseState;
  }
  if (yellBright > 255){ //This stop it blinking off at the top end when adding to yellBright skips 255
    yellBright = 254;
    pulseState = 1;
  }
  if (yellBright < 0){ //This stop it blinking on at the low end when subtracting from yellBright skips 0
    yellBright = 1;
    pulseState = 0;
  }
  analogWrite(ledPins[0], yellBright);
  analogWrite(ledPins[1], yellBright);
  analogWrite(ledPins[2], yellBright);
  analogWrite(ledPins[3], yellBright);
  analogWrite(ledPins[4], yellBright);
}