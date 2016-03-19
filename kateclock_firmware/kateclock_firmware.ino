
/*
 * Kate Arduino-powered Digital Clock
 * Firmware v.1.00
 * Coded by: John Espiritu
 * Powered by: Arduino
 * 
 * Created November 21, 2015 
 * 
 */

 //imports
 #include <avr/sleep.h>
 #include <avr/power.h>
 #include <avr/wdt.h>

//Seven segment Demultiplexer.
int segment[10][4] = {
  {0, 0, 0, 0}, //1
  {0, 0, 0, 1}, //2
  {0, 0, 1, 0}, //3
  {0, 0, 1, 1}, //4
  {0, 1, 0, 0}, //5
  {0, 1, 0, 1}, //6
  {0, 1, 1, 0}, //7
  {0, 1, 1, 1}, //8
  {1, 0, 0, 0}, //9
  {1, 0, 0, 1}, //10
};

//Character arrays
int digits[10][7] = {
  {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, LOW},
  {0, 1, 1, 0, 0, 0, 0},
  {1, 1, 0, 1, 1, 0, 1},
  {1, 1, 1, 1, 0, 0, 1},
  {0, 1, 1, 0, 0, 1, 1},
  {1, 0, 1, 1, 0, 1, 1},
  {1, 0, 1, 1, 1, 1, 1},
  {1, 1, 1, 0, 0, 0, 0},
  {1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 0, 1, 1}
};

//Catherine
int kate[10][7] = {
  {1, 0, 0, 1, 1, 1, 0}, //c
  {1, 1, 1, 1, 1, 0, 1}, //a
  {0, 0, 0, 1 ,1 ,1 ,1}, //t
  {0, 0, 1, 0, 1 ,1 ,1}, //h
  {1, 1, 0, 1, 1, 1, 1}, //e
  {1, 0, 0, 0, 1, 1, 0}, //r
  {0, 1, 1, 0, 0, 0, 0}, //i
  {1, 1, 1, 0, 1 ,1 ,0}, //n
  {1, 1, 0, 1, 1, 1, 1}, //e
  {0, 0, 0, 0, 0, 0, 0} //Blank
};

//Alphabet characters
/*
 * a b c d e f g h i j l n o p q r s t u y z
 */
int alpha[23][7] {
// a  b  c  d  e  f  g
  {1, 1, 1, 0, 1, 1, 1}, //a
  {0, 0, 1, 1, 1, 1, 1}, //b
  {1, 0, 0, 1, 1, 1, 0}, //c
  {0, 1, 1, 1, 1, 0, 1}, //d
  {1, 0, 0, 1, 1, 1, 1}, //e
  {1, 0, 0, 0, 1, 1, 1}, //f
  {1, 0, 1, 1, 1, 1, 0}, //g
  {0, 0, 1, 0, 1, 1, 1}, //h
  {0, 0, 0, 0, 1, 1, 0}, //i
  {0, 1, 1, 1, 1, 0, 0}, //j
  {0, 0, 0, 1, 1, 1, 0}, //l
  {1, 1, 1, 0, 1, 1, 0}, //n
  {1, 1, 1, 1, 1, 1, 0}, //o
  {1, 1, 0, 0, 1, 1, 1}, //p
  {1, 1, 1, 0, 0, 1, 1}, //q
  {1, 1, 0, 0, 1, 1, 0}, //r
  {1, 0, 1, 1, 0, 1, 1}, //s
  {0, 0, 0, 1, 1, 1, 1}, //t
  {0, 1, 1, 1, 1, 1, 0}, //u
  {0, 1, 1, 1, 0, 1, 1}, //y
  {0, 0, 0, 0, 0, 0, 0},  //__
  {1, 0, 0, 1, 0, 0, 0},  //=
  {1, 1, 1, 1, 0, 0, 0},  //)
};

//Startup message array
String messages[] = {
  "Hellooo =)",
  "Hi cutie",
  "Hiiii",
  "Good day",
  "Hi there",
  "Hi Hello",
  "Cathe Chan",
  "=)",
  "C========)",
  "Starting",
  "Helloo",
  "Hi =)",
  "= = = = = ",
  " = = = = =",
  "Hi pretty",
  "Loading"
};

