
//define different segment codes
static const int SegmentCodes[11][7] = {
//{A,B,C,D,E,F,G}  Segments      7-segment map:
  {1,1,1,1,1,1,0}, // 0   "0"          AAA
  {0,1,1,0,0,0,0}, // 1   "1"         F   B
  {1,1,0,1,1,0,1}, // 2   "2"         F   B
  {1,1,1,1,0,0,1}, // 3   "3"          GGG
  {0,1,1,0,0,1,1}, // 4   "4"         E   C
  {1,0,1,1,0,1,1}, // 5   "5"         E   C
  {1,0,1,1,1,1,1}, // 6   "6"          DDD
  {1,1,1,0,0,0,0}, // 7   "7"
  {1,1,1,1,1,1,1}, // 8   "8"
  {1,1,1,1,0,1,1}, // 9   "9"
  {0,0,0,0,0,0,0}, // OFF   "OFF"
};

//Code for selecting the single digits 
//the Display consits of tree-digits common anode
static const int DigitCodes[3][3] = {
  {1,1,0},
  {0,1,1},
  {1,0,1},
};

//memory of the current displayed segments
//used for refresh
int DigitToPrint[3][7] = {
  {1,0,1,1,1,1,1},
  {1,0,1,1,1,1,1},
  {1,0,1,1,1,1,1},
};

//show "End" on screen
const int displayEND[3][7] = {
  {1,0,0,1,1,1,1},
  {0,0,1,0,1,0,1},
  {0,1,1,1,1,0,1},
};  

//show "---" on screen
const int displayDoorOpen[3][7] = {
  {0,0,0,0,0,0,1},
  {0,0,0,0,0,0,1},
  {0,0,0,0,0,0,1},
};  

//define Arduino Pins the Display is attached to
static const unsigned int SevSegmentPin[] = {6,5,2,1,0,4,3};
static const unsigned int SevDigitPin[] = {14,15,16};

//define constants
const unsigned int NumberOnCycle = 10;    // time a single digit is displayed
const unsigned int NumberDigits = 3;      // number of digits of 7-seg display
const long TimeInterval = 1000;           // time interval for microwave countdown
const int ButtonTimeout = 500;            // debouncing of a very slow button
const int door_open_timeout = 200;        // debouncing of the reed contact

//define pin numbers for IO
const int tone_pin = 18;                  // sound pin, speaker attached to 18 and GND
const int off_pin = 10;                   // pin that holds a transistor high that feeds controller supply current
const int button_pin = 8;                 // pin of the button, also used to bridge the transistor at start
const int reed_pin = 9;                   // reed contect atteched
const int light_pin = 7;                  // microwave light attached

//initialize global variables
unsigned int DigitUpdateIdx = NumberDigits;   // count digit index
unsigned int OnTimeIdx = 0;                   // count on-time of a digit
unsigned long PrevMillis = millis();          // current timing
unsigned long PrevButtonRead = PrevMillis;    // button-read time
unsigned long DoorOpenTime = PrevMillis;      // time of opening door (reed contact)
unsigned int ButtonState = 1;                 // state of the Button
unsigned int CountdownTime = 10;              // countdown time before microwave ends
unsigned int read_button_pin = 1;             // button pin state
unsigned int maximum_countdown_time = 30;     // limit of maximum countdown
bool door_open_flag = false;                  // flag for door opening detection

//refresh the display digit by digit
void refreshDisplay(){

  //count time a single digit is turned on
  OnTimeIdx ++;
  if (OnTimeIdx < NumberOnCycle) return;

  //reset counter
  OnTimeIdx = 0;
  
  //set next digit
  DigitUpdateIdx ++;
  //Wenn alle Digits geschrieben sind, reset Zähler
  if (DigitUpdateIdx >= NumberDigits) DigitUpdateIdx = 0;

  unsigned int i=0;

  //turn off all segments
  for (i=0; i<7; i++){
    digitalWrite(i, LOW);
  }
  //turn off all digits
  for (i=14; i<17; i++){
    digitalWrite(i, LOW);
  }

  //set current digit
  for (i=0; i<3; i++){
    digitalWrite(SevDigitPin[i], DigitCodes[DigitUpdateIdx][i]);
  }

  //write current segment code
  for (i=0; i<7; i++){
    digitalWrite(SevSegmentPin[i], DigitToPrint[DigitUpdateIdx][i]);
  }
}

//write number to print on screen
void setDigitToPrint(int numberToPrintInit){

  int numberToPrint = numberToPrintInit;
  if (numberToPrint > 999) {
    numberToPrint = 999;
  }
  
  int k=0;
  int i=0;
  int n=3;

  //set segments; digit by digit
  for (k=n; k>0; k--){
    int l = numberToPrint % 10;
    for (i=0; i<7; i++){
      DigitToPrint[k-1][i] = SegmentCodes[l][i];
    }        
    numberToPrint /= 10;
  }

  //remove leading zeros of tens-digit
  if (numberToPrintInit < 100) {
    for (i=0; i<7; i++){
      DigitToPrint[0][i] = SegmentCodes[10][i];
    }
  }

  //remove leading zeros of hundrets-digit
  if (numberToPrintInit < 10) {
    for (i=0; i<7; i++){
      DigitToPrint[1][i] = SegmentCodes[10][i];
    }
  }  
}

