void setup() {
  // Конфигурация 13го пина как выхода
  pinMode(13, OUTPUT);
}

void loop() {
  digitalWrite(13, HIGH); // Зажигаем
  delay(500);
  digitalWrite(13, LOW);  // Гасим
  delay(500); 
}
