/* 
Перед загрузкой данной программы, неоходимо обнулмть eeprom, т.е. записать во все ячейки eeprom 0.
*/
#include <EEPROM.h>
#include <LiquidCrystal.h> //Подключаем библиотеку для работы с LCD

volatile unsigned long counter_1 = 699 ;  // Начальные показания Холодной воды
volatile unsigned long counter_2 = 696;  // Начальнаые показания Горячей воды
unsigned long counter_max = 70000;  //Максимальное значения которое хранится в одном банке памяти
#define rashod 1 //Литры за один импульс *10
#define pin1 2   // 2-й цифровой пин, нулевое прерывание Холодная вода
#define pin2 3   // 3-й цифровой пин, первое прерывание Горячая Вода
#define pinButton 10 //Кнопка 1
#define pinButton2 11 //Кнопка 2
int counter1_byte = 0; //Ячейка eeprom для хранения данных Холодной воды
int counter2_byte = 64;  //Ячейка eeprom для хранения данных Горячей воды
int adress1_byte = 128; //Адрес хранения адреса хранения данных холодной воды))
int adress2_byte = 130; //Адрес хранения адреса хранения данных горячей воды)))
int adress1 = 0;
int adress2 = 0;
long previousMillis = 0;
int val=0;
volatile unsigned long newcounter_1 = 0;
volatile unsigned long newcounter_2 = 0;
LiquidCrystal lcd(9, 8, 7, 6, 5, 4);
                                    /*LCD RS pin to digital pin 9
                                    LCD Enable pin to digital pin 8
                                    LCD D4 pin to digital pin 7
                                    LCD D5 pin to digital pin 6
                                    LCD D6 pin to digital pin 5
                                    LCD D7 pin to digital pin 4*/
//static unsigned long millis_prev = millis();
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void setup() {
    Serial.begin(9600);
    lcd.begin(16, 2);
    lcd.clear();
                   if (EEPROM.read(150)==0){ // Если первый запуск то запишем в ячейки адреса где хранятся данные
                          if (counter_1 >= counter_max && counter_1<=(counter_max*2)-1){
                          counter1_byte = 4; 
                          }
                          if (counter_1>=counter_max*2 && counter_1<=(counter_max*3)-1){
                          counter1_byte = 8; 
                          }
                          if (counter_1>=counter_max*3 && counter_1<=(counter_max*4)-1){
                          counter1_byte = 12; 
                          }
                          
                          if (counter_2>=counter_max && counter_2<=(counter_max*2)-1){
                          counter2_byte = counter2_byte + 4; 
                          }
                          if (counter_2>=counter_max*2 && counter_2<=(counter_max*3)-1){
                          counter2_byte = counter2_byte + 8; 
                          }
                          if (counter_2>=counter_max*3 && counter_2<=(counter_max*4)-1){
                          counter2_byte = counter2_byte + 12; 
                          }
                     EEPROM.write(adress1_byte, counter1_byte);
                     EEPROM.write(adress2_byte, counter2_byte);
                     EEPROMWriteInt(counter1_byte, counter_1);  //Пишем показания счетчика в eeprom из переменной
                     EEPROMWriteInt(counter2_byte, counter_2);  //Пишем показания счетчика в eeprom из переменной
                     EEPROM.write(150, 1);
                  }
     counter1_byte = EEPROM.read(adress1_byte);
     counter2_byte = EEPROM.read(adress2_byte);
     counter_1 = EEPROMReadInt(counter1_byte);
     counter_2 = EEPROMReadInt(counter2_byte);  

    //Настраиваем цифровой вход
    pinMode(13, OUTPUT);
    pinMode(pinButton, INPUT); // Порт для кнопки 1
    digitalWrite(pinButton, 1);
    pinMode(pinButton2, INPUT); // Порт для кнопки 2
    digitalWrite(pinButton2, 1);
    pinMode(pin1, INPUT);      // Сюда будем подключать подтягивающий резюк
    pinMode(pin2, INPUT);      // Сюда будем подключать подтягивающий резюк
    digitalWrite(pin1, 1);  // "Подключаем" подтягивающий резистор (реализован внутри ATmega)
    digitalWrite(pin2, 1);  // "Подключаем" подтягивающий резистор (реализован внутри ATmega)
    attachInterrupt(0, count_1, FALLING); // задаём обработчик прерывания 0 (2-й пин). 
                                        // прерывание будет при изменении уровня с HIGHT на LOW
                                        // вызывать функицю count
    attachInterrupt(1, count_2, FALLING); // задаём обработчик прерывания 1 (3-й пин). 
                                        // прерывание будет при изменении уровня с HIGHT на LOW
                                        // вызывать функицю count
  }
