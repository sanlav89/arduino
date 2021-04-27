#include <Arduino_FreeRTOS.h>

TaskHandle_t taskButtonHandle;
// TaskHandle_t taskSerialHandle;

/*
 * Константы
 */
const int BUTTON_PIN = 2;   // Номер пина, к которому подключена кнопка
const int STATE_BLINK = 0;  // Состояние "Моргание"
const int STATE_PWM = 1;    // Состояние "ШИМ"
const int BUTTON_CLICK_DELAY = 20; // Задержка после детектирования нажатия кнопки, миллисекунды
const int DIGITAL_LED_PIN = 13;

/*
 * Глобальные переменные
 */
int state = STATE_BLINK;    // Текущее состояние системы
int buttonState = LOW;      // Состояние кнопки

/*
 * Инициализация тасков
 */
void setup() {
  // put your setup code here, to run once:
  xTaskCreate(TaskButton, // Task function
              "Button", // Task name
              128, // Stack size 
              NULL, 
              0, // Priority
              &taskButtonHandle); // Task handler
  xTaskCreate(TaskSerial, // Task function
              "Serial", // Task name
              128, // Stack size 
              NULL, 
              1, // Priority
              NULL); // Task handler
}

/*
 * Смена текущего состояния системы (вызывается в таске кнопки)
 */
void switchState()
{
  if (state == STATE_BLINK) {
    state = STATE_PWM;
    digitalWrite(DIGITAL_LED_PIN, LOW);  // Гасим
  } else {
    state = STATE_BLINK;
    digitalWrite(DIGITAL_LED_PIN, HIGH);  // Гасим
  }
}

/*
 * Сглаживание дребезга (вызывается в таске кнопки)
 */
int debounce(int last) {
  int current = digitalRead(BUTTON_PIN);
  if (last != current) {
    vTaskDelay(BUTTON_CLICK_DELAY / portTICK_PERIOD_MS);
    current = digitalRead(BUTTON_PIN);
  }
  return current;
}

/*
 * Таск для обработки кнопки
 */
void TaskButton(void *pvParameters)
{
  (void) pvParameters;
  
  // Начальные настройки, инициализация
  pinMode(BUTTON_PIN, INPUT);        // Конфигурация 2го пина как входа (кнопка)
  pinMode(DIGITAL_LED_PIN, OUTPUT);  // Конфигурация 13го пина как выхода (светодиод на плате)

  // Основной цикл
  while (true) {
    // Чтение состояния кнопки
    int buttonStateCurrent = debounce(buttonState);
    if (buttonState == LOW && buttonStateCurrent == HIGH) { // Переход между состояниями
      switchState();
    }
    buttonState = buttonStateCurrent;
  }
}

/*
 * Таск для UART
 */
void TaskSerial(void *pvParameters)
{
  (void) pvParameters;

  // Начальные настройки, инициализация
  Serial.begin(9600);

  // Основной цикл
  while(true) {
    // Проверка, поступил ли запрос по UART
    int flag = 0;
    while(Serial.available()) {
      if (Serial.read() == 0x55) {
        flag = 1;
        break;
      }
    }

    // Если запрос послупил, то отвечаем
    // if (flag == 1) {
      Serial.write(state == STATE_BLINK ? 0x55 : 0xAA);
    // }

    // Задержка. Это какая-то фишка данной RTOS, видимо. Если убрать задержку - ничего работать не будет
    // Предполагаю, что переключение между процессами происходит во время того, как эта задержка активирована
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

/*
 * Привычный нам основной цикл программы пуст. Вся логика теперь в тасках
 */
void loop() {}
