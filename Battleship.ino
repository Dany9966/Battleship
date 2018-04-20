#include "LedControlMS.h"
#include <binary.h>

/*
 * Vom avea nevoie de un obiect LedControl pentru a controla ledurile matricii
 * pin 12 pentru DIN
 * pin 11 pentru CLK
 * pin 10 pentru LOAD
 * Folosim doua matrici de tipul MAX72XX
 * 
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
 * 
 */

int rightButtonPin = 3; //blue
int downButtonPin = 2; //red
int fireButtonPin = 5; //yellow

//Obiect creat pentru controlarea matricii de leduri
LedControl lc = LedControl(datainPin, clkPin, loadPin, DEVICES);

//matrici backend, pentru user si cpu
/* 
 *  valori posibile:
 *  0 - apa
 *  1 - nava
 *  2 - zona lovita anterior
 *  
 */
int playerMatrix[8][8];
int cpuMatrix[8][8];

unsigned int smileyFace[8] = { //fata zambareata
  B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10100101,
  B10011001,
  B01000010,
  B00111100
};

unsigned int straightFace[8] = { //fata lunga
  B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10111101,
  B10000001,
  B01000010,
  B00111100
};

int pRow, pColumn; //randul si coloana curenta a user-ului

//initializeaza valorile random ale unei matrici
/*
 * se genereaza valori la intamplare pentru pozitia navelor unei matrici backend 
 * matrix - matricea de initilizat
 * 
 */
void initializareShips(int matrix[8][8]){
  //initializare ship orizontal de marime 4
  int row = random(0,8); //se genereaza orice rand
  int column = random(0,5); //se genereaza pana la coloana 4 deoarece nava are o marime de 4 leduri
  for(int i = column; i < column + 4; i++){
    //Serial.println(i);
    matrix[row][i] = 1; //valorile alese se seteaza pe 1, acestea se folosesc pentru a aprinde ledurile
  }

  //initializare ship vertical de marime 3
  int column1 = random(0,8); //initial, se genereaza pe orice coloana
  int row1 = random(0,6); // se generaza pana la randul 5 pentru ca nava are o marime de 3 leduri
  while(column1 < column + 4 && column1 > column - 1){ //cat timp navele se suprapun, se genereaza nava in alt loc
    if(row1 > row || row1 < row - 2){ 
      break;
    }
    else{
      column1 = random(0,8); 
      row1 = random(0,6);
    }
  }
  
  for(int i = row1; i < row1 + 3; i++){
    //Serial.println(i);
    matrix[i][column1] = 1; //valorile alese se seteaza pe 1
  }
}

//afisarea efectiva pe matricea de leduri, folosind matricile backend
// matrix - matricea de afisat
void afisareMatrix(int matrix[8][8]){
  
  
  for(int i = 0; i < 8; i++){
    unsigned int binary = 0;  
    unsigned int aux = 0;
    for(int j = 0; j < 8; j++){
      //Serial.print(matrix[i][j]);
      
      //Serial.print("Row: ");  Serial.print(i);  Serial.print(": "); Serial.print(binary); Serial.println();   Serial.println();
      if(matrix[i][j] == 1){
        binary += (matrix[i][j] << (7-j));
      }
    }
    //Serial.println();
    //Serial.print("Row: ");  Serial.print(i);  Serial.print(": "); Serial.print(binary); Serial.println();   Serial.println();
    
    //Serial.print("row: "); Serial.print(i); Serial.print(";");  Serial.print(binary); Serial.println();
    lc.setRow(0, i , binary);
    
  }

  
}

void debugCPU(){
  Serial.println("Matrice oponent:");
  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      Serial.print(cpuMatrix[i][j]);
    }
    Serial.println();
  }
}

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0)); //seed pentru functia random()
  
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
  

  //setarea butoanelor ca pini de input
  pinMode(rightButtonPin,INPUT);
  pinMode(downButtonPin,INPUT);
  pinMode(fireButtonPin,INPUT);
}

//efect de blink pentru alegerea shot-ului. Se va stinge si se va aprinde de doua ori led-ul ales
/*
 *  dev - indexul matricii de leduri
 *  row - randul ledului ales
 *  column - coloana ledului ales
 *  
 */
void playerBlink(int dev, int row, int column){ 
  delay(150);
  //TODO play shooting sound here
  for(int i = 0; i < 2; i++){ 
    lc.setLed(dev, row, column, false);
    delay(500);
    lc.setLed(dev, row, column, true);
    delay(500);
  }
}

// alegerea shot-ului de catre user
/*
 *  row - randul ledului ales
 *  column - coloana ledului ales
 *  
 */
void playerShot(int row, int column){ 
  playerBlink(1, row, column); //efect de blink pentru shot-ul ales
  if(cpuMatrix[row][column] == 0){ //daca am nimerit apa
    lc.setLed(1, row, column, false); //nava netintita, deci se stinge led-ul
    
    //TODO play miss sound here
  }
  else{
    //TODO play hit sound here
    
  }

  cpuMatrix[row][column] = 2; //se seteaza pe 2 pentru a stii faptul ca aici s-a mai lovit si a sari peste aceasta zona
}

// alegerea oponentului
/*
 *  alegerea se face generand randul si coloana ledului aleator prin functia random()
 */
