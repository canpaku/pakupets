#include <Servo.h>
#include "Ultrasonic.h"

#define START 100
#define ALL_PATA 101
#define CAP_START 102
#define CAP_OPEN 103
#define LABEL_START 104
#define LABEL_OPEN 105
#define BOTTLE_START 106
#define BOTTLE_OPEN 107
#define THANKYOU 108

//ultrasonic
Ultrasonic Us_judge_hand_cap(2);
Ultrasonic Us_throwed_cap(3);
Ultrasonic Us_judge_hand_label(4);
Ultrasonic Us_throwed_label(5);
Ultrasonic Us_judge_hand_bottle(6);
Ultrasonic Us_throwed_bottle(8);

//distance getting from ultrasonic sensor
long distance_cap, distance_label, distance_bottle;
long just_throwed_cap, just_throwed_label, just_throwed_bottle;

//servo motor
#define FULL_OPEN_CAP 178
#define FULL_OPEN_LABEL 168
#define FULL_OPEN_BOTTLE 175
#define LITTLE_OPEN_CAP 155
#define LITTLE_OPEN_LABEL 130
#define LITTLE_OPEN_BOTTLE 145
#define CLOSE_CAP 85
#define CLOSE_LABEL 85
#define CLOSE_BOTTLE 85
#define YEAH_CAP 178
#define YEAH_LABEL 172
#define YEAH_BOTTLE 168
#define SATISFIED_CAP 94
#define SATISFIED_LABEL 94
#define SATISFIED_BOTTLE 94
Servo Motor_cap;
Servo Motor_label;
Servo Motor_bottle;
int motorPin_cap = 14;
int motorPin_label = 15;
int motorPin_bottle = 16;
//キープしておきたいカウント数を格納しておく変数
int n_cap, n_label, n_bottle;
//angle of motor
int motorAngle_cap;
int motorAngle_label;
int motorAngle_bottle;

//speaker(buzzer)
int speakerPin = 11;

//which_term : cap/label/bottle
int which_term;
int countNum;
#define CAP 0
#define LABEL 1
#define BOTTLE 2

#define RANGE 8
#define THROWED_CAP 10
#define THROWED_LABEL 10
#define THROWED_BOTTLE 10

int period_cap;
int period_label;
int period_bottle;

int phase;

int not_cap;
int not_label;
int not_bottle;

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));

  Motor_cap.attach(motorPin_cap);
  Motor_label.attach(motorPin_label);
  Motor_bottle.attach(motorPin_bottle);

  motorAngle_cap = CLOSE_CAP;
  motorAngle_label = CLOSE_LABEL;
  motorAngle_bottle = CLOSE_BOTTLE;

  Motor_cap.write(motorAngle_cap);
  Motor_label.write(motorAngle_label);
  Motor_bottle.write(motorAngle_bottle);

  n_cap = 0;
  n_label = 0;
  n_bottle = 0;

  distance_cap = 1000;
  just_throwed_cap = 1000;
  distance_label = 1000;
  just_throwed_label = 1000;
  distance_bottle = 1000;
  just_throwed_bottle = 1000;

  which_term = CAP;

  countNum = 0;

  randomSeed(analogRead(0));

  period_cap = 0;
  period_label = 0;
  period_bottle = 0;

  phase = START;

  not_cap = false;
  not_label = false;
  not_bottle = false;
}


