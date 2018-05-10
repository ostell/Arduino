//Test security system// beta 2.0

/////////////////////////////////////////////////////////////////
#include <LiquidCrystal.h>
#include <Password.h>
#include <Keypad.h> 


//Password
Password password = Password( "1234" );

const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns

char keys[ROWS][COLS] = { // Define the Keymap
  {
    '1','2','3','A'      }
  ,
  {
    '4','5','6','B'      }
  ,
  {
    '7','8','9','C'      }
  ,
  {
    '*','0','#','D'      }
};

byte rowPins[ROWS] = {6, 7, 8, 9};     //connect to the row pinouts of the keypad
byte colPins[COLS] = {A0, A1, A2, A3};     //connect to the column pinouts of the keypad

// Create the Keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12
, 11, 5, 4, 3, 2); // Assignign arduino pins to LCD display module



//constants for LEDs, inputs and outputs
int pirPin1 = A5;
int reedPin1 = 10;
int reedPin2 = A4;
int speakerPin = 13; 
int passwd_pos = 11;
int alarmStatus = 0;
int zone = 0;
int alarmActive = 0;

void setup(){
  Serial.begin(9600);
  lcd.begin(16, 2);
  
  displayCodeEntryScreen();
   
  pinMode(pirPin1, INPUT);
  pinMode(reedPin1, INPUT);
  pinMode(reedPin2, INPUT);

  digitalWrite(speakerPin, LOW); //Buzzer off
 
  keypad.addEventListener(keypadEvent); //Event listener
}

void loop(){
  
  keypad.getKey();

  if (alarmActive == 1){ 
    if (analogRead(pirPin1) > 700)
    {
      zone = 3;
      alarmTriggered();
    }
    else if (digitalRead(reedPin1) == LOW)
    {
      zone = 1;
      alarmTriggered();
    }
    else if (analogRead(pirPin2) > 700)
    {
      zone = 2;
      alarmTriggered();
    }
    
   }
}

/////////////////////////  Functions  /////////////////////////////////
//take care of some special events
///////////////////////////////////////////////////////////////////////
void keypadEvent(KeypadEvent eKey){
  switch (keypad.getState()){
  case PRESSED:
    Serial.print("Pressed: ");
    Serial.println(eKey);
     if (passwd_pos - 12 >= 5) { 
      return ;
    }
    lcd.setCursor((passwd_pos++),0);
    switch (eKey){
    case '#':                 //# is to validate password 
      passwd_pos  = 12;
      checkPassword(); 
      break;
    case '*':                 //* is to reset password attempt
      password.reset(); 
      passwd_pos = 12;
   // TODO: clear the screen output 
      break;
    default: 
      password.append(eKey);
      lcd.print("*");
    }
  }
}

///////////////////////////////////////////////////////////////////////
void alarmTriggered(){
  digitalWrite(speakerPin, HIGH);
  siren();
  password.reset();
  alarmStatus = 1;
  alarmActive = 0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("SYSTEM TRIGGERED");
  Serial.println("SYSTEM TRIGGERED");
  lcd.setCursor(0,1);

  if (zone == 1)
  { 
    lcd.print("Corridor!");
    Serial.println("Corridor!");
    delay(1000);
  }
  if(zone == 2){
    lcd.print("Liv_room!");
    Serial.println("Liv room!");
    delay(1000);
  }  
  if(zone == 3){
    lcd.print("Basement!");
    Serial.println("Basement!");
    delay(1000);
  }
}

//////////////////////////////////////////////////////////////////////                                                    
void checkPassword(){                  //Check if PIN is corrected, if not, retry!
  if (password.evaluate())
  {  
    if(alarmActive == 0 && alarmStatus == 0)
    {
      activate();
    } 
    else if( alarmActive == 1 || alarmStatus == 1) 
    {
      deactivate(); 
    }
  } 
  else {
    invalidCode();
  }
}
 
/////////////////////////////////////////////////////////////////////
void invalidCode()   //Display meaasge when a invalid is entered
{
  password.reset();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("INVALID CODE!");
  Serial.println("inavlid code");
  lcd.setCursor(0,1);
  lcd.print("TRY AGAIN!");
  Serial.println("try again");

  displayCodeEntryScreen();
  alarmTriggered();

}

////////////////////////////////////////////////////////////////////
void activate()      // Activate the system if correct PIN entered and display message on the screen
{
  if(digitalRead(reedPin1) == HIGH && analogRead(reedPin2) == 0 && analogRead(pirPin1) == 0)
  { 
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("SYSTEM");
    lcd.setCursor(0,1);
    lcd.print("ACTIVATED!");
    Serial.println("system active"); 
    alarmActive = 1;
    password.reset();
    delay(2000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Home Security");
    lcd.setCursor(0,1);
    lcd.print("System");
  }
  else{
    deactivate(); }
}

/////////////////////////////////////////////////////////////////////
void deactivate()
{
  alarmStatus = 0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("SYSTEM");
  lcd.setCursor(0,1);
  lcd.print("DEACTIVATED!");
  Serial.println("system deactivated");
  digitalWrite(speakerPin, LOW);
  siren();
  alarmActive = 0;
  password.reset();
  delay(5000);

  displayCodeEntryScreen();
}

//////////////////////////////////////////////////////////////////////
void displayCodeEntryScreen()    // Dispalying start screen for users to enter PIN
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Enter PIN:");
  Serial.println("Enter pin");
  lcd.setCursor(0,1);
  lcd.print("Home Sec System");
  Serial.println("home sec system");
  

}

///////////////////////////////////////////////////////////////////
void siren()
{
 int i;
 int n=300;
 for(i=0;n>i;i++)
  {
  digitalWrite(speakerPin, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1);              // wait for a second
  digitalWrite(speakerPin, LOW);    // turn the LED off by making the voltage LOW
  delay(1);
  }
  delay(200);
}

