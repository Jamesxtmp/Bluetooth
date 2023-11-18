// Librerias
#include <Arduino.h>
#include <ESP32Servo.h>

// Comprobacion Bluetooth
#include "BluetoothSerial.h"
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` and enable it
#endif
//_ -------------------------------------------------------------------- Declaraciones

// Declaracion de datos Abtractos
BluetoothSerial BT;
Servo BRAZO;
Servo PIERNA_L;
Servo PIERNA_R;
Servo LLANTA_L;
Servo LLANTA_R;

// Declaracion de Nombres de Pines
int pinBrazo = 13;
int pinPiernaL = 12;
int pinPiernaR = 14;
int pinLlantaL = 27;
int pinLlantaR = 26;
// Declacion de constantes
int BRAZO_FRONT_POS = 180;
int BRAZO_MAX_POS = 80;
int BRAZO_MIN_POS = 180;
// PIERNAS
int PIERNA_R_HUMAN_POS = 100;
int PIERNA_L_HUMAN_POS = 80;
int PIERNA_R_CAR_POS = 0;
int PIERNA_L_CAR_POS = 180;
// LLANTAS
int LLANTA_L_STOP = 88;
int LLANTA_R_STOP = 92;
int LLANTA_R_FRONT = 85;
int LLANTA_R_BACK = 180;
int LLANTA_L_FRONT = 180;
int LLANTA_L_BACK = 0;

// VALORES PARA CAMINAR
int HALF_R = (PIERNA_R_HUMAN_POS - PIERNA_R_CAR_POS) / 2;
int HALF_L = (PIERNA_L_CAR_POS - PIERNA_L_HUMAN_POS) / 2;
int PIERNA_R_HALF = PIERNA_R_HUMAN_POS - HALF_R;
int PIERNA_L_HALF = PIERNA_L_HUMAN_POS + HALF_L;
int PIERNA_R_HIDE = PIERNA_R_HUMAN_POS + HALF_R;
int PIERNA_L_HIDE = PIERNA_L_HUMAN_POS - HALF_L;

// Declaraciond de botones BT
bool frontButton = false;
bool backButton = false;
bool leftButton = false;
bool rightButton = false;
bool attackButton = false;
bool modeButton = false;

// Valores inicailes de estado
bool isModeCar = false;
bool isRigthFeet = true;

// Declaracion de funciones
void probarServo(Servo &SERVO);
void servoInitial();
void modeCar();
void modeHumanoid();
void asignacionBTbuttons();

//_ -------------------------------------------------------------------- Setup
void setup()
{
  // Declaracion Bluetooth
  BT.begin("Otto Control BT");

  // Declaracion de Serial
  Serial.begin(9600);
  Serial.println("El dispositivo Bluetooth está listo para emparejarse");

  // Inicializacion de servos
  BRAZO.attach(pinBrazo);
  PIERNA_L.attach(pinPiernaL);
  PIERNA_R.attach(pinPiernaR);
  LLANTA_L.attach(pinLlantaL);
  LLANTA_R.attach(pinLlantaR);

  // Inciando estado del Otto
  modeHumanoid();
}
//_ -------------------------------------------------------------------- Loop
void loop()
{
  asignacionBTbuttons();
  if (isModeCar)
  {
    if (frontButton)
    {
      LLANTA_L.write(LLANTA_L_FRONT);
      LLANTA_R.write(LLANTA_R_FRONT);
    }
    if (backButton)
    {
      LLANTA_L.write(LLANTA_L_BACK);
      LLANTA_R.write(LLANTA_R_BACK);
    }
    if (leftButton)
    {
      LLANTA_L.write(LLANTA_L_BACK);
      LLANTA_R.write(LLANTA_R_FRONT);
    }
    if (rightButton)
    {
      LLANTA_L.write(LLANTA_L_FRONT);
      LLANTA_R.write(LLANTA_R_BACK);
    }
    if (!frontButton && !backButton && !leftButton && !rightButton)
    {
      LLANTA_L.write(LLANTA_L_STOP);
      LLANTA_R.write(LLANTA_R_STOP);
    }
  }
  if (!isModeCar)
  {
    if (frontButton)
    {
      if (isRigthFeet)
      {
        PIERNA_R.write(PIERNA_R_HALF);
        delay(300);
        LLANTA_R.write(LLANTA_R_FRONT);
        LLANTA_L.write(LLANTA_L_FRONT);
        delay(300);
        LLANTA_L.write(LLANTA_L_STOP);
        LLANTA_R.write(LLANTA_R_STOP);
        PIERNA_R.write(PIERNA_R_HUMAN_POS);
        isRigthFeet = !isRigthFeet;
        frontButton = false;
      }
      else
      {
        PIERNA_L.write(PIERNA_L_HALF);
        delay(300);
        LLANTA_L.write(LLANTA_L_FRONT);
        LLANTA_R.write(LLANTA_R_FRONT);
        delay(300);
        LLANTA_R.write(LLANTA_R_STOP);
        LLANTA_L.write(LLANTA_L_STOP);
        PIERNA_L.write(PIERNA_L_HUMAN_POS);
        isRigthFeet = !isRigthFeet;
        frontButton = false;
      }
    }
    if (backButton)
    {
      if (isRigthFeet)
      {
        PIERNA_R.write(PIERNA_R_HALF);
        delay(300);
        LLANTA_R.write(LLANTA_R_BACK);
        LLANTA_L.write(LLANTA_L_BACK);
        delay(300);
        LLANTA_L.write(LLANTA_L_STOP);
        LLANTA_R.write(LLANTA_R_STOP);
        PIERNA_R.write(PIERNA_R_HUMAN_POS);
        isRigthFeet = !isRigthFeet;
        backButton = false;
      }
      else
      {
        PIERNA_L.write(PIERNA_L_HALF);
        delay(300);
        LLANTA_R.write(LLANTA_R_BACK);
        LLANTA_L.write(LLANTA_L_BACK);
        delay(300);
        LLANTA_R.write(LLANTA_R_STOP);
        LLANTA_L.write(LLANTA_L_STOP);
        PIERNA_L.write(PIERNA_L_HUMAN_POS);
        isRigthFeet = !isRigthFeet;
        backButton = false;
      }
    }
    if (leftButton)
    {
      PIERNA_L.write(PIERNA_L_HALF);
      PIERNA_R.write(PIERNA_R_HIDE);
      delay(150);
      LLANTA_R.write(LLANTA_R_STOP);
      PIERNA_R.write(PIERNA_R_HUMAN_POS);
    }
    if (rightButton)
    {
      PIERNA_R.write(PIERNA_R_HALF);
      PIERNA_L.write(PIERNA_L_HIDE);
      delay(150);
      LLANTA_L.write(LLANTA_L_STOP);
      PIERNA_L.write(PIERNA_L_HUMAN_POS);
    }
  }
  if (attackButton)
  {
    BRAZO.write(BRAZO_MAX_POS);
  }
  if (!attackButton)
  {
    BRAZO.write(BRAZO_MIN_POS);
  }
}
//_ -------------------------------------------------------------------- Funciones
// Funcines de otto mode
void probarServo(Servo &SERVO)
{
}
void servoInitial()
{
  BRAZO.write(BRAZO_FRONT_POS);
  PIERNA_L.write(PIERNA_L_HUMAN_POS);
  PIERNA_R.write(PIERNA_R_HUMAN_POS);
}
void modeCar()
{
  BRAZO.write(BRAZO_FRONT_POS);
  PIERNA_L.write(PIERNA_L_CAR_POS);
  PIERNA_R.write(PIERNA_R_CAR_POS);
}
void modeHumanoid()
{
  BRAZO.write(BRAZO_FRONT_POS);
  PIERNA_L.write(PIERNA_L_HUMAN_POS);
  PIERNA_R.write(PIERNA_R_HUMAN_POS);
}
void asignacionBTbuttons()
{
  if (BT.available())
  {
    int incoming = BT.read();
    Serial.print("Recibido: ");
    Serial.println(incoming);
    if (incoming == 1)
    {
      isModeCar = true;
      modeCar();
    }
    if (incoming == 2)
    {
      isModeCar = false;
      modeHumanoid();
    }
    if (incoming == 10)
    {
      frontButton = false;
    }
    if (incoming == 11)
    {
      frontButton = true;
    }
    if (incoming == 20)
    {
      backButton = false;
    }
    if (incoming == 21)
    {
      backButton = true;
    }
    if (incoming == 30)
    {
      leftButton = false;
    }
    if (incoming == 31)
    {
      leftButton = true;
    }
    if (incoming == 40)
    {
      rightButton = false;
    }
    if (incoming == 41)
    {
      rightButton = true;
    }
    if (incoming == 50)
    {
      attackButton = false;
    }
    if (incoming == 51)
    {
      attackButton = true;
    }
  }
}