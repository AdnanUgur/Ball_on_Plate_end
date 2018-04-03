#include <Servo.h>
#include <stdint.h>
#include "TouchScreen.h"
#include "writereadfast.h"
#include <PID_v1.h>
#include <stdio.h>
/**YILAN OYUNU DEFİNE START***/
/*  Direnç tarafı + olucak.
  - taraf 40 , 41 , 42 , 43
  MUX PİN A , B , C , D

  + taraf  30 , 31 , 32 , 33
  MUX PİN  A , B , C  , D
*/
/*COMMUNUCATION STAFS*/
#define DEFAULTMODE 'C'
#define GAMEMODE 'L'
#define SQUAREMODE 'S'
#define DIAMONDMODE 'O'
#define ENDINTERRACT 'F'
#define READYSEND "R"
#define NEXT 'N'

char mesageData[25];
/* END */
int ready = 0;
int stat;
char modexx = DEFAULTMODE;
#define BOUND 10
#define STABLE_THRESHOLD 10

#define YP A2     // must be an analog pin, use "An" notation!
#define XM A3     // must be an analog pin, use "An" notation!
#define YM 8      // can be a digital pin
#define XP 9      // can be a digital pin
#define XServoPin 7  // X Servo Pin
#define YServoPin 6  // Y Servo Pin


/*** GAME STAFS ***/
#define del 1 //Delay For LED Blink

int Led_Input_X = 0; //+ Input
int Led_Input_Y = 0; //- Input
int yem[3][2];
int yem_led[3][2];

// MUX Bacaklari
int led30 = 30; //+
int led31 = 31;
int led32 = 32;
int led33 = 33;

int led40 = 40; //-
int led41 = 41;
int led42 = 42;
int led43 = 43;

int Gelen_X = 0;
int Gelen_Y = 0;
long rand_X = 0;
long rand_Y = 0;
//////**YILAN OYUNU DEFINE END***////

/*** DRAW SQUARE STAFS****/
#define SQKOSE1X
#define SQKOSE1Y

#define SQKOSE2X
#define SQKOSE2Y

#define SQKOSE3X
#define SQKOSE3Y

#define SQKOSE4X
#define SQKOSE4Y
/*** END DRAW SQUARE ***/

/*** DRAW DIAMOND STAFS***/
#define DIKOSE1X
#define DIKOSE1Y

#define DIKOSE1X
#define DIKOSE1Y

#define DIKOSE1X
#define DIKOSE1Y

#define DIKOSE1X
#define DIKOSE1Y
/*** END DRAW DIAMOND ***/

/*** DEFAULT STAFFS ***/
#define DEFAULTPOINTX 200
#define DEFAULTPOINTY 150
/*** END DEFAULT ***/

int mode = 0; // mode 0 default = merkez, 1 = snake game, 2 = square,3 = DIAMOND

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 10);

Servo xServo;
Servo yServo;
unsigned int stable = 0;
unsigned int noTouchCount = 0;

double SetpointX = DEFAULTPOINTX, InputX = 200 , OutputX = 80;
//double xKp = 0.38  , xKd = 0.152 , xKi = 4*xKd;
//double xKp = 0.6  , xKd = 0.178 , xKi = 4*xKd;  //son hal
double xKp = 0.6  , xKd = 0.178 , xKi = 0.53;

double SetpointY = DEFAULTPOINTY, InputY = 150 , OutputY = 80;
//double yKp =0.29, yKd = 0.055 , yKi = 4*yKd;
//double yKp =0.42, yKd = 0.065 , yKi = 4*yKd;  //son hal
double yKp = 0.42, yKd = 0.065 , yKi = 0.20;

PID xPID(&InputX, &OutputX, &SetpointX, xKp, xKi, xKd, DIRECT);
PID yPID(&InputY, &OutputY, &SetpointY, yKp, yKi, yKd, DIRECT);

int sample = 5 ; //Sample Time

void setup(void) {

  xServo.attach(XServoPin);
  xServo.write(80);
  yServo.attach(YServoPin);
  yServo.write(80);

  xPID.SetMode(AUTOMATIC);
  yPID.SetMode(AUTOMATIC);
  xPID.SetSampleTime(sample);
  xPID.SetOutputLimits(0, 400);
  yPID.SetSampleTime(sample);
  yPID.SetOutputLimits(0, 300);

  // Snake game led initial:
  pinMode(led30, OUTPUT); //+  //oyun init olduğu için o if bloguna al
  pinMode(led31, OUTPUT);
  pinMode(led32, OUTPUT);
  pinMode(led33, OUTPUT);

  pinMode(led40, OUTPUT); //-
  pinMode(led41, OUTPUT);
  pinMode(led42, OUTPUT);
  pinMode(led43, OUTPUT);
  // led init end

  Serial.begin(9600);
  delay(3000);
  Led_one(11, 14);


}

