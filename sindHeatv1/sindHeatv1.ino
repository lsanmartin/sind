#include <OneWire.h>
#include <DallasTemperature.h>


// Definiciones de microcorrientes

const int pwmPin = 9;  // Elige un pin PWM compatible, por ejemplo, 9

int minIntensity = 0; // Intensidad mínima del PWM
int maxIntensity = 55; // Intensidad máxima del PWM




// Pin del sensor de temperatura
const int pinSensorTemp = 2; 

// Pin del MOSFET
const int pinMOSFET = 3; 

// Temperatura objetivo en grados Celsius
const float temperaturaObjetivo = 35.0; 

// Inicialización del sensor de temperatura
OneWire oneWire(pinSensorTemp);
DallasTemperature sensors(&oneWire);




void setup() {
  // Iniciar comunicación serial
  Serial.begin(9600);
  // Iniciar el sensor de temperatura
  sensors.begin();
  // Configurar el pin del MOSFET como salida
  pinMode(pinMOSFET, OUTPUT);


  //mcorr
   pinMode(pwmPin, OUTPUT); // Configura el pin PWM como salida
}

void loop() {
  // Leer la temperatura actual
  sensors.requestTemperatures(); 
  float temperaturaActual = sensors.getTempCByIndex(0);

  // Mostrar la temperatura actual
  Serial.print("Temperatura Actual: ");
  Serial.print(temperaturaActual);
  Serial.println(" C");

  // Controlar el alambre de nicromo
  if (temperaturaActual < temperaturaObjetivo) {
    // Encender el alambre si la temperatura está por debajo del objetivo
    digitalWrite(pinMOSFET, HIGH);
  } else {
    // Apagar el alambre si la temperatura alcanza o supera el objetivo
    digitalWrite(pinMOSFET, LOW);
  }

  // Esperar un poco antes de la próxima medición
  delay(1000);


  //mc
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
