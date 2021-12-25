/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
                                                            termo_furnace.ino
                                                      Copyright © 2021, Nik.S
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
  Arduino Pro Mini:
  Скетч использует 12176 байт (39%) памяти устройства. Всего доступно 30720 байт.
Глобальные переменные используют 773 байт (37%) динамической памяти, 
оставляя 1275 байт для локальных переменных. Максимум: 2048 байт.
/*****************************************************************************\
  Arduino Pro Mini:
    цифровые:
  - реле (pin D5)
  - кнопка (pin D6)
  - датчик температуры DS18B20 (pin D8)
/*****************************************************************************/
//  Блок DEVICE  --------------------------------------------------------------
//  Arduino Pro Mini
#define DEVICE_ID "termo_furnace"
#define VERSION 2

//  Блок libraries  -----------------------------------------------------------

//#define DS_CHECK_CRC true // отключить проверку подлинности принятых данных
// (может привести к выдаче некорректных измерений)
#include <microDS18B20.h>
#include <GyverOLED.h>
#include "GyverButton.h"

//  Блок settings  ------------------------------------------------------------

#define RELAY_PIN 5
#define DS18_PIN 8  // Датчик один - не используем адресацию
MicroDS18B20 <DS18_PIN> sensor;
int8_t tDS18;
uint8_t tMax = 39;
uint8_t tMin = 39;
#define BUTTON_PIN 6 // кнопка подключена сюда (BUTTON_PIN --- КНОПКА ---1К --- GND)
GButton butt1(BUTTON_PIN);  
// HIGH_PULL - кнопка подключена к GND, пин подтянут к VCC (по умолчанию)

// примеры:
//GyverOLED<SSD1306_128x64, OLED_BUFFER> oled;
GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> oled;


uint8_t statusOled = 0;
uint8_t nHour = 1;
int32_t timeLeft;
uint32_t timerVal ;

//  end init  -----------------------------------------------------------------

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
            setup
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void setup() {

  Serial.begin(9600);

  pinMode(RELAY_PIN, OUTPUT); // инициализируем что выход
  //pinMode(BUTTON_PIN, INPUT_PULLUP); // инициализируем что вход,

  oled.init();  // инициализация
  // настройка скорости I2C //Wire.setClock(800000L);   // макс. 800'000

  // --------------------------
  oled.clear();   // очистить дисплей (или буфер)
  //oled.update();  // обновить. Только для режима с буфером! OLED_BUFFER

  // --------------------------
  oled.home();         // курсор в 0,0
  oled.print("Старт!");// печатай что угодно: числа,строки,float,как Serial!
  oled.update();
  delay(2000);

  // --------------------------
  // СЕРВИС
  oled.setContrast(1);   // яркость 0..15
  //oled.setPower(true);    // true/false - включить/выключить дисплей
  oled.flipH(true);       // true/false - отзеркалить по горизонтали
  oled.flipV(true);       // true/false - отзеркалить по вертикали

  // --------------------------
  oled.setCursor(5, 1);   // курсор в (пиксель X, строка Y)
  oled.setScale(2);
  oled.println("Термопечка");
  oled.print("   запуск");
  oled.update();
  delay(2000);
  // --------------------------
  // MANUAL - нужно вызывать функцию tick() вручную
  // AUTO - tick() входит во все остальные функции и опрашивается сама!
  butt1.setTickMode(AUTO);
  butt1.setTimeout(1111); // настройка таймаута на удержание ( 500 мс)
  //butt1.setClickTimeout(500);// настройка таймаута между кликами ( 300 мс)

}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
            loop
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void loop() {

  sensor.requestTemp();

  ///////   кнопка
  if (butt1.isDouble())         // проверка на двойной клик
  {
    timerVal = millis();
    if (statusOled == 0)
    {
      oled.setPower(true);    // true/false - включить/выключить дисплей
      Serial.println("Включить дисплей");
      statusOled = 1;
    }
  }

  if (millis() - timerVal < 22000)
  {
    if (statusOled == 1)
    {
      oled.setPower(true);    // true/false - включить/выключить дисплей
      Serial.println("Включить дисплей");
      statusOled = 0;
    }

    if (butt1.isSingle())         // проверка на один клик
    {
      Serial.println("   tMin = tMax ");
      tMin = tMax;
    }

    if (butt1.isStep()) 
    {        // если кнопка была удержана (это для инкремента)
      Serial.print("   tMax увеличить");
      tMax++;// увеличивать переменную value с шагом и интервалом
    }

  if (butt1.isStep(1))           // один клик + удержание
    {
      Serial.println(" Знаачение nHour увеличить");
      nHour++;
    }

  }
  else
  {
    if (statusOled == 0)
    {
      oled.setPower(false); // true/false - включить/выключить дисплей
      Serial.println("Выключить дисплей");
      statusOled = 1;
    }
  }

  ///////   реле
    timeLeft = 3600000 * nHour - millis();

  if (timeLeft > 11)
  {
  
  if (digitalRead(RELAY_PIN) == LOW && tDS18 < tMin)
  {
    digitalWrite(RELAY_PIN, HIGH);    // пишется высокий уровень вкл
  }

  if (digitalRead(RELAY_PIN) == HIGH && tDS18 > tMax)
  {
    digitalWrite(RELAY_PIN, LOW);     // пишется низкий уровень выкл
  }
  }
  else if (digitalRead(RELAY_PIN) == HIGH)
  {
    digitalWrite(RELAY_PIN, LOW);     // пишется низкий уровень выкл
  }
  
  ///////   Вывод в Serial
  Serial.print("  +");
  tDS18 = sensor.getTempInt();
  Serial.print(tDS18);
  Serial.println(" °C");
  Serial.print(" tMax = ");
  Serial.println(tMax);
  Serial.println(" ");
  Serial.println(upTime(millis()));

  ///////   Вывод на дисплей
  oled.clear();   // очистить дисплей (или буфер)
  oled.setCursor(11, 0);   // курсор в (пиксель X, строка Y)
  oled.setScale(3);

  oled.print("+");
  oled.print(tDS18);
  oled.println(" *C");

  oled.setCursor(1, 4);   // курсор в (пиксель X, строка Y)
  oled.setScale(2);

  if (digitalRead(RELAY_PIN) == HIGH )
  {
    oled.print(tMin - 1);
    oled.setScale(1);
    oled.print( "    Нагрев   ");
    oled.setScale(2);
    oled.print(tMax + 1);
  }

  if (digitalRead(RELAY_PIN) == LOW )
  {
    oled.print(tMax + 1);
    oled.setScale(1);
    oled.print( "  Остывание  ");
    oled.setScale(2);
    oled.print(tMin - 1);
  }

    oled.setScale(1);
    oled.setCursor(4, 7);   // курсор в (пиксель X, строка Y)
    oled.print(upTime(millis()));
    oled.setCursor(74, 7);   // курсор в (пиксель X, строка Y)
      if (timeLeft > 0)
      {
        oled.print(upTime(timeLeft));
      }
      else
      {
        timeLeft = 0 - timeLeft;
        oled.print("- ");
        oled.print(upTime(timeLeft));
      }
      
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
            end
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/