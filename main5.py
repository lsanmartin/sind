import time
# from machine import SoftI2C, Pin
from machine import I2S, Pin
import socket
import os
# import ads1x15

# Configuración de pines I2S
sck_pin = Pin(14)   # Bit clock
ws_pin = Pin(13)    # Word select / LR clock
sd_pin = Pin(12)  # Serial data input

# Configuración de I2S
i2s = I2S(
    0,              # Número del periférico I2S (0 o 1)
    sck=sck_pin,    # Pin para Serial Clock
    ws=ws_pin,      # Pin para Word Select
    sd=sd_pin,      # Pin para Serial Data
    mode=I2S.RX,
    bits=32,
    format=I2S.STEREO,  # Utiliza I2S.STEREO
    rate=10000,
    ibuf=2048       # Tamaño del buffer interno
)

# Tiempo de grabación en segundos
record_time = 3
audio_data = bytearray()

# Grabar audio
start_time = time.time()
while time.time() - start_time < record_time:
    buf = bytearray(2048)
    num_bytes_read = i2s.readinto(buf)
    audio_data += buf[:num_bytes_read]

# Guardar los datos de audio en un archivo
with open('audio.raw', 'wb') as f:
    f.write(audio_data)

# Desinicializar I2S
i2s.deinit()
print("Grabación completada y guardada en 'audio.raw'")


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