void CPUShot(){ 
  delay(200);
  int row = random(0,8);
  delay(200);
  int column = random(0,8);

  while(playerMatrix[row][column] == 2){ //daca s-a ales deja un loc in care s-a impuscat deja, se genereaza altul
    row = random(0,8);
    column = random(0,8);
  }
  
  
    playerBlink(0, row, column); //efect de blink pentru alegere
  /*  
    //TODO play hit sound here
  }
  else{
    
    //TODO play miss sound here
  }*/

  playerMatrix[row][column] = 2;    // se seteaza 2 pentru a nu impusca din nou in acelasi loc
  lc.setLed(0, row, column, false); //se stinge ledul ales, fie ca a nimerit, fie ca nu
}

//verifica daca tot randul a fost deja tintit pentru a-l evita
/*
 *  daca un rand a fost deja ocupat cu incercari de tintire, acel rand trebuie evitat de ledul de selectie a urmatoarei miscari
 *  row - randul de verificat
 *  matrix - matricea backend in care se afla randul 
 *  
 */
boolean fullRow(int row, int matrix[8][8]){ 
  for(int i = 0; i < 8; i++){
    if(matrix[row][i] != 2){
      return false;
    }
  }

  return true;
}

//verifica daca intreaga coloana a fost deja tintita
/*
 *  asemenea fullRow, dar se verifica coloana
 *  column - coloana de verificat
 *  matrix - matricea in care se afla column
 *  
 */
boolean fullColumn(int column, int matrix[8][8]){ 
  for(int i = 0; i < 8; i++){
    if(matrix[i][column] != 2){
      return false;
    }
  }

  return true;
}

//functie de mutare a ledului spre dreapta
/*
 *  se verifica mai intai daca poozitiile din dreapta ledului aprins au fost deja incercate, iar daca da, se sare peste aceste pozitii
 *  row - randul curent
 *  column - coloana curenta
 *  
 */
void moveRight(){ 
  lc.setLed(1, pRow, pColumn, false);
  do {                    //cat timp zona curenta a fost deja lovita, se trece la urmatoarea
    pColumn++;
    pColumn %= 8;
    while(fullRow(pRow, cpuMatrix)){  //cat timp randurile curente sunt deja lovite, se trec la urmatoarele
      pRow++;
      pRow %= 8;
    }
  }while(cpuMatrix[pRow][pColumn] == 2); 
  
  lc.setLed(1, pRow, pColumn, true); // mutarea efectiva a ledului (in frontend)
}

//functie de mutare a ledului in jos, similar moveRight, dar se interschimba row cu column
/*
 *  row - randul curent
 *  column - coloana curenta
 *  
 */
void moveDown(){ 
   
   lc.setLed(1, pRow, pColumn, false);
   do{
      pRow++;
      pRow %= 8;
      while(fullColumn(pColumn, cpuMatrix)){
        pColumn++;
        pColumn %= 8;
      }
   } while(cpuMatrix[pRow][pColumn] == 2);
   
 
  lc.setLed(1, pRow, pColumn, true); //afisare in frontend
  
}

//afisare fete de final la infinit pana la apasarea butonului reset
/*
 *  face1 - fata de final a jucatorului
 *  face2 - fata de final cpu-ului
 *  fetele pot fi smileyFace say straightFace
 *  
 */
void displayFaces(unsigned int face1[], unsigned int face2[]){ 
  lc.clearDisplay();
  delay(200);
  for(int i = 0; i < 8; i++){
    lc.setRow(0, i, face1[i]);
    lc.setRow(1, i, face2[i]);
  }
  while(true){
    ;
  }
}


/*
 *  functie ce verifica daca jucatorul sau cpu-ul a pierdut, pentru a termina jocul. Se verifica in functie de matricile backend a acestora.
 *  daca o matrice nu mai contine nicio valoare de 1 ( nu mai exista nicio nava nedistrusa ) 
 *  matrix - matricea de verificat
 *  
 */
int lostCheck(int matrix[8][8]){
  for(int i = 0; i < 8; i++){     //se parcurge matricea, iar la prima valoare de 1 ( exista nava nedistrusa ) functia returneaza 0
    for(int j = 0; i < 8; j++){
      if(matrix[i][j] == 1){
        return 0;
      }
    }
  }
                                  // altfel se returneaza 1, iar jocul se termina
  return 1;
}

void loop() {
  
  //miscarea user-ului
  pRow = 0;
  pColumn = -1;
  moveRight();
  
  int rightButtonState, downButtonState, fireButtonState; 
  while(true){
    delay(150);
    //citesc starile butoanelor
    rightButtonState = digitalRead(rightButtonPin);
    downButtonState = digitalRead(downButtonPin);
    fireButtonState = digitalRead(fireButtonPin);
    //daca se apasa butonul dreapta
    if(rightButtonState == HIGH){
      moveRight();
      //Serial.println("right pressed");
    }

    if(downButtonState == HIGH){ //daca se apasa butonul down
      moveDown(); 
      //Serial.println("down pressed");
    }

    if(fireButtonState == HIGH){ //daca se apasa butonul fire
      //Serial.println("fire pressed");
      
      playerShot(pRow, pColumn); //se incearca shot-ul prin functia playerShot
      break;
    }

    
  }
  
  if(lostCheck(cpuMatrix)){ //se verifica daca CPU a pierdut
    
    displayFaces(smileyFace, straightFace); //se afiseaza fetele finale
  }
  else{
    CPUShot(); // altfel, e randul oponentului
    if(lostCheck(playerMatrix)) //se verifica daca jucatorul a pierdut
      displayFaces(straightFace, smileyFace); //se afiseaza fetele finale
  }
}
