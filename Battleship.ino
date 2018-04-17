#include "LedControlMS.h"
#include <binary.h>

/*
 * Vom avea nevoie de un obiect LedControl pentru a controla ledurile matricii
 * pin 12 pentru DIN
 * pin 11 pentru CLK
 * pin 10 pentru LOAD
 * Folosim doua matrici de tipul MAX72XX
 */
 #define DEVICES 2
int datainPin = 12;
int clkPin = 11;
int loadPin = 10;
int numDevices = 2;

/*
 * Butonul ce va misca navele spre dreapta se va plasa pe pinul 2
 * Cel pentru a muta navele in jos este plasat pe pinul 3
 * Butonul cu care actionam va fi plasat la pinul 4
 */

int rightButtonPin = 2; //blue
int downButtonPin = 3; //red
int fireButtonPin = 4; //yellow

//Obiect creat pentru controlarea matricii de leduri
LedControl lc = LedControl(datainPin, clkPin, loadPin, DEVICES);

//matrici backend, pentru user si cpu
int playerMatrix[8][8];
int cpuMatrix[8][8];

//initializeaza valorile random ale unei matrici
void initializareShips(int matrix[8][8]){
  //initializare ship orizontal de marime 4
  int row = random(0,8);
  int column = random(0,5);
  for(int i = column; i < column + 4; i++){
    Serial.println(i);
    matrix[row][i] = 1; //valorile alese se seteaza pe 1, acestea se folosesc pentru a aprinde ledurile
  }

  int column1 = random(0,8);
  int row1 = random(0,6);
  while(column1 < column + 4 && column1 > column - 1){ //se verifica daca navele se suprapun, si daca da, se genereaza intr-un loc diferit
    if(row1 > row || row1 < row - 2){
      break;
    }
    else{
      column1 = random(0,8);
      row1 = random(0,6);
    }
  }
  for(int i = row1; i < row1 + 3; i++){
    Serial.println(i);
    matrix[i][column1] = 1; //valorile alese se seteaza pe 1
  }
}

//afisarea efectiva pe matricea de leduri, folosind matricile backend
void afisareMatrix(int matrix[8][8]){
  unsigned int binary;
  
  for(int i = 0; i < 8; i++){
    binary = 0;
    for(int j = 0; j < 8; j++){
      Serial.print(matrix[i][j]);
      binary += matrix[i][j] * (int) pow(2, 7 - j);
      
    }
    Serial.println();
    Serial.print("Row: ");  Serial.print(i);  Serial.print(": "); Serial.print(binary); Serial.println();   Serial.println();
    lc.setRow(0,i,binary);
  }

  
}

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0)); //seed pentru functia random()
  
  // put your setup code here, to run once:
  for(int i = 0; i < numDevices; i++){
    lc.shutdown(i,false); //dezactivare power save pentru matricea i
    lc.setIntensity(i,8);
    lc.clearDisplay(i); //opreste toate ledurile
    
  }
  //initializare matricilor backend (0 inseamna led off)
  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      playerMatrix[i][j] = 0;
      cpuMatrix[i][j] = 0;
    }
  }

  initializareShips(playerMatrix); //generam matricea user-ului 
  delay(200);
  initializareShips(cpuMatrix); //generam matricea oponentului
  delay(200);
  afisareMatrix(playerMatrix); //afisarea efectiva a ledurilor
  Serial.println();
  //afisareMatrix(cpuMatrix);

  //setarea butoanelor ca pini de input
  pinMode(rightButtonPin,INPUT);
  pinMode(downButtonPin,INPUT);
  pinMode(fireButtonPin,INPUT);
  //lc.setRow(0,0,B10000000);
  

  
}

void loop() {
  // put your main code here, to run repeatedly:
  //lc.setLed(0,0,0,true);
  
  
}
