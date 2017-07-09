// Текст для вывода на кластер модулей вводится через консоль

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>

int pinCS = 10; // Attach CS to this pin, DIN to MOSI and CLK to SCK
int numberOfHorizontalDisplays = 1;
int numberOfVerticalDisplays = 4;

Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

String tape;             // строка для отображения
int wait = 48;          // Задержка в мс перед сдвигом картинки
int spacer = 1;          // расстояние в точках меджу символами
int width = 5 + spacer;  // Ширина шрифта 5 точек


String utf8rus(String source) // Перекодировка для поддержки русского языка в скетче
                              // через СОМ порт с клавиатуры и так русский работает
{
  int i, k;
  String target;
  unsigned char n;
  char m[2] = { '0', '\0' };

  k = source.length(); i = 0;
  while (i < k) {
    n = source[i]; i++;
    if (n >= 0xC0) {
      switch (n) {
        case 0xD0: {
            n = source[i]; i++;
            if (n == 0x81) {
              n = 0xA8;
              break;
            }
            if (n >= 0x90 && n <= 0xBF) n = n + 0x2F;
            break;
          }
        case 0xD1: {
            n = source[i]; i++;
            if (n == 0x91) {
              n = 0xB7;
              break;
            }
            if (n >= 0x80 && n <= 0x8F) n = n + 0x6F;
            break;
          }
      }
    }
    m[0] = n; target = target + String(m);
  }
  return target;
}

String Serial_Read() {
  unsigned char c;                 // переменная для чтения сериал порта
  String Serial_string = "";       // Формируемая из символов строка
  while (Serial.available() > 0) { // Если в сериал порту есть символы
    c = Serial.read();             // Читаем символ
    if (c == '\n') {               // Если это конец строки
      return Serial_string;        // Возвращаем строку
    }
    //if (c == 0xB8) c = c - 0x01;   // Коррекция кодов символа под таблицу, так как русские символы в
    // таблице сдвинуты относительно стандартной кодировки utf на 1 символ
    //if (c >= 0xBF && c <= 0xFF) c = c - 0x01;
    Serial_string = Serial_string + String(char(c)); //Добавить символ в строку
  }
  return utf8rus(Serial_string);
}


void setup() {

  Serial.begin(9600);

  tape = utf8rus("Введите текст"); // Строка отображаемая до ввода с клавы новой
  // Как раз случай вывода русского текста из программы

  matrix.setRotation(matrix.getRotation() + 1); // Поворот отображаемой картинки на 90 градусов
  matrix.setIntensity(4); // Яркость матрицы от 0 до 15
}


void loop() {

  if (Serial.available()) {
    tape = Serial_Read();
  }

  // стандартный вывод из примера библиотеки МАХ72ххPanel
  for ( int i = 0 ; i < width * tape.length() + matrix.width() - spacer; i++ ) {

    matrix.fillScreen(LOW);  // Задание цвета фона экрана (вне букв) LOW - темный, HIGH - светлый

    int letter = i / width;
    int x = (matrix.width() - 1) - i % width;
    int y = (matrix.height() - 8) / 2; // center the text vertically

    while ( x + width - spacer >= 0 && letter >= 0 ) {
      if ( letter < tape.length() ) {
        matrix.drawChar(x, y, tape[letter], HIGH, LOW, 1); // Задание цветов вывода надписи.
                                                           // Первый параметр HIGH цвет символов
                                                           // Второй LOW цвет фона
                                                           // Для инверсии поменять HIGH на LOW
                                                           // и LOW на HIGH
      }

      letter--;
      x -= width;
    }

    matrix.write(); // Отправка на экран

    delay(wait);

  }

delay(2000);

}

