/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
                                                            termo_furnace.ino
                                                      Copyright © 2021, Nik.S
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
  Arduino Pro Mini:
  Скетч использует 9658 байт (31%) памяти устройства. Всего доступно 30720 байт.
Глобальные переменные используют 727 байт (35%) динамической памяти, 
оставляя 1321 байт для локальных переменных. Максимум: 2048 байт.
/*****************************************************************************\
   Arduino Pro Mini выдает данные:
    цифровые:
  датчик температуры DS18B20 (pin D8)
/*****************************************************************************/
//  Блок DEVICE  --------------------------------------------------------------
//  Arduino Pro Mini
#define DEVICE_ID "termo_furnace"
#define VERSION 1

//  Блок libraries  -----------------------------------------------------------

//#define DS_CHECK_CRC true // отключить проверку подлинности принятых данных
// (может привести к выдаче некорректных измерений)
#include <microDS18B20.h>
#include <GyverOLED.h>
#include "GyverButton.h"

//  Блок settings  ------------------------------------------------------------

// Датчик один - не используем адресацию
#define DS_PIN 8
MicroDS18B20 <DS_PIN> sensor;
int tDS18;

#define BTN_PIN 6 // кнопка подключена сюда (BTN_PIN --- КНОПКА ---1К --- GND)
GButton butt1(BTN_PIN);  
// HIGH_PULL - кнопка подключена к GND, пин подтянут к VCC 

// примеры:
//GyverOLED<SSD1306_128x64, OLED_BUFFER> oled;
GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> oled;

int pinRelay  = 5;

int tMax = 39;
int tMin = 39;
int statusOled = 0;
uint32_t tmr ;

//  end init  -----------------------------------------------------------------

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
            setup
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void setup() {

  Serial.begin(9600);

  pinMode(pinRelay, OUTPUT); // инициализируем что выход
  //pinMode(BTN_PIN, INPUT_PULLUP); // инициализируем что вход,

  oled.init();  // инициализация
  // настройка скорости I2C
  //Wire.setClock(800000L);   // макс. 800'000

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

  if (butt1.isStep(1))           // один клик + удержание
  {
    tmr = millis();
    if (statusOled == 0)
    {
      oled.setPower(true);    // true/false - включить/выключить дисплей
      Serial.println("Включить дисплей");
      statusOled = 1;
    }
  }

  if (millis() - tmr < 22000)
  {
    if (statusOled == 1)
    {
      oled.setPower(true);    // true/false - включить/выключить дисплей
      Serial.println("Включить дисплей");
      statusOled = 0;
    }


    ///////   кнопка

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
  if (digitalRead(pinRelay) == LOW && tDS18 < tMin)
  {
    digitalWrite(pinRelay, HIGH);    // пишется высокий уровень вкл
  }

  if (digitalRead(pinRelay) == HIGH && tDS18 > tMax)
  {
    digitalWrite(pinRelay, LOW);     // пишется низкий уровень выкл
  }


  ///////   Вывод в Serial
  Serial.print("  +");
  tDS18 = sensor.getTempInt();
  Serial.print(tDS18);
  Serial.println(" °C");
  Serial.print(" tMax = ");
  Serial.println(tMax);
  Serial.println(" ");

  /////// дисплей
  oled.clear();   // очистить дисплей (или буфер)
  oled.setCursor(11, 0);   // курсор в (пиксель X, строка Y)
  oled.setScale(3);

  oled.print("+");
  oled.print(tDS18);
  oled.println(" *C");

  oled.setCursor(1, 4);   // курсор в (пиксель X, строка Y)
  oled.setScale(2);

  if (digitalRead(pinRelay) == HIGH )
  {
    oled.print( "   Нагрев");
    oled.setScale(0);
    oled.setCursor(30, 7);   // курсор в (пиксель X, строка Y)
    oled.print(tMin - 1);
    oled.print("   >>>   ");
    oled.print(tMax + 1);
  }

  if (digitalRead(pinRelay) == LOW )
  {
    oled.print( " Остывание");
    oled.setScale(0);
    oled.setCursor(30, 7);   // курсор в (пиксель X, строка Y)
    oled.print(tMin - 1);
    oled.print("   <<<   ");
    oled.print(tMax + 1);
  }

  ///////////////////////////////////////////

}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
            end
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/