import time
# from machine import SoftI2C, Pin
from machine import I2S, Pin
# import socket
# import os


sck_pin = Pin(33)   # Bit clock
ws_pin = Pin(25)   # Word select / LR clock
sd_pin = Pin(32)  # Serial data input


#  Configuración de I2S
i2s = I2S(
    0,
    sck=sck_pin,
    ws=ws_pin,
    sd=sd_pin,
    mode=I2S.RX,
    bits=16,
    format=I2S.STEREO,
    rate=8000,
    ibuf=1024
)



# Tiempo de grabación en segundos
record_time = 1
audio_data = bytearray()

# Grabar audio
start_time = time.time()
while time.time() - start_time < record_time:
    buf = bytearray(1024)
    num_bytes_read = i2s.readinto(buf)
    audio_data += buf[:num_bytes_read]

# Guardar los datos de audio en un archivo
with open('audio.raw', 'wb') as f:
    f.write(audio_data)

# Desinicializar I2S
i2s.deinit()
print("Grabación completada y guardada en 'audio.raw'")

