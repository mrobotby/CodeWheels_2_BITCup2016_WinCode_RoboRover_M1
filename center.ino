/*В роботе используются пины (для Arduino Uno-подобной платы)
   Мотордрайвер использует пины:
   D2, D3, D5, D8, D10, D11
   Сервопривод использует пины:
   D9
   Ультразвуковой датчик использует пины:
   D4, D7
   Свободные цифровые пины:
   D0, D1, D6, D12, D13
   Аналоговые ИК-датчики используют пины:
   A0, A1
   Свободные аналоговые пины:
   A2, A3, A4, A5
*/

#include <CurieBLE.h> //Подключаем библиотеку по работе с Bluetooth LE
#include "RoverM1.h" // Подключаем библиотеку для работы с механизмами RoverM1

BLEPeripheral blePeripheral;  // BLE Peripheral Device (Сама плата)
BLEService bleService("19B10000-E8F2-537E-4F6C-D104768A1214"); // Сервис
BLEUnsignedCharCharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite); // характеристка на чтение/запись char (заменить на boolean?)

//Создаем экземпляры классов, отвечающих за механизмы робота
IRMeasurer irLeft = IRMeasurer(0);
IRMeasurer irRight = IRMeasurer(1);
USMeasurer us = USMeasurer(7, 4);

Motor leftMotor = Motor(5, 2, 8);
Motor rightMotor = Motor(3, 10, 11);
DriveSystem driveSystem = DriveSystem(&leftMotor, &rightMotor);

//Переменные по хранению расстояний с переднего левого и правого ИК-датчиков
int irLeftDist, irRightDist;

//Переменные по хранению значений с УЗ датчика
int usCenterDist;
//Функция по измерению расстояний с переднего левого и правого ИК-датчиков
void irMeasure() {
  //Меряем расстояние с правого ИК
  irRightDist = irRight.getDistance();
  //Меряем расстояние с левого ИК
  irLeftDist = irLeft.getDistance();
}

void usMeasure() {
  // расстояние с УЗ датчика
  usCenterDist = us.getDistance();
}

// Функция setup() выполняется каждый раз, когда будет перезапущена подача питания
// или будет произведена перезагрузка платы
void setup() {
  Serial.begin(9600);

  blePeripheral.setLocalName("5_VSTU"); // имя робота
  blePeripheral.setAdvertisedServiceUuid(bleService.uuid());

  blePeripheral.addAttribute(bleService);
  blePeripheral.addAttribute(switchCharacteristic);

  switchCharacteristic.setValue(0); // установка значения по умолчанию
  blePeripheral.begin();
}

int corSpeedL, corSpeedR; // скорости для поворота
int speedL, speedR; // текущая скорость колес
int deadLock = 25; // расстояние с ИК, с которого считается, что робот стоит "лицом в стене"
int deadLockUS = 8; // расстояние с УЗ, с которого считается, что робот стоит "лицом в стене"
int rangeL = 40; // коэффициент корректировки налево
int rangeR = 60;// коэффициент корректировки направо
// Данные коэффициенты отличаются, так как на малых напряжениях левая и правая пары колес
// при одинаковой длине импульса ШИМ имеют различные угловые скорости.

boolean inDeadlock() { // проверка на то что робот стоит "лицом в стене"
  return irLeftDist <= deadLock && irRightDist <= deadLock && usCenterDist <= deadLockUS;
}

//Главный бесконечный цикл, в котором находятся управляющие операторы и циклы
void loop() {
  //Начало подключения Bluetooth
  BLECentral central = blePeripheral.central(); // проверка подключения
  if (central) { // если удалось подключится
    while (central.connected()) { // пока подключено
      if (switchCharacteristic.written()) { // если получено значение
        while (1) { // бесконечный цикл
          if (switchCharacteristic.value()) { // в зависимости от того, какое получено значение с BLE (0x00 или 0x01)
            usMeasure(); // измерить расстояние с УЗ
            irMeasure(); // измерить расстояние с ИК

            if (inDeadlock()) { // если мы уперлись в стену
              driveSystem.setSpeed(-255, -255); // ехать назад
              delay(400); // 400 мс
              driveSystem.setSpeed(-255, 255); // разворот налево (за счет разности угловых скоростей)
              delay(300); // в течение 300 мс
            } else { // если мы не уперлись в стену
              // вычисляем скорости колес как разность максимальной скорости и разности расстояний, взятой с коэффициентом (255/rangeL)
              corSpeedL = 255 - 255 * (irLeftDist - irRightDist) / rangeL;
              corSpeedR = 255 - 255 * (irRightDist - irLeftDist) / rangeR;
              if (corSpeedL > 255) { // проверка на максимальную скорость
                corSpeedL = 255;
              } else if (corSpeedL < -255) { // проверка на минимальную скорость
                corSpeedL = -255;
              }
              if (corSpeedR > 255) {
                corSpeedR = 255;
              } else if (corSpeedR < -255) {
                corSpeedR = -255;
              }
              if (irLeftDist == irRightDist) { // если расстояния одинаковые, едем вперед
                speedL = 255;
                speedR = 255;
              } else if (irLeftDist < irRightDist) { // если слева расстояния больше, устанавливаем на левые колеса вычисленную скорость
                speedL = 255;
                speedR = corSpeedR;
              } else { // и аналогично, если расстояние больше справа
                speedL = corSpeedL;
                speedR = 255;
              }
            }
            driveSystem.setSpeed(speedL, speedR); // установка скоростей непосредственно на двигатели
          } else { // если по BLE передан 0
            driveSystem.setSpeed(0, 0); // остановить робота
          }
        }
      }
    }
  }
}
