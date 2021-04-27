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
const int BLINK_PHASE = 100; // Полупериод "Моргания", миллисекунды
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
int cnt = 0;

/* 
 * Конфигурация 
 */
void setup() {
  Serial.begin(9600);
  pinMode(PWM_LED_PIN, OUTPUT);      // Конфигурация 3го пина как выхода (светодиод)
  pinMode(BUTTON_PIN, INPUT);        // Конфигурация 2го пина как входа (кнопка)
  pinMode(13, OUTPUT);
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
    digitalWrite(13, LOW);
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
 * Чтение значения кнопки
 */
void checkStateByButton() {
  int buttonStateCurrent = digitalRead(BUTTON_PIN);
  if (buttonState == LOW && buttonStateCurrent == HIGH) { // Переход между состояниями
    switchState();
  }
  buttonState = buttonStateCurrent;
}

/*
 * Запрос состояния по UART
 */
void checkStateByUart() {
  int uartState = state;
  while(Serial.available()) {
    int rd = Serial.read();
    if (rd == 0x55) {
      uartState = STATE_BLINK;
      digitalWrite(13, HIGH);
    } else if (rd == 0xAA) {
      uartState = STATE_PWM;
      digitalWrite(13, LOW);
    }
  }
  
  if (state != uartState) {
    switchState();
  }
//  delay(10);
cnt++;
if (cnt == 100) {
  Serial.write(0x55);
  cnt = 0;
}
}


/*
 * Основной цикл
 */
void loop() {
  
  // Заппрос на смену состояния
  // checkStateByButton(); // Опрос кнопки
  checkStateByUart(); // Опрос второй платы по UART

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