//end-sequence when microwave is finished
void execBingEnd() {

  //turn off light
  digitalWrite(light_pin, LOW);

  //set "END" on screen
  int k=0;
  int i=0;
  for (k=0; k<3; k++){
    for (i=0; i<7; i++){
      DigitToPrint[k][i] = displayEND[k][i];
    }        
  }
  
  int numberBingTones = 4;
  int bing = 1865;              // sound frequency
  int bing_pause = 100;         // pause between sounds
  int bing_duration = 50;       // sound duration
  int bing_duration_2 = 100;    // sound duration 2
  
  // flag parameter
  bool exitBing = false;
  bool toneFlag = true;

  // timings
  unsigned long prevTime = millis();
  unsigned long currentTime = millis();

  // start tone
  tone(tone_pin, bing);
  
  // execute sounds one after the other
  while (exitBing == false) {

    refreshDisplay();
    currentTime = millis();
    
    if (currentTime - prevTime >= bing_duration) {

      if (toneFlag == true) {
        noTone(tone_pin);
        toneFlag = false;
        numberBingTones --;
      } 
      else {
        tone(tone_pin, bing);
        toneFlag = true;
      }
      
      prevTime = currentTime;
      exitBing = (numberBingTones <= 0) ? true : false;      
    }  
  }

  // break after bing has finished
  prevTime = currentTime;
  while (currentTime - prevTime <= 10*bing_duration_2) {
    currentTime = millis();
    refreshDisplay();
  }

}

void execBingDoorOpen() {

  digitalWrite(light_pin, LOW);

  //setze "END" als Display-Anzeige
  int k=0;
  int i=0;
  for (k=0; k<3; k++){
    for (i=0; i<7; i++){
      DigitToPrint[k][i] = displayDoorOpen[k][i];
    }        
  }
  
  int numberBingTones = 5;
  int bing = 932;
  int bing_pause = 80;
  int bing_duration = 40;
  int bing_duration_2 = 100;
  bool exitBing = false;
  bool toneFlag = true;
  unsigned long prevTime = millis();
  unsigned long currentTime = millis();
  
  tone(tone_pin, bing);

  while (exitBing == false) {

    refreshDisplay();
    currentTime = millis();
    
    if (currentTime - prevTime >= bing_duration) {

      if (toneFlag == true) {
        noTone(tone_pin);
        toneFlag = false;
        numberBingTones --;
      } 
      else {
        tone(tone_pin, bing);
        toneFlag = true;
      }
      
      prevTime = currentTime;
      exitBing = (numberBingTones <= 0) ? true : false;      
    }  
  }
  
  prevTime = currentTime;
  while (currentTime - prevTime <= 2*bing_duration_2) {
    currentTime = millis();
    refreshDisplay();
  }

}

void setup() {
  // put your setup code here, to run once:

  unsigned long start_time = millis();
  unsigned char i;

  pinMode(button_pin, INPUT);
  pinMode(reed_pin, INPUT);

  pinMode(off_pin, OUTPUT);  
  digitalWrite(off_pin, HIGH);

  pinMode(light_pin, OUTPUT);
  digitalWrite(light_pin, HIGH);
  
  //schalte alle Segments aus
  for (i=0; i<7; i++){
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
  //schalte alle Digits aus
  for (i=14; i<17; i++){
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }

  if (digitalRead(reed_pin) == LOW){
    execBingDoorOpen();
    bool hold_operation = true;
    while (hold_operation == true) {
      digitalWrite(off_pin, LOW);
      refreshDisplay();
    }
  }

  tone(tone_pin, 58);

  setDigitToPrint(CountdownTime);

}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long CurrentMillis = millis();
  
  read_button_pin = digitalRead(button_pin);
  if (read_button_pin == LOW && CurrentMillis - PrevButtonRead > ButtonTimeout) {
    if (CountdownTime <= maximum_countdown_time) {
      CountdownTime += 5;
      PrevButtonRead = CurrentMillis;
    }
  }

  if (door_open_flag == true) {
    
    if (CurrentMillis - DoorOpenTime >= door_open_timeout){
      execBingEnd();
      bool hold_operation = true;
      while (hold_operation == true) {
        digitalWrite(off_pin, LOW);
        refreshDisplay();
      }
    }
    else if (digitalRead(reed_pin) == HIGH){
      door_open_flag = false;
    }
  }
  else if (digitalRead(reed_pin) == LOW && door_open_flag == false){
    door_open_flag = true;
    DoorOpenTime = CurrentMillis;
  }

  
  if (CurrentMillis - PrevMillis >= TimeInterval && CurrentMillis - PrevButtonRead > ButtonTimeout) {
    
    PrevMillis = CurrentMillis;
    CountdownTime--;
    setDigitToPrint(CountdownTime);
    
    if (CountdownTime <= 0) {
      execBingEnd();
      digitalWrite(off_pin, LOW);
      CountdownTime = 999;
    }
  }
  
  refreshDisplay();

}