void loop(void) {

  /*if(Save_Yem() == true)
    {
    Led_one(yem_led[0][1]-1,yem_led[0][0]-1);
    Led_one(yem_led[1][1]-1,yem_led[1][0]-1);
    Led_one(yem_led[2][1]-1,yem_led[2][0]-1);
    /*}*/
  int updateKp, updateKi, updateKd;
  if (Serial.available()) {
    int inByte = Serial.read();
    char dead;
    if (inByte == 'S') // check hand shake
    {
      Serial.write("R");
    }
    else if (inByte == 'X') {
      dead = Serial.read();
      updateKp = Serial.read() - '0';
      dead = Serial.read();
      updateKi = Serial.read() - '0';
      dead = Serial.read();
      updateKd = Serial.read() - '0';
      xPID.SetTunings(updateKp, updateKi, updateKd);
    }
    else if (inByte == 'Y') {
      dead = Serial.read();
      updateKp = Serial.read() - '0';
      dead = Serial.read();
      updateKi = Serial.read() - '0';
      dead = Serial.read();
      updateKd = Serial.read() - '0';
      yPID.SetTunings(updateKp, updateKi, updateKd);
    }
    else if (inByte == 'M') {
      dead = Serial.read();
      inByte = Serial.read();
      modexx = inByte;
      //ERROR
    }
  }


  switch (modexx) {
    case DEFAULTMODE:
      //Led_3x3(1,1);
    SetpointX = DEFAULTPOINTX;
    SetpointY = DEFAULTPOINTY;
    go_center();
      break;
    case GAMEMODE:
      //Led_3x3(10,1);
    game_fonk();
    modexx = DEFAULTMODE;
      break;
    case SQUAREMODE:
      //Led_3x3(1, 10);
      draw_square();
    modexx = DEFAULTMODE;
      break;
    case DIAMONDMODE:
      //Led_3x3(10, 10);
      draw_Diamond();
    modexx = DEFAULTMODE;
      break;
  }
}
/*MAIN FUNCTION FOR LOOP*/




//Set POINTLER AYARLANACAK
int draw_Diamond() {
  int done = 2;
  int points[8] = {190, 50,
                    90, 140,
                    200, 234,
                    314, 162
                   };
  SetpointX = points[0];
    SetpointY = points[1];
  while (done < 8) {

  stable = 0;
    while (!Serial.available() && stable < 2)
    {
      TSPoint p = ts.getPoint();
      if ( p.x != 0 && p.y != 1023 ) // Ball is on plate
      {
        xServo.attach(XServoPin);
        yServo.attach(YServoPin);

        InputX = map(p.x, 125, 965, 0, 400);  ///uzun kenar Xx
        InputY = map(p.y, 130, 910, 300, 0);

        Led_Input_X =  map(InputX, 0, 400, 0, 13); //*     !!!!!!Burda map edim ters olabilir
        Led_Input_Y =  map(InputY, 0, 300 , 1, 10);

        noTouchCount = 0;
        if ( (InputX > SetpointX - BOUND && InputX < SetpointX + BOUND  && InputY > SetpointY - BOUND && InputY < SetpointY + BOUND) )
        {
          stable++;
         // Led_diamond();
        }
        else
        {
          stable = 0;
        }

        xPID.Compute();
        yPID.Compute();

        OutputY = map(OutputY, 0, 300, 60, 120);
        OutputX = map(OutputX, 0, 400, 55, 125);
        sprintf(mesageData, "%d %d %d %d", InputX, InputY, OutputX, OutputY);

        Serial.write(mesageData);
        /*Serial.print("   InputX: "); Serial.print(InputX);
          Serial.print("   InputY:"); Serial.print(InputY);
          Serial.print("   Xoutput1:"); Serial.print(OutputX);
          Serial.print("   Youtput1:"); Serial.println(OutputY);*/
      }
      else // Ball is not on plate
      {
        noTouchCount++;
        if (noTouchCount == 75)
        {
          noTouchCount++;
          OutputX = 80;
          OutputY = 80;
          xServo.write(OutputX);
          yServo.write(OutputY);
          delay(100);
        }
        if (noTouchCount == 150) //if there is no ball on plate longer
        {
          xServo.detach(); //detach servos
          yServo.detach();
        }
      }

      xServo.write(OutputX); // outputX
      yServo.write(OutputY);
    }
  SetpointX = points[done];
  done++;
    SetpointY = points[done];
  done++;

   /* xServo.detach();
    yServo.detach();

    while (!Serial.available() && stable == 3)
    {
      TSPoint p = ts.getPoint();

      InputX = map(p.x, 125, 965, 0, 400);
      InputY = map(p.y, 130, 910, 300, 0);
      sprintf(mesageData, "%d %d %d %d", InputX, InputY, OutputX, OutputY);
      Serial.write(mesageData);
      if ( ! ( InputX > SetpointX - BOUND && InputX < SetpointX + BOUND  && InputY > SetpointY - BOUND && InputY < SetpointY + BOUND) )
      {
        xServo.attach(XServoPin);
        yServo.attach(YServoPin);
        stable = 0;
      }
    }*/
  SetpointX = points[++done];
    SetpointY = points[++done];
  }
}

