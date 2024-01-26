#include <Arduino.h>

const int pwmPin = 4;       // Elige un pin PWM
const int frequency = 150; // Frecuencia del PWM en Hz
const int pwmChannel = 0;   // Canal PWM
const int resolution = 8;   // Resolución del PWM, 8 bits

int intensity = 55;          // Intensidad del PWM, de 27 a 55
// 80  aprox 1000microA
void setup() {
  // Configura el pin PWM
  ledcSetup(pwmChannel, frequency, resolution);
  ledcAttachPin(pwmPin, pwmChannel);
}

void loop() {
  // Incrementa la intensidad gradualmente
  for (intensity = 0; intensity <= 255; intensity++) {
    ledcWrite(pwmChannel, intensity);
    delay(5); // Ajusta este valor si es necesario
  }

  // Disminuye la intensidad gradualmente
  for (intensity = 255; intensity >= 0; intensity--) {
    ledcWrite(pwmChannel, intensity);
    delay(5); // Ajusta este valor si es necesario
  }
}
