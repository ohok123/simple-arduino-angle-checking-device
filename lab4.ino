#include "Energia.h"
#include "SPI.h"
#define HX8353E
#include "Screen_HX8353E.h"
Screen_HX8353E myScreen;
#define mySerail Serial1


//set up pin for accelerometer
const int xpin = 23;
const int ypin = 24;
const int zpin = 25;

//variables for converted value
float x;
float y;
float z;

//set up interrupt pins
const int S1 = 33;
const int S2 = 32;
const int P1 = PUSH1;

//interrupt flags
int INCRE = 0;
int DECRE = 0;
int EXE = 0;

//variable for execcute flag
int continue_x = 0;
int continue_y = 0;

//variable for without delay
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
long interval = 500;
boolean output_flag = true;

//variable used for store userinput x, y value
int u_x = 0;
int u_y = 0;

//increse interrupt
void incre()
{
  INCRE = 1;
}

//decrease interrupt
void decre()
{
  DECRE = 1;
}

//execution interrupt
void exe()
{
  EXE = 1;
}

void setup()
{
  //set S1,S2,P1 to input
  pinMode(S1,INPUT_PULLUP);
  pinMode(S2,INPUT_PULLUP);
  pinMode(P1,INPUT_PULLUP);
  //set interrupt for buttons
  attachInterrupt(S1,incre, INPUT_PULLUP);
  attachInterrupt(S2,decre, INPUT_PULLUP);
  attachInterrupt(P1,exe, INPUT_PULLUP);
  //LCD  
  myScreen.begin();
  myScreen.setOrientation(0);
  //serial
  Serial.begin(9600);
}

void loop()
{
  //reading accelerometer output
  x = ((int)analogRead(xpin)-512)*45/102;
  y = ((int)analogRead(ypin)-512)*45/102;
  z = ((int)analogRead(zpin)-512)*45/102;
  
  myScreen.gText(4,0,"3-Axis Accelerometer");
  
  myScreen.gText(0,20,"Input x angle:");
  //to eliminate extra digit that remains on screen
  myScreen.gText(85,20,String(u_x+1),blackColour);
  myScreen.gText(85,20,String(u_x-1),blackColour);
  myScreen.gText(85,20,String(u_x));
  
  if (continue_x == 1)
  {
    myScreen.gText(0,30,"Input y angle:");
    myScreen.gText(85,30,String(u_y+1),blackColour);
    myScreen.gText(85,30,String(u_y-1),blackColour);
    myScreen.gText(85,30,String(u_y));
    
    if (continue_x == 1 && continue_y == 1)
    {
        //report without delay
        currentMillis = millis();
        if (currentMillis - previousMillis > interval)
        {
          if (output_flag == true)
          {
            Serial.print(y);
            Serial.println();
            myScreen.gText(0,40,"Current angle(x):");
            myScreen.gText(105,40,String(int(x)+1),blackColour);
            myScreen.gText(105,40,String(int(x)-1),blackColour);
            myScreen.gText(105,40,String(int(x)));
            myScreen.gText(0,50,"Current angle(y):");
            myScreen.gText(105,50,String(int(y)+1),blackColour);
            myScreen.gText(105,50,String(int(y)-1),blackColour);
            myScreen.gText(105,50,String(int(y)));
            
            output_flag = false;
          }
          else
          {
            output_flag = true;
          }
          previousMillis = currentMillis;
        }
        //match up procedure
        if ((u_x >= x - 0.025 * x) && (u_x <= x + x * 0.025) && (u_y >= y - 0.025 * y) && (u_y <= y + y * 0.025))
        {
          //call tone twice to make it work when progress is resetted
          tone(40, 4000, 200);
          tone(40, 4001, 200);
          //LED
          digitalWrite(38, HIGH);
          delay(100);
          digitalWrite(38, LOW);
          delay(100);
          
          myScreen.gText(0, 60, "Matched!");
          myScreen.gText(0, 70, "Reset?");
        }
    }
  }
  
  //x interrupt
  if(INCRE == 1 && continue_x == 0 && continue_y == 0)
  {
    u_x++;
    INCRE = 0;
  }
  if(DECRE == 1 && continue_x == 0 && continue_y == 0)
  {
    u_x--;
    DECRE = 0;
  } 
  if(EXE == 1 && continue_x == 0 && continue_y == 0)
  {
    continue_x = 1;
    EXE =0;
  }
  
  //y interrupt
  if(INCRE == 1 && continue_x == 1 && continue_y == 0)
  {
    u_y++;
    INCRE = 0;
  }
  if(DECRE == 1 && continue_x == 1 && continue_y == 0)
  {
    u_y--;
    DECRE = 0;
  }
  if(EXE == 1 && continue_x == 1 && continue_y == 0)
  {
    continue_y = 1;
    EXE = 0;
  }
  
  //reset
  if(EXE == 1 && continue_x == 1 && continue_y == 1)
  {
    continue_x = 0;
    continue_y = 0; 
    
    previousMillis = 0;
    currentMillis = 0;
    output_flag = true;
    
    u_x = 0;
    u_y = 0;
    
    myScreen.clear();
    EXE = 0;
  }  
}