int draw_square() {
  int done = 2;
  unsigned long oldtime = millis();
  Serial.println("drawing square");
  int points[14] = {83, 57,
                    73, 237,
                    210, 237,
                    345, 237,
                    340, 57,
                    202, 57,
                    83, 57
                   };
  SetpointX = points[0];
    SetpointY = points[1];
  
  while (done < 14) {
  stable = 0;
    while (!Serial.available() && stable < 2)
    {
      TSPoint p = ts.getPoint();
      
      if ( p.x != 0 && p.y != 1023 ) // Ball is on plate
      {
        xServo.attach(XServoPin);
        yServo.attach(YServoPin);
        Serial.print("P .x : "); Serial.println(p.x);
        Serial.print("P .y : "); Serial.println(p.y);
        InputX = map(p.x, 125, 965, 0, 400);  ///uzun kenar Xx
        InputY = map(p.y, 130, 910, 300, 0);

        Led_Input_X =  map(InputX, 0, 400, 0, 13); //*     !!!!!!Burda map edim ters olabilir
        Led_Input_Y =  map(InputY, 0, 300 , 1, 10);

        noTouchCount = 0;
        if ( (InputX > SetpointX - BOUND && InputX < SetpointX + BOUND  && InputY > SetpointY - BOUND && InputY < SetpointY + BOUND) )
        {
          stable++;
      //Led_rectangle();
        }
        else
        {
          stable = 0;
        }

        xPID.Compute();
        yPID.Compute();

        OutputY = map(OutputY, 0, 300, 60, 120);
        OutputX = map(OutputX, 0, 400, 55, 125);
        /*sprintf(mesageData, "{ %d %d %d %d}", OutputX, OutputY, InputX, InputY);
        Serial.write(mesageData);*/
      }
      else // Ball is not on plate
      {
        noTouchCount++;
        if (noTouchCount == 75)
        {
          noTouchCount++;
          OutputX = 80;
          OutputY = 80;
          xServo.write(OutputX);
          yServo.write(OutputY);
          delay(100);
        }
        if (noTouchCount == 150) //if there is no ball on plate longer
        {
          xServo.detach(); //detach servos
          yServo.detach();
        }
      }
      if (millis() - oldtime >= 100) {
      sprintf(mesageData, "{ %d %d %d %d}", (int)OutputX, (int)OutputY, (int)InputX, (int)InputY);
      Serial.write(mesageData);

      xServo.write(OutputX); // outputX
      yServo.write(OutputY);
    }
  SetpointX = points[done];
  done++;
    SetpointY = points[done];
  done++;

   /* xServo.detach();
    yServo.detach();

    while (!Serial.available() && stable == 2)
    {
      TSPoint p = ts.getPoint();

      InputX = map(p.x, 125, 965, 0, 400);
      InputY = map(p.y, 130, 910, 300, 0);
      sprintf(mesageData, "{ %d %d %d %d}", InputX, InputY, OutputX, OutputY);
      Serial.write(mesageData);
      if ( ! ( InputX > SetpointX - BOUND && InputX < SetpointX + BOUND  && InputY > SetpointY - BOUND && InputY < SetpointY + BOUND) )
      {
        xServo.attach(XServoPin);
        yServo.attach(YServoPin);
        stable = 0;
      }
    }*/
  }
}
}

int game_fonk() {
  unsigned long oldtime = millis();
  int count = 0;

  Save_Yem();

  SetpointX = yem[count][0];
  SetpointY = yem[count][1];

  while (!Serial.available() && count != 3)
  {
    TSPoint p = ts.getPoint();
    if ( p.x != 0 && p.y != 1023 ) // Ball is on plate
    {
      xServo.attach(XServoPin);
      yServo.attach(YServoPin);

      InputX = map(p.x, 125, 965, 0, 400);  ///uzun kenar Xx
      InputY = map(p.y, 130, 910, 300, 0);

      Led_Input_X =  map(InputX, 0, 400, 14, 0); //*     !!!!!!Burda map edim ters olabilir
      Led_Input_Y =  map(InputY, 0, 300 , 11, 0);

      Led_3x3(Led_Input_Y - 1 , Led_Input_X - 1 ); ////////////////////Buralar recep çerceve yapınca değişebiilirm
      Led_one(yem_led[count][1] - 1, yem_led[count][0] - 1); ////////////yem
      noTouchCount = 0;
      if ( (InputX > SetpointX - BOUND && InputX < SetpointX + BOUND  && InputY > SetpointY - BOUND && InputY < SetpointY + BOUND) )
      {
        //Serial.print("Yem yendi:");Serial.print(count);
        ++count;
        if (count != 3)
        {
          SetpointX = yem[count][0];
          SetpointY = yem[count][1];
        }
      }

      xPID.Compute();
      yPID.Compute();

      OutputY = map(OutputY, 0, 300, 60, 120);
      OutputX = map(OutputX, 0, 400, 55, 125);

    }
    else // Ball is not on plate
    {
      noTouchCount++;
      if (noTouchCount == 75)
      {
        noTouchCount++;
        OutputX = 80;
        OutputY = 80;
        xServo.write(OutputX);
        yServo.write(OutputY);
        delay(100);                       //delayler azaltılmalı
      }
      if (noTouchCount == 150) //if there is no ball on plate longer
      {
        xServo.detach(); //detach servos
        yServo.detach();
      }
    }

    xServo.write(OutputX); // outputX
    yServo.write(OutputY);
    if (millis() - oldtime >= 100) {
      sprintf(mesageData, "{ %d %d %d %d}", (int)OutputX, (int)OutputY, (int)InputX, (int)InputY);
      Serial.write(mesageData);
    }
  }
}

