/*
 * Константы
 */
const int PWM_LED_PIN = 3;  // Номер пина, к которому подключен светодиод
const int BUTTON_PIN = 2;   // Номер пина, к которому подключена кнопка
const int STATE_BLINK = 0;  // Состояние "Моргание"
const int STATE_PWM = 1;    // Состояние "ШИМ"
const int PWM_CODE_MIN = 0; // Минимальный код ШИМ (светодиод не горит)
const int PWM_CODE_MAX = 255; // Максимальный код ШИМ (светодиод горит максимально ярко)
const int BUTTON_CLICK_DELAY = 10; // Задержка после детектирования нажатия кнопки, миллисекунды
const int BLINK_PHASE = 250; // Полупериод "Моргания", миллисекунды
const int PWM_CHANGE_DELAY = 3; // Задержка между сменой кода ШИМ в состоянии "ШИМ"

/*
 * Глобальные переменные
 */
int state = STATE_BLINK;    // Текущее состояние системы
int buttonState = LOW;      // Состояние кнопки
int pwmStateCode = PWM_CODE_MIN;       // Код ШИМ в состоянии "ШИМ"
int pwmStateDirection = +1; // +1 - увеличение яркости, -1 - уменьшение
int blinkStateCnt = 0;      // Счетчик миллисекунд в состоянии "Моргание"
int blinkStateCode = PWM_CODE_MIN;     // Код ШИМ в состоянии "Моргание"

/* 
 * Конфигурация 
 */
void setup() {
  pinMode(PWM_LED_PIN, OUTPUT);      // Конфигурация 3го пина как выхода (светодиод)
  pinMode(BUTTON_PIN, INPUT);        // Конфигурация 2го пина как входа (кнопка)
}

/*
 * Смена текущего состояния системы
 */
void switchState()
{
  delay(BUTTON_CLICK_DELAY);
  if (state == STATE_BLINK) {
    state = STATE_PWM;
    blinkStateCnt = 0;
    blinkStateCode = PWM_CODE_MIN;
  } else {
    state = STATE_BLINK;
    pwmStateCode = PWM_CODE_MIN;
    pwmStateDirection = +1;
  }
}

/*
 * Смена значения на светодиоде в состоянии "Моргание"
 */
void switchBlinkValue() {
  blinkStateCnt = 0;
  if (blinkStateCode == PWM_CODE_MIN) {
    blinkStateCode = PWM_CODE_MAX;
  } else if (blinkStateCode == PWM_CODE_MAX) {
    blinkStateCode = PWM_CODE_MIN;
  }
  analogWrite(PWM_LED_PIN, blinkStateCode);
}

/*
 * Основной цикл
 */
void loop() {
  
  // Чтение состояния кнопки
  int buttonStateCurrent = digitalRead(BUTTON_PIN);
  if (buttonState == LOW && buttonStateCurrent == HIGH) { // Переход между состояниями
    switchState();
  }
  buttonState = buttonStateCurrent;

  // Реализация состояний системы
  if (state == STATE_BLINK) { // Состояние "ШИМ"
    pwmStateCode += pwmStateDirection;
    if (pwmStateCode == PWM_CODE_MIN || pwmStateCode == PWM_CODE_MAX) {
      pwmStateDirection *= -1;  // Смена направления: увеличение или уменьшение яркости
    }
    analogWrite(PWM_LED_PIN, pwmStateCode);  // 3 - номер пина, i - код ШИМ (от 0 до 255)
    delay(PWM_CHANGE_DELAY);
  } else { // Состояние "Моргание"
    blinkStateCnt++;
    if (blinkStateCnt == BLINK_PHASE) {
      switchBlinkValue();
    } 
    delay(1);
  }
}
