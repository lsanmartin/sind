const int pwmPin = 9;  // Elige un pin PWM compatible, por ejemplo, 9

int minIntensity = 0; // Intensidad mínima del PWM
int maxIntensity = 55; // Intensidad máxima del PWM

void setup() {
  pinMode(pwmPin, OUTPUT); // Configura el pin PWM como salida
}

void loop() {
  // Incrementa la intensidad gradualmente
  for (int intensity = minIntensity; intensity <= maxIntensity; intensity++) {
    analogWrite(pwmPin, intensity);
    delay(15); // Ajusta este valor si es necesario
  }

  // Disminuye la intensidad gradualmente
  for (int intensity = maxIntensity; intensity >= minIntensity; intensity--) {
    analogWrite(pwmPin, intensity);
    delay(15); // Ajusta este valor si es necesario
  }
}
