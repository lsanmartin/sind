#include <Arduino.h>
#include <SPI.h>
#include <Arduino_GFX_Library.h>

#define PIN_DC   21
#define PIN_CS   22
#define PIN_SCK  18
#define PIN_MOSI 23
#define PIN_MISO -1
#define PIN_RST  19

#define BUTTON_UP_PIN 2
#define BUTTON_DOWN_PIN 14

Arduino_DataBus *bus = new Arduino_ESP32SPI(PIN_DC, PIN_CS, PIN_SCK, PIN_MOSI, PIN_MISO, VSPI);
Arduino_GC9A01 *tft = new Arduino_GC9A01(bus, PIN_RST, 0 /* rotation */, true /* IPS */);

int numero = 0; 
int pantallaWidth = 240;
int pantallaHeight = 240;

uint16_t colorFondo = tft->color565(48, 57, 65);   // Color de fondo #303941
uint16_t colorTexto = tft->color565(198, 203, 212); // Color de texto #C6CBD4

void setup() {
  tft->begin();
  tft->fillScreen(colorFondo);
  tft->setTextColor(colorTexto);
  tft->setTextSize(3);

  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);

  mostrarNumero();
}

void loop() {
  if (digitalRead(BUTTON_UP_PIN) == LOW && numero < 100) {
    numero++;
    mostrarNumero();
    delay(200);
  }

  if (digitalRead(BUTTON_DOWN_PIN) == LOW && numero > 0) {
    numero--;
    mostrarNumero();
    delay(200);
  }
}

void mostrarNumero() {
  tft->fillScreen(colorFondo);
  
  int textWidth = 6 * 6 * 3; // Aproximado: 6 píxeles por carácter, 6 caracteres ("100"), tamaño de texto 3
  int x = (pantallaWidth - textWidth) / 2; // Centrar horizontalmente
  int y = (pantallaHeight - 8 * 3) / 2;    // Centrar verticalmente, 8 píxeles por altura de caracter

  tft->setCursor(x, y);
  tft->print(numero);
}