//Display Array
int disp[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

//Date variables.
int year = 2015;
int month = 12;
int day = 21;

//Time variables
int hour = 21;
int mins = 6;
int sec = 0;

//Time state variables

//Prev states
unsigned long prevTick = 0;
unsigned long prevTime = 0;
unsigned long prevBlink = 0;
unsigned long prevSetBlink = 0;
//Current states
unsigned long currTick = 0;
unsigned long currBlink = 0;
unsigned long currTime = 0;
unsigned long currSetBlink = 0;

//Define clockmode
volatile int ClockMode = 0;
//Clock modes:
//0 - Time
//1 - Set Time
//2 - Display Kate
//3 - Show message.

//Welcome message members.
bool welcome_msg = false;
bool welcome_msg_done = false;
int welcome_msg_id = 0;

//Variables for multiplex state.
int ctr = 0;
int z = 0;
int aa = 0;
int currSeg = 0;

//State if kate will be shown.
bool showKate = false;

//Button states.
//Pressed states
int B1_PSTATE = 0;
int B2_PSTATE = 0;
int B3_PSTATE = 0;

//Depress counter.
long B1_PTIME = 0;
long B2_PTIME = 0;
long B3_PTIME = 0;

//Seconds blinker states.
int blinkstate = 0;
int blinkstate_setmode = 0;

//Mains state.
int MAINS_STATE = 0;

//Set Mode
int SetClock  = 0;
//0 = Month
//1 = Day
//2 = Year
//3 = Hr
//4 = Min

//sleep state.
volatile bool sleeping = false;

//Watchdoge Interrupt.
ISR(WDT_vect){
  //set flags.
  if(sleeping == true) {
    sleeping = false;
  }
  wdt_disable();  //disable watchdoge.
}

//Sleep the arduino.
void standby(){
  
  // The ATmega328 has five different sleep states.
  // See the ATmega 328 datasheet for more information.
  // SLEEP_MODE_IDLE -the least power savings 
  // SLEEP_MODE_ADC
  // SLEEP_MODE_PWR_SAVE
  // SLEEP_MODE_STANDBY
  // SLEEP_MODE_PWR_DOWN -the most power savings
  // I am using the deepest sleep mode from which a
  // watchdog timer interrupt can wake the ATMega328
  set_sleep_mode(SLEEP_MODE_STANDBY); //Set sleep mode.
  cli(); //clear/prevent interrupts.

  sleep_enable(); //enable sleep.

  //Set flags.
  ADCSRA = 0;           //Disable ADC

  //turn off brown-out
  MCUCR = bit (BODS) | bit (BODSE);
  MCUCR = bit (BODS); 

  /*
  //disable some modules:
  power_adc_disable();  //disable ADC converter.
  power_spi_disable();   //disable SPi
  power_usart0_disable(); //disable serial

  //Do nothing with timer 0 and timer 1 because we'll use them, baby.
 
  power_timer2_disable(); //disable timer2
  power_twi_disable();  //disable TWI
  */

  //Setup watchdoge.
  //wow.
  MCUSR = 0;  //clear "reset" flags.
  WDTCSR = bit(WDCE) | bit(WDE);
  WDTCSR = bit(WDIE) | bit(WDP2) | bit(WDP0); //Setup WDT interrupt, 500ms delay.
  wdt_reset();  //pat doge. wow.

  //Start sleeping.
  sleeping = true;  //set flag.
  sei();        //allow interrupts.
  sleep_cpu();  //sleep within the specified duration.

  /* The program will continue here after the WDT timeout  */
  
  //cancel sleep as a precaution.
  sleep_disable();

  //power_all_enable(); //power all peripherals.
  
  sei(); //set/allow interrupts.
}

//Run when arduino boots or preparing.
void setup() {

  //Set pins 0 - 13 as output.
  //pins 0 - 6 (segments) 7 - 11 (digit multiplex)
  for(int i = 0; i <=13; i++){
    pinMode(i, OUTPUT);
  }

  //B1 = Mode Button
  pinMode(A5, INPUT);

  //B2 = SEL Button
  pinMode(A4, INPUT);

  //B3 = SET Button
  pinMode(A3, INPUT);

  //MAINS trigger
  pinMode(A2, INPUT);

  //Get current time.
  prevTime = micros();
  prevTick = millis();
  prevBlink = millis();
}

bool sleeped = false;

void loop() {
  
  //Detect main line switch.
  MAINS_STATE = digitalRead(A2);
  
  if(MAINS_STATE == LOW){    
    //Get if mains is not active.
    
    //Turn of blinkers.
    blinkstate = false;
    blinkLights(blinkstate);
    
    standby();  //simply standby, to remember last time.
    
  } else if(MAINS_STATE == HIGH){
    //If mains is active.
    
    //Tick the clock.
    currTick = micros();
    if((long)(currTick - prevTick) >= 0){
        //prevTick = currTick;
        prevTick = (unsigned long)(prevTick + (long)1000000);
        runTime();
    }

    //Message timeout
    if(welcome_msg_done == false){
      if((long)(millis()>= 2000)){
        ClockMode = 0;
        welcome_msg_done = true;
      }
    }


    //Read button inputs.
    int B1_STATE = digitalRead(A5);
    int B2_STATE = digitalRead(A4);
    int B3_STATE = digitalRead(A3);
    currTime = millis();
  
    //MODE SELECT
    if(B1_STATE == HIGH){
      if(B1_STATE != B1_PSTATE){
        if((unsigned long)((unsigned long)currTime - (unsigned long)B1_PTIME) >= (unsigned long)500){ //Prevent very quick press.
          ClockMode++;
          if(ClockMode > 2) ClockMode = 0;
          if(ClockMode == 1) SetClock = 0;
          B1_PTIME = currTime;
          B1_PSTATE = B1_STATE;
        }
      }
    }
  
    //SELECT BUTTON
    if(B2_STATE == HIGH){
      if(B2_STATE != B2_PSTATE){
        if((unsigned long)(unsigned long)(currTime - (unsigned long)B2_PTIME) >= (unsigned long)500){
          if(ClockMode == 1){
            SetClock++;
            if(SetClock > 4) SetClock = 0;
            B2_PTIME = currTime;
            B2_PSTATE = B2_STATE;
          }
        }
      }
    }
  
    //SET BUTTON
    if(B3_STATE == HIGH){
      if(B3_STATE != B3_PSTATE){
        if((unsigned long)((unsigned long)currTime - (unsigned long)B3_PTIME) >= (unsigned long)500l){
          if(ClockMode == 1){
  
            //Set month.
            if(SetClock == 0){
              month++;
              if(month > 12) month = 1;
            }
  
            //Set day.
            if(SetClock == 1){
              if(month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12){
                //Reset when > 31
                if(day < 31){
                  day++;
                } else{
                  day = 1;
                }
              } else if(month == 2){
                if(year % 4 == 0){
                  if(day < 29){
                    //Leap ear
                    day++;
                  } else{
                    day = 1;
                  }
                } else{
                  if(day < 28){
                    //Not Leap year
                    day++;
                  } else{
                    day = 1;
                  }
                }
              } else{
                //Reset when > 30
                if(day < 30){
                  day++;
                } else{
                  day = 1;
                }
              }
            }
  
            //Set year.
            if(SetClock == 2){
              year++;
              if(year > 2099) year = 2000;
            }
  
            //Set hours.
            if(SetClock == 3){
              hour++;
              if(hour > 23) hour = 0;
            }
  
            //Set minutes.
            if(SetClock == 4){
              mins++;
              if(mins > 59) mins = 0;
            }
            
            B3_PTIME = currTime;
            B3_PSTATE = B3_STATE;
          }
        }
      }
    }
    
    B1_PSTATE = B1_STATE;
    B2_PSTATE = B2_STATE;
    B3_PSTATE = B3_STATE;
   
    //Multiplex display
    currTime = micros();
    if((unsigned long)((unsigned long)currTime - (unsigned long)prevTime) >=  1400){
      z++;
      aa++;
      prevTime = currTime;
      clearOutput();
    }
  
    //Blink blinkers.
    currBlink = millis();
    //if((unsigned long)((unsigned long)currBlink - (unsigned long)prevBlink) > 0){
    if((long)((long)currBlink - (long)prevBlink) >= 0){
      //blinkLights();
      blinkstate = !blinkstate;
      //prevBlink = currBlink;
      prevBlink = (long)(prevBlink + 1000l);
      if(showKate == false)displayTime();
    }
  
    //Blink selected SET values.
    currSetBlink = millis();
    if((unsigned long)((unsigned long)currSetBlink - (unsigned long)prevSetBlink) >= (unsigned long)250l){
      prevSetBlink = currSetBlink;
      blinkstate_setmode = !blinkstate_setmode;
    }
  
    //Set if dots and blinker is active. It should only be active during TIME and SET TIME mode.
    if(ClockMode == 0 || ClockMode == 1) digitalWrite(13, LOW);
    else digitalWrite(13, HIGH);
    
    if(z > 9) {
      z = 0;
    }
    
    if(aa > 9){
      aa = 0;
    }
  
    blinkLights(blinkstate);
  
    //Switch segment when timeout lapsed.
    if(z != currSeg){
      selectSegment(z);
      currSeg = z;
    }
  
    if(welcome_msg == false){
      //Display a welcome screen on start.
      welcome_msg = true;
      randomSeed(analogRead(0) + analogRead(1) + analogRead(2) + analogRead(3) + analogRead(4));
      welcome_msg_id = random(16);
      ClockMode = 3;
    }
    
  
    if(ClockMode == 2){ //Mode display kate.
      displayKate();
      outputKate(disp[aa]);
    } else if(ClockMode == 3){ //Display message on screen.
      setMessage(messages[welcome_msg_id]);
      displayMessage(disp[aa]);
    } else if(ClockMode == 1){
      displayTime();
      
      //Flash Month
      if(SetClock == 0){
        if (!(aa == 0 || aa == 1) || blinkstate_setmode == 0) outputNumber(disp[aa]);
      }
  
      //Flash Day
      if(SetClock == 1){
        if (!(aa == 2 || aa == 3) || blinkstate_setmode == 0) outputNumber(disp[aa]);
      }
  
      //Flash Year
      if(SetClock == 2){
        if (!(aa == 4 || aa == 5) || blinkstate_setmode == 0) outputNumber(disp[aa]);
      }
  
      //Flash Hour
      if(SetClock == 3){
        if (!(aa == 6 || aa == 7) || blinkstate_setmode == 0) outputNumber(disp[aa]);
      }
  
      //Flash Min
      if(SetClock == 4){
        if (!(aa == 8 || aa == 9) || blinkstate_setmode == 0) outputNumber(disp[aa]);
      }
      
    }
    else{
      displayTime();
      outputNumber(disp[aa]);
    }
  } else{
    for(int i = 0; i < 13; i++){
      digitalWrite(i, LOW);
    }
  }
}

//Display a message on the display.
void setMessage(String msg){
 for(int i = 0; i < 10; i++){
    disp[i] = 20;
 }
 int c = 0;
 for(int i = 0; i < msg.length(); i++){
  if(i < 10){
    int m = 0; //output index.
    if((char)toUpperCase(msg.charAt(i)) == 'A') m = 0;
    else if((char)toUpperCase(msg.charAt(i)) == 'B') m = 1;
    else if((char)toUpperCase(msg.charAt(i)) == 'C') m = 2;
    else if((char)toUpperCase(msg.charAt(i)) == 'D') m = 3;
    else if((char)toUpperCase(msg.charAt(i)) == 'E') m = 4;
    else if((char)toUpperCase(msg.charAt(i)) == 'F') m = 5;
    else if((char)toUpperCase(msg.charAt(i)) == 'G') m = 6;
    else if((char)toUpperCase(msg.charAt(i)) == 'H') m = 7;
    else if((char)toUpperCase(msg.charAt(i)) == 'I') m = 8;
    else if((char)toUpperCase(msg.charAt(i)) == 'J') m = 9;
    else if((char)toUpperCase(msg.charAt(i)) == 'L') m = 10;
    else if((char)toUpperCase(msg.charAt(i)) == 'N') m = 11;
    else if((char)toUpperCase(msg.charAt(i)) == 'O') m = 12;
    else if((char)toUpperCase(msg.charAt(i)) == 'P') m = 13;
    else if((char)toUpperCase(msg.charAt(i)) == 'Q') m = 14;
    else if((char)toUpperCase(msg.charAt(i)) == 'R') m = 15;
    else if((char)toUpperCase(msg.charAt(i)) == 'S') m = 16;
    else if((char)toUpperCase(msg.charAt(i)) == 'T') m = 17;
    else if((char)toUpperCase(msg.charAt(i)) == 'U') m = 18;
    else if((char)toUpperCase(msg.charAt(i)) == 'Y') m = 19;
    else if(msg.charAt(i) == ' ') m = 20;
    else if(msg.charAt(i) == '=') m = 21;
    else if(msg.charAt(i) == ')') m = 22;
    else m = 20;
    disp[c] = m;
    c++;
  }
 }

}

//Display the message on the display
void displayMessage(int lettr){
  int n = 0;
  for(int i = 0; i <= 6; i++){
     digitalWrite(i, !alpha[lettr][n]);
     n++;
  }
}

//Display "Catherine" on display.
void displayKate(){
  for(int i = 0; i < 10; i++){
    disp[i] = i;
  }
}

//Forwards the current date/time to the display array.
void displayTime(){
  //Set month on array 0 and 1.
  disp[0] = month / 10;
  disp[1] = month % 10;

  //Set day on array 2 and 3.
  disp[2] = day / 10;
  disp[3] = day % 10;

  //Set year on array 4 and 5
  int yy = (year % 1000) % 100;
  disp[4] = yy / 10;
  disp[5] = yy % 10;

  //Set hr on array 6 and 7
  disp[6] = hour / 10;
  disp[7] = hour % 10;

  //Set min on array 8 and 9.
  disp[8] = mins / 10;
  disp[9] = mins % 10;
}

//The main time updater.
void runTime(){
  sec++;  //Increment seconds.

  //If seconds >= 60, add minutes, then reset seconds.
  if(sec >= 60){
    sec = 0;
    mins++;
  }

  //If min >= 60, add hours, then reset minuts.
  if(mins >= 60){
    mins = 0;
    hour++;
  }

  //If hour >= 24, add days
  if(hour >= 24){
    day++;
    hour = 0;
  }

  //Increment days. If months are, Jan, Mar, May, Jul, Aug, Oct, Dec = up to 31, else up to 30 only. Special case = Feb (Leap Year 29, else 28
  if(month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12){
    //Reset when > 31
    if(day > 31){
      day = 0;
      month++;
    }
  } else if(month == 2){
    if(year % 4 == 0){
      if(day > 29){
        //Reset on 29 (leap year)
        day = 0;
        month++;
      }
    } else{
      if(day > 28){
        //Reset on 28 (not leap year)
        day = 0;
        month++;
      }
    }
  } else{
    //Reset when > 30
    if(day > 30){
      day = 0;
      month++;
    }
  }

  //Increment month.
  if(month > 12){
    month = 1;
    year++;
  }
}

void blinkLights(int val){
  digitalWrite(12, val);
}

void addAll(){
  for(int i = 0; i < 10; i++){
    disp[i] = disp[i] + 1;
    if(disp[i] > 9) disp[i] = 0;
  }
}

void outputKate(int letter){
  int n = 0;
  for(int i = 0; i <= 6; i++){
     digitalWrite(i, !kate[letter][n]);
     n++;
  }
}

void outputNumber(int num){
  int n = 0;
  for(int i = 0; i <= 6; i++){
     digitalWrite(i, !digits[num][n]);
     n++;
  }
}

void clearOutput(){
  for(int i = 0; i <= 11; i++){
    digitalWrite(i, HIGH);
  }
}

void selectSegment(int index){
  int n = 0;
  digitalWrite(7, HIGH);
  for(int i = 8; i <= 11; i++){
    digitalWrite(i, segment[index][n]);
    n++;
  }
}



