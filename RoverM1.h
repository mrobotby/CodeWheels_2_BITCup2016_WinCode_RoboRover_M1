#include <Arduino.h>

// класс для управления сервоприводом
class Servo {
  public:
    Servo(int); // конструктор

    static const int FRONT_ANGLE = 185; //Значение для угла в 90 градусов (от 0 до 255)
    static const int LEFT_ANGLE = 163; //Значение для левого положения
    static const int RIGHT_ANGLE = 207; //Значение для правого положения

    void setAngle(int angle); //перемещение привода на указанный угол
    int getCurrentAngle();
  private:
    int servoPin = 9; //пин сервопривода
    int currentAngle; // текущее положение

};

//Класс для измерения расстояния с ИК-датчика робота
class IRMeasurer {
  public:
    IRMeasurer(int); // конструктор

    int getDistance(); // получение расстояния с датчика
  private:
    int irPin; // пин датчика
    int distance; //текущее расстояние (функциональная переменная, используется в качестве буфера для экономии памяти)
    double volts;
    //Экспериментальные значения в вольтах для определенного расстояния
    float val[40] = { 2.73, 2.34, 1.99, 1.76, 1.57, 1.42, 1.29, 1.20, 1.07, 1.00, 0.94, 0.88, 0.82, 0.79, 0.74, 0.71, 0.68, 0.65, 0.63, 0.61, 0.58, 0.55, 0.53, 0.50, 0.51, 0.49, 0.48, 0.47, 0.45, 0.44, 0.42, 0.41, 0.40, 0.39, 0.38, 0.37, 0.36, 0.33, 0.28, 0.26};
    int cm[40] = {8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 66, 68, 70, 72, 74, 76, 78, 80, 90, 100, 110};
    static constexpr float voltConst = 0.0033; // дискрета опорного напряжения (3,3V / 1024)
};

//Класс для измерений с помощью УЗ-датчика
class USMeasurer {
  public:
    int trigPin = 7; // пин подачи напряжения
    int echoPin = 4; // пин эха

    int getDistance(); // получение расстояния с датчика

    USMeasurer(int, int); // конструктор
  private:
    long duration; // (функциональная переменная, используется в качестве буфера для экономии памяти)
};

// Класс для управления движением
class Motor {
  public:
    Motor(int, int, int); // конструктор
    void setForward(); // установка движения вперед
    void setReverse(); // установка движения назад
    void stop();
    void setSpeed(int); // установка скорости
    byte getDirection(); // получение направления (+1, -1, 0)
  private:
    byte direction; // направление
    int pwmPin; //ШИМ-выход
    int pin1, pin2; //пины направления (+,- в случае движения вперед)
};

//Класс для управления движением
class DriveSystem {
  public:
    DriveSystem(Motor*, Motor*); // конструктор

    int getLeftSpeed(); // получение левой скорости
    int getRightSpeed(); // получение правой скорости

    void setSpeed(int, int); // метод установки скорости
  private:
    //ссылки на моторы
    Motor *leftMotor;
    Motor *rightMotor;

    //хранимые скорости
    int leftSpeed;
    int rightSpeed;
};