void go_center() //////////////////////draw yapanlar için UYARI ONLARA YENİ SETPOİNTLER LAZIM KÖŞELERE GİDEMEZLER BUZZER ÖTER
{
  unsigned long oldtime = millis();
  while (!Serial.available() && stable < STABLE_THRESHOLD)
  {
    TSPoint p = ts.getPoint();
    if ( p.x != 0 && p.y != 1023 ) // Ball is on plate
    {
      xServo.attach(XServoPin);
      yServo.attach(YServoPin);

      InputX = map(p.x, 125, 965, 0, 400);  ///uzun kenar Xx
      InputY = map(p.y, 130, 910, 300, 0);

      Led_Input_X =  map(InputX, 0, 400, 14, 0); //*     !!!!!!Burda map edim ters olabilir
      Led_Input_Y =  map(InputY, 0, 300 , 11, 0);

      Led_3x3(Led_Input_Y - 1 , Led_Input_X - 1 ); ////////////////////Buralar recep çerceve yapınca değişebiilirm

      noTouchCount = 0;
      if ( (InputX > SetpointX - BOUND && InputX < SetpointX + BOUND  && InputY > SetpointY - BOUND && InputY < SetpointY + BOUND) )
      {
        stable++;
      }
      else
      {
        stable = 0;
      }

      xPID.Compute();
      yPID.Compute();

      OutputY = map(OutputY, 0, 300, 60, 120);
      OutputX = map(OutputX, 0, 400, 55, 125);
    }
    else // Ball is not on plate
    {

      noTouchCount++;
      if (noTouchCount == 75)
      {
        noTouchCount++;
        OutputX = 80;
        OutputY = 80;
        xServo.write(OutputX);
        yServo.write(OutputY);

        delay(100);                       //delayler azaltılmalı
      }
      if (noTouchCount == 150) //if there is no ball on plate longer
      {
        xServo.detach(); //detach servos
        yServo.detach();
      }
    }

    xServo.write(OutputX); // outputX
    yServo.write(OutputY);
    if (millis() - oldtime >= 100) {
      sprintf(mesageData, "{ %d %d %d %d}", (int)OutputX, (int)OutputY, (int)InputX, (int)InputY);
      Serial.write(mesageData);
    }
  }

  xServo.detach();
  yServo.detach();

  while (!Serial.available() && stable == STABLE_THRESHOLD)
  {
    TSPoint p = ts.getPoint();

    InputX = map(p.x, 125, 965, 0, 400);
    InputY = map(p.y, 130, 910, 300, 0);
    Led_Input_X =  map(InputX, 0, 400, 14, 0); //*     !!!!!!Burda map edim ters olabilir
    Led_Input_Y =  map(InputY, 0, 300 , 11, 0);

    if ( ! ( InputX > SetpointX - BOUND && InputX < SetpointX + BOUND  && InputY > SetpointY - BOUND && InputY < SetpointY + BOUND) )
    {
      xServo.attach(XServoPin);
      yServo.attach(YServoPin);
      stable = 0;
    }
    if (millis() - oldtime >= 100) {
      sprintf(mesageData, "{ %d %d %d %d}", (int)OutputX, (int)OutputY, (int)InputX, (int)InputY);
      Serial.write(mesageData);
    }
    Led_3x3(Led_Input_Y - 1, Led_Input_X - 1);////////////////////Buralar recep çerceve yapınca değişebiilirm
  }
}