void loop() {

  Serial.print(countNum);
  Serial.print(":::");
  Serial.print(n_cap);
  Serial.print(":::");
  Serial.print(distance_cap);
  Serial.print(":::");
  Serial.println(phase);

  distance_cap = Us_judge_hand_cap.MeasureInCentimeters();
  just_throwed_cap = Us_throwed_cap.MeasureInCentimeters();

  distance_label = Us_judge_hand_label.MeasureInCentimeters();
  just_throwed_label = Us_throwed_label.MeasureInCentimeters();

  distance_bottle = Us_judge_hand_bottle.MeasureInCentimeters();
  just_throwed_bottle = Us_throwed_bottle.MeasureInCentimeters();

  if (phase == START && distance_bottle < 50 && RANGE < distance_bottle) {
    motorAngle_cap = LITTLE_OPEN_CAP;
    motorAngle_label = LITTLE_OPEN_LABEL;
    motorAngle_bottle = LITTLE_OPEN_BOTTLE;
    n_cap = countNum;
    n_label = countNum;
    n_bottle = countNum;

    phase = ALL_PATA;
  }

  if (phase == ALL_PATA) {
    //パタパタ関数で全員パタパタさせる
    patapata(true, true, true);
    //もしどれかのセンサが反応したときphaseをCAP_STARTにする
    if (distance_label < RANGE || distance_bottle < RANGE) {
      motorAngle_cap = LITTLE_OPEN_CAP;
      n_cap = countNum;
      phase = CAP_START;
    } else if (distance_cap < RANGE) {
      phase = CAP_START;
    }

  }

  if (phase == CAP_START) {
    //CAP以外に近づけたら→CAPパタパタ
    if ((distance_label < RANGE || distance_bottle < RANGE) && (not_cap == false)) {
      not_cap = true;
    }
    if (not_cap == true) {
      patapata(true, false, false);
      Motor_label.write(CLOSE_LABEL);
      Motor_bottle.write(CLOSE_BOTTLE);
    }
    if (distance_cap < RANGE) {
      //CAPに近づけたら→CAPオープン
      Motor_cap.write(FULL_OPEN_CAP);
      Motor_label.write(CLOSE_LABEL);
      Motor_bottle.write(CLOSE_BOTTLE);
      phase = CAP_OPEN;
    }
  }

  if (phase == CAP_OPEN) {
    //開けたままで捨てられるのを待つ
    //捨てられたらLABEL_STARTに
    if (just_throwed_cap < THROWED_CAP) {
      Motor_cap.write(CLOSE_CAP);
      motorAngle_label = LITTLE_OPEN_LABEL;
      n_label = countNum;
      phase = LABEL_START;
    }
  }

  if (phase == LABEL_START) {
    //LABELパタパタ
    patapata(false, true, false);
    Motor_cap.write(CLOSE_CAP);
    Motor_bottle.write(CLOSE_BOTTLE);
    if (distance_label < RANGE) {
      //CAPに近づけたら→CAPオープン
      Motor_cap.write(CLOSE_CAP);
      Motor_label.write(FULL_OPEN_LABEL);
      Motor_bottle.write(CLOSE_BOTTLE);
      phase = LABEL_OPEN;
    }
  }

  if (phase == LABEL_OPEN) {
    //捨てられたらBOTTLE_STARTに
    if (just_throwed_label < THROWED_LABEL) {
      Motor_label.write(CLOSE_LABEL);
      motorAngle_bottle = LITTLE_OPEN_BOTTLE;
      n_bottle = countNum;
      phase = BOTTLE_START;
    }
  }

  if (phase == BOTTLE_START) {
    //BOTTLEパタパタ
    patapata(false, false, true);
    Motor_cap.write(CLOSE_CAP);
    Motor_label.write(CLOSE_LABEL);
    if (distance_bottle < RANGE) {
      Motor_cap.write(CLOSE_CAP);
      Motor_label.write(CLOSE_LABEL);
      Motor_bottle.write(FULL_OPEN_BOTTLE);
      phase = BOTTLE_OPEN;
    }
  }

  if (phase == BOTTLE_OPEN) {
    //捨てられたらしまってthankyou
    if (just_throwed_bottle < THROWED_BOTTLE) {
      Motor_bottle.write(CLOSE_BOTTLE);
      phase = THANKYOU;
    }
  }

  if (phase == THANKYOU) {
    //thankyouして
    thankYou(Motor_cap, Motor_label, Motor_bottle);
    phase = START;
    countNum = 0;
  }

  countNum++;
}

