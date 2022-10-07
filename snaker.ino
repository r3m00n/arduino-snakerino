#include "LedControl.h"

unsigned long curMil = 0;   //für besseren UserInput 
unsigned long prevMil = 0;  //Milis statt delay 

const int SW_pin = 2; // digital pin connected to switch output
const int X_pin = 0;  // ANALog pin connected to X output
const int Y_pin = 1;  // ANALog pin connected to Y output

LedControl lc=LedControl(12,10,11,1); //ledScreenPins

/* we always wait a bit between updates of the display */
unsigned long delaytime1=500;
unsigned long delaytime2=50;

byte karte [8][8];  //Map auf der alles abgebildet wird
short player [3];   //x, y, dir {1=links, 2=rechts, 3=oben, 4=unten)
byte food [3];      //x, y, iz da {0=nein, 1=ja}
byte score = 0;     //offensichtlich wofür diese var zuständig ist
byte taiL = 2;      //Tail length (darf irgendwie nicht 1 sein)
byte tailX[64];
byte tailY[64];

void setup() {
  randomSeed(analogRead(0));  //random einleiten
  Serial.begin(9600);         //SerialScreen auf 9600
  pinMode(SW_pin, INPUT);     //JOYSTICK SETUP
  digitalWrite(SW_pin, HIGH); //JOYSTICK SETUP

  lc.shutdown(0,false); //The MAX72XX is in power-saving mode on startup, we have to do a wakeup call
  lc.setIntensity(0,8); //Set the brightness to a medium values 
  lc.clearDisplay(0);   //and clear the display     SCREENSETUP

  reset();
}

void loop() {
  curMil = millis();
  GenerateFood();
  GetPlayerInput();
  if((curMil - prevMil) > 250){
    prevMil = curMil;
    Think();
    DrawSerialKarte();
    DrawKarte();
  }
}

void reset(){
  score = 0;
  for(int x=0; x<8; x++){
    for(int y=0; y<8; y++){
      karte[x][y] = 0;
    }
  }
  for(int c=0; c<64; c++){
    tailX[c] = 0;
    tailX[c] = 0;
  }
  player[0] = 0;    //x = 3
  player[1] = 3;    //y = 2
  player[2] = 2;   //dir = rechts
  food[2] = 0;
  GenerateFood();
}

void GenerateFood(){
  if(food[2] == 0){
    int foodX = random(7);
    int foodY = random(7);
    if(player[0] == foodX && player[1] == foodY){ //check ob auf Spieler
      GenerateFood();                             //dann neu
    }
    food[0] = foodX;
    food[1] = foodY;
    food[2] = 1;
    karte[food[1]][food[0]] = 2;
  }
}

void GetPlayerInput(){
  if(analogRead(X_pin)<100){  //links
    player[2] = 1;
    Serial.println("GOlinks");
  }
  if(analogRead(X_pin)>900){  //rechts
    player[2] = 2;
    Serial.println("GOrechts");
  }
  if(analogRead(Y_pin)<100){  //oben
    player[2] = 3;
    Serial.println("GOoben");
  }
  if(analogRead(Y_pin)>900){  //unten
    player[2] = 4;
    Serial.println("GOunten");
  }
  if(digitalRead(SW_pin)==0){
    //Serial.println("GOreset");
    reset();
  }
}

void Think(){
  byte prevX = tailX[0];
  byte prevY = tailY[0];
  byte pre2X, pre2Y;
  tailX[0] = player[0];
  tailY[0] = player[1];
  for(int i=1; i<taiL; i++){
    pre2X = tailX[i];
    pre2Y = tailY[i];
    tailX[i] = prevX;
    tailY[i] = prevY;
    prevX = pre2X;
    prevY = pre2Y;
  }
  karte[prevY][prevX] = 0;
  karte[pre2Y][pre2X] = 0;

  if(player[2] == 1 && player [0] > -1)
    {player[0]--;}
  if(player[2] == 1 && player [0] == -1)
    {player[0]=7;}
  if(player[2] == 2 && player [0] < 8)
    {player[0]++;}
  if(player[2] == 2 && player [0] == 8)
    {player[0]=0;}
  if(player[2] == 3 && player [1] > -1)
    {player[1]--;}
  if(player[2] == 3 && player [1] == -1)
    {player[1]=7;}
  if(player[2] == 4 && player [1] < 8)
    {player[1]++;}
  if(player[2] == 4 && player [1] == 8)
    {player[1]=0;}
    
  if(player[0] == food[0] && player[1] == food[1]){
    food[2]=0;
    score++;
    taiL++;
  }

  for(int x; x < taiL; x++){
    for(int y; y < taiL; y++){
      //if(player[0] == 
    }
  }
  
  karte[player[1]][player[0]] = 1;

  for(int a = 0; a < taiL; a++){
    karte[tailY[1]][tailX[0]] = 1;
  }
}

void DrawKarte(){
  for(int i=0; i<8; i++){
    for(int k=0; k<8; k++){
      lc.setLed(0,i,k,karte[i][k]);
      if(karte[i][k] == 2 && curMil%2==0)
        {lc.setLed(0,i,k,0);}   //food blinkt
    }
  }
}

void DrawSerialKarte(){
  Serial.print(player[0]);
  Serial.print(" , ");
  Serial.print(player[1]);
  Serial.println();
  Serial.println();
  for(int i=0; i<8; i++){
    for(int k=0; k<8; k++){
      Serial.print(karte[i][k]);
      Serial.print(" ");
    }
    Serial.println();   
  }
  Serial.print("\n\n");
}


/*
 * 2DO: anfangs blinken wenn's losgeht
 *      Tail überarbeiten
 *      Rail Hit
 *      FoodSpawn (on Tail)
 *      reset
 *      Messages
 *      
 * BUG: Manchmal erscheint Food nicht
 *      das mit der 1 bei taiL
 *      
 *      MENU:
 *        Blinkt in Tickgeschwindigkeit
 *        Hoch langsamer
 *        runter schneller
 *        rechts los/Start
*/
