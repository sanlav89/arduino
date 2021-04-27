void setup() {
  // Конфигурация 3го пина как выхода
  pinMode(3, OUTPUT);
}

// Глобальные переменные
int i = 0;
int direction = +1; // +1 - увеличение яркости, -1 - уменьшение

void loop() {
  i += direction;
  if (i == 255 || i == 0) {
    direction *= -1;  // Смена направления: увеличение или уменьшение яркости
  }
  analogWrite(3, i);  // 3 - номер пина, i - код ШИМ (от 0 до 255)
  delay(5);           // Задержка
}