#include <LiquidCrystal.h>  // Добавляем необходимую библиотеку
LiquidCrystal lcd(7, 6, 5, 4, 3, 2); // (RS, E, DB4, DB5, DB6, DB7)

//c - constants, v - global variables, t - local variables, p - function patameters
// BT - Bottle Layer

const byte cVers = 7;
const byte cDelay = 55;
const byte cExistEnough = 155;
const byte cCheckOutPeriod = 33;
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
const byte cVolumeDetermination = 3;

byte cNum = 20; //число периодов несущей 38кГц в посылаемом пакете
byte cImpulse = 10;  // длительность полупериода

long vProcessed[3];
boolean vBLExists[3];

long vBLExisting[3];
byte vBLAbsenting[3];
byte vInspectingBL;
byte vCurrBL;
byte vPegasStatus = cVolumeDetermination;
long vExistEnough = cExistEnough;
byte vCheckOutPeriod = cCheckOutPeriod;

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
  //Отключаем обработку не задействованых датчиков
  if (vPegasStatus > c1L) {
    processBL(c2L, cSensor2, cLED2);
  }
  if (vPegasStatus > c0L) {
    processBL(c1L, cSensor1, cLED1);
  }
  processBL(c0L, cSensor0, cLED0);
}

void processBL(byte pBottLayer, byte pSensor, byte pLED) {
  vCurrBL = pBottLayer;
  if (vPegasStatus == cVolumeDetermination) {
    maintainBL(pBottLayer, pSensor, pLED);

    if (vBLExists[vInspectingBL]) {
      byte tParentLayerExisting = 0;
      if (vInspectingBL < c2L) {
        tParentLayerExisting = vBLExisting[vInspectingBL + 1];
      }
      if (vBLExisting[vInspectingBL] - tParentLayerExisting > cCheckOutPeriod) {
        vPegasStatus = vInspectingBL;
        lcd.setCursor(0, 1);
        lcd.print("            ");
        vExistEnough = cExistEnough * (vPegasStatus + 1);
        vCheckOutPeriod = cCheckOutPeriod * (vPegasStatus + 1);
      }
    }

  } else {

    maintainBL(vPegasStatus, pSensor, pLED);
    if (vBLAbsenting[vPegasStatus] == vCheckOutPeriod) {
      if (vBLExisting[vInspectingBL] == vExistEnough - vCheckOutPeriod) {
        bottProcessed(true);
      } else {
        bottProcessed(false);
      }
    }
  }
}

boolean maintainBL(byte pBL, byte pSensor, byte pLED) {
  vInspectingBL = pBL;
  delay(cDelay);
  for (int i = 0; i < cNum; i++) //цикл несущей
  {
    digitalWrite(pLED, HIGH); //зажигаем ик-диод
    delayMicroseconds(cImpulse); //положительный полупериод
    digitalWrite(pLED, LOW); //гасим ик-диод
    delayMicroseconds(cImpulse - 1); //выравниваем скважность
  }
  vBLExists[vInspectingBL] = !digitalRead(pSensor);
  changeCounters();
}

void bottProcessed(boolean Appropriate) {
  if (Appropriate) {
    vProcessed[vInspectingBL]++;
  }
  vPegasStatus = cVolumeDetermination;
  lcd.setCursor(0, 0);
  lcd.print("            ");
  lcd.setCursor(12, 0);
  lcd.print(vProcessed[0] + vProcessed[1] * 2 + vProcessed[2] * 4);
  lcd.setCursor(0, 1);
  lcd.print("             ");

  for (int i = 0; i <= 2; i++) {
    resetBottLayer(i);
  }
}

void resetBottLayer(byte pLayer) {
  vBLExists[pLayer] = false;

  vBLExisting[pLayer] = 0;
  vBLAbsenting[pLayer] = 0;

  lcd.setCursor(pLayer * 4 +1, 0);
  lcd.print(vProcessed[pLayer]);
}

void changeCounters() {
  //lcd.setCursor(vInspectingBL * 3, 0);
  //lcd.print("  ");
  lcd.setCursor(vCurrBL * 4, 0);
  lcd.print(" ");
  lcd.setCursor(vCurrBL * 4, 1);
  lcd.print(" ");
  
  if (vBLExists[vInspectingBL]) {
    lcd.setCursor(vCurrBL * 4, 0);
    lcd.print("\xFF");
    lcd.setCursor(vCurrBL * 4, 1);
    lcd.print("\xFF");
    if (vBLExisting[vInspectingBL] < vExistEnough) {
      vBLExisting[vInspectingBL]++;
    }
    if (vBLAbsenting[vInspectingBL] > 0) {
      vBLAbsenting[vInspectingBL]--;
    }
  } else {
    if (vBLAbsenting[vInspectingBL] < vCheckOutPeriod) {
      vBLAbsenting[vInspectingBL]++;
    }
    if (vBLExisting[vInspectingBL] > 0) {
      vBLExisting[vInspectingBL]--;
    }
  }

  lcd.setCursor(vInspectingBL * 4 +1, 1);
  lcd.print("   ");
  lcd.setCursor(vInspectingBL * 4 +1, 1);
  lcd.print(vBLExisting[vInspectingBL]);
}