/*SNAKE GAME ALL FUNCTIONS START*/
bool Save_Yem() //Basıldıkça yemleri kaydediyor.
{ int oldx = 15, oldy = 15, count = 0;

  do {

    TSPoint parmak = ts.getPoint();
    if ( parmak.x != 0 && parmak.y != 1023 ) // Ball is on plate
    {
      if ( oldx != map(map(parmak.x, 125, 965, 0, 400), 0, 400, 14, 0) || oldy  !=  map(map(parmak.y, 130, 910, 300, 0), 0, 300 , 11, 0))
      {
        yem[count][0] = map(parmak.x, 125, 965, 0, 400);
        yem[count][1] = map(parmak.y, 130, 910, 300, 0);
        yem_led[count][0] =  map(yem[count][0], 0, 400, 14, 0); //*     !!!!!!Burda map edim ters olabilir
        yem_led[count][1] =  map(yem[count][1], 0, 300 , 11, 0);
        oldx = yem_led[count][0];
        oldy = yem_led[count][1];
        ++count;
        delay(1000);
      }

    }

  } while (count != 3);
  return true;

}

bool Check_Eat(int Gelen_X, int  Gelen_Y )
{ //topun olduğu konumun led mapi geliyor.

  if ((Gelen_X - 1 == rand_X) && (Gelen_Y - 1 == rand_Y) )
    return true;
  else if ((Gelen_X - 1 == rand_X) && (Gelen_Y == rand_Y) )
    return true;
  else if ((Gelen_X - 1 == rand_X) && (Gelen_Y + 1 == rand_Y) )
    return true;
  else if ((Gelen_X + 1 == rand_X) && (Gelen_Y - 1 == rand_Y) )
    return true;
  else if ((Gelen_X + 1 == rand_X) && (Gelen_Y  == rand_Y) )
    return true;
  else if ((Gelen_X + 1 == rand_X) && (Gelen_Y + 1 == rand_Y) )
    return true;
  else if ((Gelen_X  == rand_X) && (Gelen_Y - 1 == rand_Y) )
    return true;
  else if ((Gelen_X  == rand_X) && (Gelen_Y  == rand_Y) )
    return true;
  else if ((Gelen_X  == rand_X) && (Gelen_Y + 1 == rand_Y) )
    return true;

  return false;
}
void Random_Point() ///burası ters merak etme dogru
{
  rand_Y = random(1, 11); //max-1
  rand_X = random(0, 14);

  SetpointY = map(rand_Y, 1, 10 , 0 , 300);
  SetpointX = map(rand_X, 0, 13 , 0 , 400);
  stat = 1;
}

