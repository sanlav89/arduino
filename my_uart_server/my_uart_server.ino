/*
 * Константы
 */
const int BUTTON_PIN = 2;   // Номер пина, к которому подключена кнопка
const int STATE_BLINK = 0;  // Состояние "Моргание"
const int STATE_PWM = 1;    // Состояние "ШИМ"
const int BUTTON_CLICK_DELAY = 10; // Задержка после детектирования нажатия кнопки, миллисекунды

/*
 * Глобальные переменные
 */
int state = STATE_BLINK;    // Текущее состояние системы
int buttonState = LOW;      // Состояние кнопки


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT);        // Конфигурация 2го пина как входа (кнопка)
  pinMode(13, OUTPUT);
}

/*
 * Смена текущего состояния системы
 */
void switchState()
{
  if (state == STATE_BLINK) {
    state = STATE_PWM;
    digitalWrite(13, LOW);  // Гасим
  } else {
    state = STATE_BLINK;
    digitalWrite(13, HIGH);  // Зажигаем
  }
}

/*
 * Функция обработки дребезга контактов кнопки
 * "обработанное" значение кнопки - значение, после стабилизации "дребезга",
 * то есть значение, которое возвращает эта функция
 * 
 * входной параметр: последнее "обработанное" состяние кнопки
 * выходной параметр: текущее "обработанное" состояние кнопки
 * 
 * Если прочитанное состояние пина не равно последнему обработанному состоянию кнопки, 
 * то начался "дребезг". Ждем несколько миллисекунд, после этого состояние кнопки должно
 * стабилизироваться. Читаем состояние пина снова, возвращаем прочитанное значение. 
 */
int debounce(int last) {
  int current = digitalRead(BUTTON_PIN);
  if (last != current) {
    delay(BUTTON_CLICK_DELAY);
    current = digitalRead(BUTTON_PIN);
  }
  return current;
}


void loop() {

// ------------- Работа с кнопкой --------------------
  // Чтение состояния кнопки
  // buttonStateCurrent - значение кнопки, которое мы читаем сейчас
  // buttonState - значение кнопки, которое установлено ранее
  // Отлавиваем ситуацию, когда "обработанное" значение кнопки меняется с 0 на 1 (или с LOW на HIGH)
  // Только в этом случае меняем "состояние системы". 
  int buttonStateCurrent = debounce(buttonState);
  if (buttonState == LOW && buttonStateCurrent == HIGH) { // Переход между состояниями
    switchState();
  }
  buttonState = buttonStateCurrent;

// ------------- Работа с UART --------------------
  // Логика следующая: 
  // Пока в буфере уарта есть какие-то байты, читаем все, 
  // и если пришел байт запроса состояния от клиента (0x55), то готовимся ему ответить
  bool flag = false;  // Признак того, что принят байт запроса состояния по UART
  while(Serial.available()) {
    if (Serial.read() == 0x55) {
      flag = true;
      break;
    }
  }

  // Если Был принят байт запроса, то нужно ответить клиенту. Отвечаем
  if (flag) {
    // Serial.write(state == STATE_BLINK ? 0x55 : 0xAA); // это эквивалентно следующему: 
    int byteToWrite;
    if (state == STATE_BLINK)
      byteToWrite = 0x55;
    else
      byteToWrite = 0xAA;
    Serial.write(byteToWrite);
  }
}