///////////////////////////////////ОСНОВНОЙ ЦИКЛ/////////////////////////////////////////////  
void loop(){
             if(digitalRead(pinButton)==HIGH){ //Когда нажата кнопка 1
                if (millis() - previousMillis > 1000){ //Если от предыдущего срабатывания прошло больше 100 миллисекунд
                    previousMillis = millis(); //Запоминаем время первого срабатывания
                    digitalWrite(13, 1); //Включение подсветки дисплея
                    }
                }
                 else {//когда не нажата
                    digitalWrite(13, 0); //Выключение подсветки дисплея
                     }
                   // delay(100);
                    lcd.setCursor(0, 0);     // устанавливаем курсор в 0-ом столбце, 1 строке (начинается с 0)
                    lcd.print("Cold=");
                    lcd.print(counter_1);
                    lcd.setCursor(0, 1);     // устанавливаем курсор в 0-ом столбце, 2 строке
                    lcd.print("Hot =");
                    lcd.print(counter_2);
                
                if (Serial.available() > 0) {
                   if (Serial.find("Status")) { //Ищем на входе символ "C" Если нашли получаем данные 
                   for (int i=0; i <= 1; i++){
                        Serial.print("C");
                        Serial.print(counter_1); // выводим в консоль значение счётчика
                        Serial.print("H");
                        Serial.print(counter_2); // выводим в консоль значение счётчика
                        Serial.print("$");
                    }
                   }
                     if (Serial.find("CHG")) { //Ищем на входе символ "C" Если нашли получаем данные 
                       unsigned long change_counter1 = Serial.parseInt(); // Парсинг числа во входящем потоке
                       counter_1 = change_counter1;
                       unsigned long change_counter2 = Serial.parseInt(); // Парсинг числа во входящем потоке
                       counter_2 = change_counter2;
                       if (counter_1 > 0 && counter_2 > 0){
                          if (counter_1 >= counter_max && counter_1<=(counter_max*2)-1){
                          counter1_byte = 4; 
                          }
                          if (counter_1>=counter_max*2 && counter_1<=(counter_max*3)-1){
                          counter1_byte = 8; 
                          }
                          if (counter_1>=counter_max*3 && counter_1<=(counter_max*4)-1){
                          counter1_byte = 12; 
                          }
                          
                          if (counter_2>=counter_max && counter_2<=(counter_max*2)-1){
                          counter2_byte = counter2_byte + 4; 
                          }
                          if (counter_2>=counter_max*2 && counter_2<=(counter_max*3)-1){
                          counter2_byte = counter2_byte + 8; 
                          }
                          if (counter_2>=counter_max*3 && counter_2<=(counter_max*4)-1){
                          counter2_byte = counter2_byte + 12; 
                          }
                     EEPROM.write(adress1_byte, counter1_byte);
                     EEPROM.write(adress2_byte, counter2_byte);
                     EEPROMWriteInt(counter1_byte, counter_1);  //Пишем показания счетчика в eeprom из переменной
                     EEPROMWriteInt(counter2_byte, counter_2);  //Пишем показания счетчика в eeprom из переменной
                       }  
                  }
                }

                  if (counter_1 != newcounter_1 || counter_2 != newcounter_2){  
                    detachInterrupt (0);
                    detachInterrupt (1);
                    for (int i=0; i <= 1; i++){
                        Serial.print("C");
                        Serial.print(counter_1); // выводим в консоль значение счётчика
                        Serial.print("H");
                        Serial.print(counter_2); // выводим в консоль значение счётчика
                        Serial.print("$");
                        newcounter_1 = counter_1;
                        newcounter_2 = counter_2;
                    }
                  attachInterrupt(0, count_1, FALLING);  
                  attachInterrupt(1, count_2, FALLING);
                  }
}
//////////////////////////////////ФУНКЦИИ СЧЕТЧИКА//////////////////////////////////////
void count_1(){
    detachInterrupt (0);
  // вызывается прерыванием 0 от 2-го цифрового входа
  static unsigned long millis_prev;
        if(millis()-50 > millis_prev) { //Защита от дребезга контаков
          counter_1=counter_1+rashod;  // ясен перец, увеличиваем переменную. Утекло ещё 10 литров    
            if (counter_1>=counter_max && counter_1<=(counter_max*2)-1){
              counter1_byte = counter1_byte + 4; 
              EEPROM.write(adress1_byte, counter1_byte);
            }
            if (counter_1>=counter_max*2 && counter_1<=(counter_max*3)-1){
              counter1_byte = counter1_byte + 8; 
              EEPROM.write(adress1_byte, counter1_byte);
            }
            if (counter_1>=counter_max*3 && counter_1<=(counter_max*4)-1){
              counter1_byte = counter1_byte + 12; 
              EEPROM.write(adress1_byte, counter1_byte);
            } 
              EEPROMWriteInt(counter1_byte, counter_1); //Пишем данные счетчика в энергонезависимую память
              millis_prev = millis();
           }
            attachInterrupt(0, count_1, FALLING);
}
void count_2(){
   detachInterrupt (1);
  // вызывается прерыванием 1 от 3-го цифрового входа
 static unsigned long millis_prev;
      if(millis()-50 > millis_prev) { //Защита от дребезга контаков
        counter_2=counter_2+rashod;  // ясен перец, увеличиваем переменную. Утекло ещё 10 литров
             if (counter_2>=counter_max && counter_2<=(counter_max*2)-1){
               counter2_byte = counter2_byte + 4; 
              EEPROM.write(adress2_byte, counter2_byte);
             }
             if (counter_2>=counter_max*2 && counter_2<=(counter_max*3)-1){
               counter2_byte = counter2_byte + 8; 
              EEPROM.write(adress2_byte, counter2_byte);
             }
             if (counter_2>=counter_max*3 && counter_2<=(counter_max*4)-1){
               counter2_byte = counter2_byte + 12; 
              EEPROM.write(adress2_byte, counter2_byte);
             }
            EEPROMWriteInt(counter2_byte, counter_2); //Пишем данные счетчика в энергонезависимую память
            millis_prev = millis();
      }
     attachInterrupt(1, count_2, FALLING);
}
////////////////////////////////////ФУНКЦИИ ПАМЯТИ///////////////////////////////////////////
//кушаем аж 4 байта EEPROM
void EEPROMWriteInt(int p_address, unsigned long p_value){
    byte four = (p_value & 0xFF);
    byte three = ((p_value >> 8) & 0xFF);
    byte two = ((p_value >> 16) & 0xFF);
    byte one = ((p_value >> 24) & 0xFF);
    EEPROM.write(p_address, four);
    EEPROM.write(p_address + 1, three);
    EEPROM.write(p_address + 2, two);
    EEPROM.write(p_address + 3, one);
}
// считаем нашы 4 байта
unsigned long EEPROMReadInt(int p_address){
   long four = EEPROM.read(p_address);
   long three = EEPROM.read(p_address + 1);
   long two = EEPROM.read(p_address + 2);
   long one = EEPROM.read(p_address + 3);
   return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
 }