void Led_3x3(int Gelen_X, int Gelen_Y) //fonksiyon ilk row x olarak yazıldığı için calları ters
{
  Led_one(Gelen_X - 1, Gelen_Y - 1);
  //  delay(del);
  Led_one(Gelen_X - 1, Gelen_Y);
  //  delay(del);
  Led_one(Gelen_X - 1, Gelen_Y + 1);
  // delay(del);

  Led_one(Gelen_X + 1, Gelen_Y - 1);
  //  delay(del);
  Led_one(Gelen_X + 1, Gelen_Y);
  //  delay(del);
  Led_one(Gelen_X + 1, Gelen_Y + 1);
  // delay(del);

  Led_one(Gelen_X, Gelen_Y - 1);
  // delay(del);
  Led_one(Gelen_X, Gelen_Y + 1);
  // delay(del);
  Led_one(Gelen_X, Gelen_Y);
  //  delay(del);
}
//void 
void Led_diamond() {

  Row_X5();
  delay(del);
  Column_Y3();
  delay(del);

  Row_X4();
  delay(del);
  Column_Y4();
  delay(del);

  Row_X3();
  delay(del);
  Column_Y5();
  delay(del);

  Row_X2();
  delay(del);
  Column_Y6();
  delay(del);

  Row_X1();
  delay(del);
  Column_Y7();
  delay(del);

  Row_X2();
  delay(del);
  Column_Y8();
  delay(del);

  Row_X3();
  delay(del);
  Column_Y9();
  delay(del);

  Row_X4();
  delay(del);
  Column_Y10();
  delay(del);

  Row_X5();
  delay(del);
  Column_Y11();
  delay(del);

  Row_X6();
  delay(del);
  Column_Y10();
  delay(del);

  Row_X7();
  delay(del);
  Column_Y9();
  delay(del);

  Row_X8();
  delay(del);
  Column_Y8();
  delay(del);

  Row_X9();
  delay(del);
  Column_Y7();
  delay(del);

  Row_X8();
  delay(del);
  Column_Y6();
  delay(del);

  Row_X7();
  delay(del);
  Column_Y5();
  delay(del);

  Row_X6();
  delay(del);
  Column_Y4();
  delay(del);

}
void Led_rectangle()
{
  Row_X2();
  Column_Y2();
  delay(del);
  Column_Y3();
  delay(del);
  Column_Y4();
  delay(del);
  Column_Y5();
  delay(del);
  Column_Y6();
  delay(del);
  Column_Y7();
  delay(del);
  Column_Y8();
  delay(del);
  Column_Y9();
  delay(del);
  Column_Y10();
  delay(del);
  Column_Y11();
  delay(del);

  Row_X8();
  Column_Y2();
  delay(del);
  Column_Y3();
  delay(del);
  Column_Y4();
  delay(del);
  Column_Y5();
  delay(del);
  Column_Y6();
  delay(del);
  Column_Y7();
  delay(del);
  Column_Y8();
  delay(del);
  Column_Y9();
  delay(del);
  Column_Y10();
  delay(del);
  Column_Y11();
  delay(del);

  Column_Y2();
  Row_X2();
  delay(del);
  Row_X3();
  delay(del);
  Row_X4();
  delay(del);
  Row_X5();
  delay(del);
  Row_X6();
  delay(del);
  Row_X7();
  delay(del);
  Row_X8();
  delay(del);

  Column_Y12();
  Row_X2();
  delay(del);
  Row_X3();
  delay(del);
  Row_X4();
  delay(del);
  Row_X5();
  delay(del);
  Row_X6();
  delay(del);
  Row_X7();
  delay(del);
  Row_X8();
  delay(del);
}
void Led_one(int x, int y)
{
  //setpoint versin cevresini yakalım.
  switch (x) {

    case 0:
      {
        Row_X0();
        break;
      }
    case 1:
      {
        Row_X1();
        break;
      }
    case 2:
      {
        Row_X2();
        break;
      }
    case 3:
      {
        Row_X3();
        break;
      }
    case 4:
      {
        Row_X4();
        break;
      }
    case 5:
      {
        Row_X5();
        break;
      }
    case 6:
      {
        Row_X6();
        break;
      }
    case 7:
      {
        Row_X7();
        break;
      }
    case 8:
      {
        Row_X8();
        break;
      }
    case 9:
      {
        Row_X9();
        break;
      }
    case 10:
      {
        Row_X10();
        break;
      }
    case 11: {
        Row_X11();
        break;
      }
    default:
      {

        break;
      }

  }
  delay(del);

  switch (y) {

    case 0:
      {
        Column_Y0();
        break;
      }
    case 1:
      {
        Column_Y1();
        break;
      }
    case 2:
      {
        Column_Y2();
        break;
      }
    case 3:
      {
        Column_Y3();
        break;
      }
    case 4:
      {
        Column_Y4();
        break;
      }
    case 5:
      {
        Column_Y5();
        break;
      }
    case 6:
      {
        Column_Y6();
        break;
      }
    case 7:
      {
        Column_Y7();
        break;
      }
    case 8:
      {
        Column_Y8();
        break;
      }
    case 9:
      {
        Column_Y9();
        break;
      }
    case 10:
      {
        Column_Y10();
        break;
      }
    case 11:
      {
        Column_Y11();
        break;
      }
    case 12:
      {
        Column_Y12();
        break;
      }
    case 13:
      {
        Column_Y13();
        break;
      }
    case 14: {
        Column_Y14();
        break;
      }

    default:
      {

      }

  }

  delay(del);
  Column_Y14();
  Row_X11();

}
void Row_X0()
{
  digitalWriteFast(led30, LOW);//Bu komut ledi söndürüyor   //A  // HIGH -- LOW
  digitalWriteFast(led31, LOW);//Bu komut ledi söndürüyor   //B
  digitalWriteFast(led32, LOW);//Bu komut ledi söndürüyor //C
  digitalWriteFast(led33, LOW);//Bu komut ledi söndürüyor  //D
}

void Row_X1()
{
  digitalWriteFast(led30, HIGH);//Bu komut ledi söndürüyor   //A  // HIGH -- LOW
  digitalWriteFast(led31, LOW);//Bu komut ledi söndürüyor   //B
  digitalWriteFast(led32, LOW);//Bu komut ledi söndürüyor //C
  digitalWriteFast(led33, LOW);//Bu komut ledi söndürüyor  //D
}

void Row_X2()
{
  digitalWriteFast(led30, LOW);//Bu komut ledi söndürüyor   //A  // HIGH -- LOW
  digitalWriteFast(led31, HIGH);//Bu komut ledi söndürüyor   //B
  digitalWriteFast(led32, LOW);//Bu komut ledi söndürüyor //C
  digitalWriteFast(led33, LOW);//Bu komut ledi söndürüyor  //D
}

void Row_X3()
{
  digitalWriteFast(led30, HIGH);//Bu komut ledi söndürüyor   //A  // HIGH -- LOW
  digitalWriteFast(led31, HIGH);//Bu komut ledi söndürüyor   //B
  digitalWriteFast(led32, LOW);//Bu komut ledi söndürüyor //C
  digitalWriteFast(led33, LOW);//Bu komut ledi söndürüyor  //D
}

