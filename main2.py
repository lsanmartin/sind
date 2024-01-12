import network
import time
from machine import SoftI2C, Pin
import socket
import os
import ads1x15


# Conexión a la red WiFi
wlan = network.WLAN(network.STA_IF)
wlan.active(True)
wlan.connect("VTR-1493572", "Xt5qdvkprjgz")  # Reemplaza con tus credenciales de WiFi

# Espera hasta que la conexión sea exitosa
while not wlan.isconnected():
    pass

print("Conexión a WiFi exitosa")
print(wlan.ifconfig())

# Inicializa la comunicación I2C
i2c = SoftI2C(scl=Pin(22), sda=Pin(21))

# Dirección I2C y ganancia para el ADS1115
addr = 72  # Ajusta la dirección si es diferente
gain = 1   # Ajusta la ganancia según sea necesario

# Inicializa el ADS1115
ads = ads1x15.ADS1115(i2c, addr, gain)

# Configura el archivo para grabar el audio
archivo = "grabacion.raw"

# Define la duración de la grabación y la frecuencia de muestreo
duracion_grabacion = 5  # en segundos
frecuencia_muestreo = 16000  # Hz, valor bajo para demostración

# Calcula el intervalo de muestreo
intervalo_muestreo = 1 / frecuencia_muestreo

# Grabar el audio
print("Comenzando la grabación")
with open(archivo, "wb") as f:
    inicio = time.ticks_ms()
    while time.ticks_diff(time.ticks_ms(), inicio) < duracion_grabacion * 1000:
        print(1)
        valor_adc = ads.read(1, gain)  # Lee del canal 0
        f.write(bytearray([valor_adc & 0xFF, (valor_adc >> 8) & 0xFF]))
        time.sleep(intervalo_muestreo)

print("Grabación completada")


# Configuración del servidor web
addr = socket.getaddrinfo('0.0.0.0', 80)[0][-1]

s = socket.socket()
s.bind(addr)
s.listen(1)

print('Escuchando en', addr)

def send_response(client, payload, status_code=200, content_type='text/html'):
    client.send('HTTP/1.1 {} OK\r\n'.format(status_code))
    client.send('Content-Type: {}\r\n'.format(content_type))
    client.send('Connection: close\r\n\r\n')
    client.sendall(payload)

while True:
    cl, addr = s.accept()
    print("Conexión desde", addr)
    request = cl.recv(1024)
    request = str(request)

    if "GET /download/" in request:
        filename = request.split("/download/")[1].split(" ")[0]
        try:
            with open(filename, "rb") as file:
                file_data = file.read()
                send_response(cl, file_data, content_type="application/octet-stream")
        except OSError:
            send_response(cl, "File not found", status_code=404)
    else:
        # Generar la lista de archivos
        response = "<html><body><h1>ESP32 File Server</h1><ul>"
        for file in os.listdir():
            response += '<li><a href="/download/{}">{}</a></li>'.format(file, file)
        response += "</ul></body></html>"
        send_response(cl, response)

    cl.close()
