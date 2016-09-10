#include <LiquidCrystal.h>  // Добавляем необходимую библиотеку
LiquidCrystal lcd(7, 6, 5, 4, 3, 2); // (RS, E, DB4, DB5, DB6, DB7)

//c - constants, v - global variables, t - local variables, p - function patameters
// BT - Bottle Layer

const byte cVers = 9;
byte cNum0 = 20; //число периодов несущей 38кГц в посылаемом пакете
byte cImpulse0 = 9;  // длительность полупериода
byte cNum1 = 20;
byte cImpulse1 = 10;
byte cNum2 = 20;
byte cImpulse2 = 10;
const byte cDelay = 55;
const byte cExistEnough = 155;
const byte cCheckOutPeriod = 28;
const byte cExists = 2;
const byte cWas = 1;
const byte cNone = 0;

const byte cSensor0 = 8;
const byte cSensor1 = 10;
const byte cSensor2 = 12;

const byte cLED0 = 9;
const byte cLED1 = 11;
const byte cLED2 = 13;

const byte c0L = 0;
const byte c1L = 1;
const byte c2L = 2;
const byte cFake = 3;
const byte cVolumeDetermination = 3;

long vProcessed[3];
boolean vBLExists[3];

long vBLExisting[3];
byte vBLAbsenting[3];
byte vInspectingBL;
byte vPegasStatus = cVolumeDetermination;
//long vExistEnough = cExistEnough;
//byte vCheckOutPeriod = cCheckOutPeriod;

void setup()
{
  lcd.begin(16, 2);// Задаем размерность экрана
  lcd.setCursor(15, 1);
  lcd.print(cVers);

  pinMode(cSensor0, INPUT);
  pinMode(cSensor1, INPUT);
  pinMode(cSensor2, INPUT);

  pinMode(cLED0, OUTPUT);
  pinMode(cLED1, OUTPUT);
  pinMode(cLED2, OUTPUT);
}

void loop() {    
  processBL(c2L, cSensor2, cLED2, cNum2, cImpulse2);
  processBL(c1L, cSensor1, cLED1, cNum1, cImpulse1);
  processBL(c0L, cSensor0, cLED0, cNum0, cImpulse0);
}

void processBL(byte pBottLayer, byte pSensor, byte pLED, byte pNum, byte pImpulse) {
  maintainBL(pBottLayer, pSensor, pLED, pNum, pImpulse);
  
  if (vPegasStatus == cVolumeDetermination) {
    if (vBLExisting[vInspectingBL] == cExistEnough) {
      vPegasStatus = vInspectingBL;
    } else {
      if (vBLAbsenting[vInspectingBL] == cCheckOutPeriod) {bottProcessed(cFake);}
    }
  } else {
    if (vBLAbsenting[vPegasStatus] == cCheckOutPeriod) {
      //c2L достаточно значимый к общему обьему и c1L подверждает консистентность значения, а не является просто большой помехой
      if ((vBLExisting[c2L] / (vBLExisting[c0L] + vBLExisting[c1L] +0.1) > 0.3) && (vBLExisting[c2L] / (vBLExisting[c1L] +0.1)> 0.2)) { 
        bottProcessed(c2L);
      } else {
        if ( vBLExisting[c1L] / (vBLExisting[c0L] +0.1) > 0.4) {
          bottProcessed(c1L); 
        } else {
          bottProcessed(c0L);  
        }
      }
    }
  }
}
boolean maintainBL(byte pBL, byte pSensor, byte pLED, byte pNum, byte pImpulse) {
  vInspectingBL = pBL;
  delay(cDelay);
  for (int i = 0; i < pNum; i++) //цикл несущей
  {
    digitalWrite(pLED, HIGH); //зажигаем ик-диод
    delayMicroseconds(pImpulse); //положительный полупериод
    digitalWrite(pLED, LOW); //гасим ик-диод
    delayMicroseconds(pImpulse - 1); //выравниваем скважность
  }
  vBLExists[vInspectingBL] = !digitalRead(pSensor);
  changeCounters();
}

void bottProcessed(byte pBL) {
  lcd.setCursor(0, 0);
  lcd.print("            ");  
  if (pBL != cFake) {
    vProcessed[pBL]++;
    lcd.setCursor(pBL * 4, 0);
    lcd.print("+");
    }
  vPegasStatus = cVolumeDetermination;

  lcd.setCursor(13, 0);
  lcd.print(vProcessed[c0L] + vProcessed[c1L] * 2 + vProcessed[c2L] * 4);
  //lcd.setCursor(0, 1);
  //lcd.print("             ");

  for (int i = 0; i <= 2; i++) {
    resetBL(i);
  }
}

void resetBL(byte pLayer) {
  vBLExists[pLayer] = false;
  lcd.setCursor(pLayer * 4+1, 0);
  //lcd.print(" ");
  lcd.print(vProcessed[pLayer]);
  vBLExisting[pLayer] = 0;
  vBLAbsenting[pLayer] = 0;
}

void changeCounters() {

  //lcd.setCursor(vInspectingBL * 4, 0);
  //lcd.print("  ");
  lcd.setCursor(vInspectingBL * 4, 1);
  //lcd.print("     ");
  
  if (vBLExists[vInspectingBL]) {
    lcd.print("\xFF");
    if (vBLAbsenting[vInspectingBL] > 0) {
      vBLAbsenting[vInspectingBL]--;
      }
    vBLExisting[vInspectingBL]++;
    //lcd.setCursor(vInspectingBL * 4, 1);
    //lcd.print(vBLExisting[vInspectingBL]);
    
  } else {
    lcd.print(" ");
    if (vBLExisting[vInspectingBL] > 0) {
      vBLAbsenting[vInspectingBL]++;
      vBLExisting[vInspectingBL]--;
      //lcd.print("\xFF");
      //lcd.setCursor(vInspectingBL * 4, 0);
      //lcd.print(vBLAbsenting[vInspectingBL]);
    } 
  }
}