void Row_X4()
{
  digitalWriteFast(led30, LOW);//Bu komut ledi söndürüyor   //A  // HIGH -- LOW
  digitalWriteFast(led31, LOW);//Bu komut ledi söndürüyor   //B
  digitalWriteFast(led32, HIGH);//Bu komut ledi söndürüyor //C
  digitalWriteFast(led33, LOW);//Bu komut ledi söndürüyor  //D
}
void Row_X5()
{
  digitalWriteFast(led30, HIGH);//Bu komut ledi söndürüyor   //A  // HIGH -- LOW
  digitalWriteFast(led31, LOW);//Bu komut ledi söndürüyor   //B
  digitalWriteFast(led32, HIGH);//Bu komut ledi söndürüyor //C
  digitalWriteFast(led33, LOW);//Bu komut ledi söndürüyor  //D
}
void Row_X6()
{
  digitalWriteFast(led30, LOW);//Bu komut ledi söndürüyor   //A  // HIGH -- LOW
  digitalWriteFast(led31, HIGH);//Bu komut ledi söndürüyor   //B
  digitalWriteFast(led32, HIGH);//Bu komut ledi söndürüyor //C
  digitalWriteFast(led33, LOW);//Bu komut ledi söndürüyor  //D
}
void Row_X7()
{
  digitalWriteFast(led30, HIGH);//Bu komut ledi söndürüyor   //A  // HIGH -- LOW
  digitalWriteFast(led31, HIGH);//Bu komut ledi söndürüyor   //B
  digitalWriteFast(led32, HIGH);//Bu komut ledi söndürüyor //C
  digitalWriteFast(led33, LOW);//Bu komut ledi söndürüyor  //D
}
void Row_X8()
{
  digitalWriteFast(led30, LOW);//Bu komut ledi söndürüyor   //A  // HIGH -- LOW
  digitalWriteFast(led31, LOW);//Bu komut ledi söndürüyor   //B
  digitalWriteFast(led32, LOW);//Bu komut ledi söndürüyor //C
  digitalWriteFast(led33, HIGH);//Bu komut ledi söndürüyor  //D
}
void Row_X9()
{
  digitalWriteFast(led30, HIGH);//Bu komut ledi söndürüyor   //A  // HIGH -- LOW
  digitalWriteFast(led31, LOW);//Bu komut ledi söndürüyor   //B
  digitalWriteFast(led32, LOW);//Bu komut ledi söndürüyor //C
  digitalWriteFast(led33, HIGH);//Bu komut ledi söndürüyor  //D
}
void Row_X10()
{
  digitalWriteFast(led30, LOW);//Bu komut ledi söndürüyor   //A  // HIGH -- LOW
  digitalWriteFast(led31, HIGH);//Bu komut ledi söndürüyor   //B
  digitalWriteFast(led32, LOW);//Bu komut ledi söndürüyor //C
  digitalWriteFast(led33, HIGH);//Bu komut ledi söndürüyor  //D
}
void Row_X11()
{
  digitalWriteFast(led30, HIGH);//Bu komut ledi söndürüyor   //A  // HIGH -- LOW
  digitalWriteFast(led31, HIGH);//Bu komut ledi söndürüyor   //B
  digitalWriteFast(led32, LOW);//Bu komut ledi söndürüyor //C
  digitalWriteFast(led33, HIGH);//Bu komut ledi söndürüyor  //D
}
/////////////////////////////////////////////////////////////////////////////////////////
void Column_Y0()
{
  digitalWriteFast(led40, LOW);//Bu komut ledi söndürüyor   //A  // HIGH -- LOW
  digitalWriteFast(led41, LOW);//Bu komut ledi söndürüyor   //B
  digitalWriteFast(led42, LOW);//Bu komut ledi söndürüyor //C
  digitalWriteFast(led43, LOW);//Bu komut ledi söndürüyor  //D
}

void Column_Y1()
{
  digitalWriteFast(led40, HIGH);//Bu komut ledi söndürüyor   //A  // HIGH -- LOW
  digitalWriteFast(led41, LOW);//Bu komut ledi söndürüyor   //B
  digitalWriteFast(led42, LOW);//Bu komut ledi söndürüyor //C
  digitalWriteFast(led43, LOW);//Bu komut ledi söndürüyor  //D
}

void Column_Y2()
{
  digitalWriteFast(led40, LOW);//Bu komut ledi söndürüyor   //A  // HIGH -- LOW
  digitalWriteFast(led41, HIGH);//Bu komut ledi söndürüyor   //B
  digitalWriteFast(led42, LOW);//Bu komut ledi söndürüyor //C
  digitalWriteFast(led43, LOW);//Bu komut ledi söndürüyor  //D
}

void Column_Y3()
{
  digitalWriteFast(led40, HIGH);//Bu komut ledi söndürüyor   //A  // HIGH -- LOW
  digitalWriteFast(led41, HIGH);//Bu komut ledi söndürüyor   //B
  digitalWriteFast(led42, LOW);//Bu komut ledi söndürüyor //C
  digitalWriteFast(led43, LOW);//Bu komut ledi söndürüyor  //D
}

