#include "RoverM1.h"

//-------Servo-------

Servo::Servo(int newServoPin) {
  servoPin = newServoPin;
  pinMode(servoPin, OUTPUT);
}

void Servo::setAngle(int angle) {
  currentAngle = angle; // сохраняем значение угла
  analogWrite(servoPin, angle); // выводим угол на порт
}

int Servo::getCurrentAngle(){
  return currentAngle; // возвращаем сохраненных угол
}

//-------IRMeasurer-------

IRMeasurer::IRMeasurer(int newIrPin) {
  irPin = newIrPin;
}

int IRMeasurer::getDistance() {
  volts = analogRead(irPin) * voltConst; // измерение напряжения
  if (volts < 0.26) {
    return 120; // если напряжение меньше 0.26, расстояние принимается = 120
  }
  if (volts > 3.00) {
    return 6; // если напряжение меньше 0.26, расстояние принимается = 120
  }
  for (int i = 0; i < 39; i++) { // перебираем массив
    if (val[i] <= volts) { 
      return cm[i]; // возвращаем соответствующее подходящее расстояние
    }
  }
  return 120; // возвращаем максимальное расстояние
}

//------USMeasurer------

USMeasurer::USMeasurer(int newTrigPin, int newEchoPin) {
  trigPin = newTrigPin;
  echoPin = newEchoPin;
  //Установка режимов портов
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

int USMeasurer::getDistance() {
  digitalWrite(trigPin, LOW); //выключение датчика
  delayMicroseconds(2); // ожидание
  digitalWrite(trigPin, HIGH); // включение на 10 мкс
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW); // выключение
  duration = pulseIn(echoPin, HIGH); // получение задержки эха
  delay(25); // "отдых" перед следующим измерением
  return duration / 58.2; // вычисление дистанции
}

//------Motor------
Motor::Motor(int newPwmPin, int newPin1, int newPin2) {
  pwmPin = newPwmPin;
  pin1 = newPin1;
  pin2 = newPin2;
  // утсановка режима портов
  pinMode(pwmPin, OUTPUT);
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
}

void Motor::setForward() { // установка прямой полярности
  direction = 1;
  digitalWrite(pin1, LOW); // ОБЯЗАТЕЛЬНО СНАЧАЛА ВЫКЛЮЧЕНИЕ для предотвращение порчи драйвера
  digitalWrite(pin2, HIGH);
}

void Motor::setReverse() { // установка обратной полярности
  direction = -1;
  digitalWrite(pin2, LOW);
  digitalWrite(pin1, HIGH);
}

void Motor::stop() { // выключение ШИМ и подачи управляющего напряжения
  direction = 0;
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
  setSpeed(0);
}

void Motor::setSpeed(int speed) { // утсановка длины импульса ШИМ
  analogWrite(pwmPin, speed);
}

//-----DriveSystem------
DriveSystem::DriveSystem(Motor* newLeftMotor, Motor* newRightMotor) {
  leftMotor = newLeftMotor;
  rightMotor = newRightMotor;
}

int DriveSystem::getLeftSpeed() {
  return leftSpeed;
}

int DriveSystem::getRightSpeed() {
  return rightSpeed;
}

void DriveSystem::setSpeed(int leftSpeed, int rightSpeed) { // утсановка скорости
  if (leftSpeed == 0) { // если передан 0 на левый двигатель
    leftMotor->stop(); // остановка
  } else if (leftSpeed < 0) { // если передано значение меньше нуля
    leftMotor->setReverse(); // переполюсовка двигателя на реверс
    leftMotor->setSpeed(-leftSpeed); // установка ШИМ на заданную длину импульса
  } else {
    leftMotor->setForward(); // прямая полюсовка двигателя
    leftMotor->setSpeed(leftSpeed); // установка ШИМ на заданную длину импульса
  }
  if (rightSpeed == 0) { // аналогично для правого мотора
    rightMotor->stop();
  } else if (rightSpeed < 0) {
    rightMotor->setReverse();
    rightMotor->setSpeed(-rightSpeed);
  } else {
    rightMotor->setForward();
    rightMotor->setSpeed(rightSpeed);
  }
  // сохранение переданных скоростей
  this->leftSpeed = leftSpeed;
  this->rightSpeed = rightSpeed;
}

