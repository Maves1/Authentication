from enum import Enum

import socket
import struct

IP = '127.0.0.1'
PORT = 2025

sock = socket.socket(socket.AF_INET,
                     socket.SOCK_DGRAM)
sock.bind((IP, PORT))

class DataType(Enum):
    AccelerometerReading = 0
    GyroscopeReading = 1
    KeyboardReading = 2

class Predictor:
    def __init__(self) -> None:
        self.accelerometer_list = []
        self.gyroscope_list = []
        self.keyboard_list = []

        self.hz = 50
        self.set_auth_window(20)

    def set_auth_window(self, window_len):
        self.auth_window_len = window_len
        self.sensor_reading_count = self.auth_window_len * self.hz + self.hz  # let's keep one more second of readings just in case

while True:
    data, addr = sock.recvfrom(1024)
    
    data_type = struct.unpack('', data)

    if data_type == DataType.AccelerometerReading.value:
        print(DataType.AccelerometerReading.name)
    elif data_type == DataType.GyroscopeReading.value:
        print(DataType.GyroscopeReading.name)
    elif data_type == DataType.KeyboardReading.value:
        print(DataType.KeyboardReading.name)