void Column_Y4()
{
  digitalWriteFast(led40, LOW);//Bu komut ledi söndürüyor   //A  // HIGH -- LOW
  digitalWriteFast(led41, LOW);//Bu komut ledi söndürüyor   //B
  digitalWriteFast(led42, HIGH);//Bu komut ledi söndürüyor //C
  digitalWriteFast(led43, LOW);//Bu komut ledi söndürüyor  //D
}
void Column_Y5()
{
  digitalWriteFast(led40, HIGH);//Bu komut ledi söndürüyor   //A  // HIGH -- LOW
  digitalWriteFast(led41, LOW);//Bu komut ledi söndürüyor   //B
  digitalWriteFast(led42, HIGH);//Bu komut ledi söndürüyor //C
  digitalWriteFast(led43, LOW);//Bu komut ledi söndürüyor  //D
}
void Column_Y6()
{
  digitalWriteFast(led40, LOW);//Bu komut ledi söndürüyor   //A  // HIGH -- LOW
  digitalWriteFast(led41, HIGH);//Bu komut ledi söndürüyor   //B
  digitalWriteFast(led42, HIGH);//Bu komut ledi söndürüyor //C
  digitalWriteFast(led43, LOW);//Bu komut ledi söndürüyor  //D
}
void Column_Y7()
{
  digitalWriteFast(led40, HIGH);//Bu komut ledi söndürüyor   //A  // HIGH -- LOW
  digitalWriteFast(led41, HIGH);//Bu komut ledi söndürüyor   //B
  digitalWriteFast(led42, HIGH);//Bu komut ledi söndürüyor //C
  digitalWriteFast(led43, LOW);//Bu komut ledi söndürüyor  //D
}
void Column_Y8()
{
  digitalWriteFast(led40, LOW);//Bu komut ledi söndürüyor   //A  // HIGH -- LOW
  digitalWriteFast(led41, LOW);//Bu komut ledi söndürüyor   //B
  digitalWriteFast(led42, LOW);//Bu komut ledi söndürüyor //C
  digitalWriteFast(led43, HIGH);//Bu komut ledi söndürüyor  //D
}
void Column_Y9()
{
  digitalWriteFast(led40, HIGH);//Bu komut ledi söndürüyor   //A  // HIGH -- LOW
  digitalWriteFast(led41, LOW);//Bu komut ledi söndürüyor   //B
  digitalWriteFast(led42, LOW);//Bu komut ledi söndürüyor //C
  digitalWriteFast(led43, HIGH);//Bu komut ledi söndürüyor  //D
}
void Column_Y10()
{
  digitalWriteFast(led40, LOW);//Bu komut ledi söndürüyor   //A  // HIGH -- LOW
  digitalWriteFast(led41, HIGH);//Bu komut ledi söndürüyor   //B
  digitalWriteFast(led42, LOW);//Bu komut ledi söndürüyor //C
  digitalWriteFast(led43, HIGH);//Bu komut ledi söndürüyor  //D
}
void Column_Y11()
{
  digitalWriteFast(led40, HIGH);//Bu komut ledi söndürüyor   //A  // HIGH -- LOW
  digitalWriteFast(led41, HIGH);//Bu komut ledi söndürüyor   //B
  digitalWriteFast(led42, LOW);//Bu komut ledi söndürüyor //C
  digitalWriteFast(led43, HIGH);//Bu komut ledi söndürüyor  //D
}
void Column_Y12()
{
  digitalWriteFast(led40, LOW);//Bu komut ledi söndürüyor   //A  // HIGH -- LOW
  digitalWriteFast(led41, LOW);//Bu komut ledi söndürüyor   //B
  digitalWriteFast(led42, HIGH);//Bu komut ledi söndürüyor //C
  digitalWriteFast(led43, HIGH);//Bu komut ledi söndürüyor  //D
}
void Column_Y13()
{
  digitalWriteFast(led40, HIGH);//Bu komut ledi söndürüyor   //A  // HIGH -- LOW
  digitalWriteFast(led41, LOW);//Bu komut ledi söndürüyor   //B
  digitalWriteFast(led42, HIGH);//Bu komut ledi söndürüyor //C
  digitalWriteFast(led43, HIGH);//Bu komut ledi söndürüyor  //D
}
void Column_Y14()
{
  digitalWriteFast(led40, LOW);//Bu komut ledi söndürüyor   //A  // HIGH -- LOW
  digitalWriteFast(led41, HIGH);//Bu komut ledi söndürüyor   //B
  digitalWriteFast(led42, HIGH);//Bu komut ledi söndürüyor //C
  digitalWriteFast(led43, HIGH);//Bu komut ledi söndürüyor  //D
}
/*FUNCTİON END*/