void patapata (int cap, int label, int bottle) {

  if (cap) {
    if (motorAngle_cap == LITTLE_OPEN_CAP) { //パタパタの「パ」の角度で
      period_cap = random(2, 5);
      if ((countNum - n_cap) > period_cap) { //かつ「パ」になったタイミングから100カウント済んでいたら
        motorAngle_cap = CLOSE_CAP;
        n_cap = countNum;
      }
    } else if (motorAngle_cap == CLOSE_CAP) { //パタパタの「タ」の角度で
      period_cap = random(5, 10);
      if ((countNum - n_cap) > period_cap) { //100カウント済んでいたら
        motorAngle_cap = LITTLE_OPEN_CAP;
        n_cap = countNum;
      }
    }
    Motor_cap.write(motorAngle_cap);
  }
  if (label) {
    if (motorAngle_label == LITTLE_OPEN_LABEL) { //パタパタの「パ」の角度で
      period_label = random(2, 5);
      Serial.println("た１");
      if ((countNum - n_label) > period_label) { //かつ「パ」になったタイミングから100カウント済んでいたら
        motorAngle_label = CLOSE_LABEL;
        Serial.println("た");
        n_label = countNum;
      }
    } else if (motorAngle_label == CLOSE_LABEL) { //パタパタの「タ」の角度で
      period_label = random(10, 25);
      Serial.println("ぱ１");
      if ((countNum - n_label) > period_label) { //100カウント済んでいたら
        motorAngle_label = LITTLE_OPEN_LABEL;
        Serial.println("ぱ");
        n_label = countNum;
      }
    }
    Motor_label.write(motorAngle_label);
  }
  if (bottle) {
    if (motorAngle_bottle == LITTLE_OPEN_BOTTLE) { //パタパタの「パ」の角度で
      period_bottle = random(5, 10);
      if ((countNum - n_bottle) > period_bottle) { //かつ「パ」になったタイミングから100カウント済んでいたら
        motorAngle_bottle = CLOSE_BOTTLE;
        n_bottle = countNum;
      }
    } else if (motorAngle_bottle == CLOSE_BOTTLE) { //パタパタの「タ」の角度で
      period_bottle = random(20, 30);
      if ((countNum - n_bottle) > period_bottle) { //100カウント済んでいたら
        motorAngle_bottle = LITTLE_OPEN_BOTTLE;
        n_bottle = countNum;
      }
    }
    Motor_bottle.write(motorAngle_bottle);
  }
}

void thankYou(Servo mc, Servo ml, Servo mb) {
  mc.write(CLOSE_CAP);
  ml.write(CLOSE_LABEL);
  mb.write(CLOSE_BOTTLE);
  //ぱぱぱ、
  delay(500);
  mc.write(YEAH_CAP);
  delay(500);
  ml.write(YEAH_LABEL);
  delay(500);
  mb.write(YEAH_BOTTLE);
  delay(1000);
  //ぱぱぱ、
  mb.write(CLOSE_BOTTLE);
  delay(500);
  ml.write(CLOSE_LABEL);
  delay(500);
  mc.write(CLOSE_CAP);
  delay(1000);
  //ぱ、ぱ、ぱ！
  mc.write(YEAH_CAP);
  ml.write(LITTLE_OPEN_LABEL);
  mb.write(YEAH_BOTTLE);
  delay(1000);
  mc.write(LITTLE_OPEN_CAP);
  ml.write(YEAH_LABEL);
  mb.write(LITTLE_OPEN_BOTTLE);
  delay(1000);
  mc.write(YEAH_CAP);
  ml.write(YEAH_LABEL);
  mb.write(YEAH_BOTTLE);
  delay(1000);
  mc.write(CLOSE_CAP);
  ml.write(CLOSE_LABEL);
  mb.write(CLOSE_BOTTLE);
  delay(10000);
}
