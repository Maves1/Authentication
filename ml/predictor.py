from enum import Enum
from sklearn.svm import OneClassSVM
from sklearn.preprocessing import StandardScaler

import socket
import struct
import pickle
import threading
import time
import os
from hmog import MyDatasetHelper

import numpy as np
import pandas as pd

HMOG_HEADER = list(map(str, range(1, 65)))
HEADER = ['timestamp'] + HMOG_HEADER
THRESHOLD_SVM = 0.326
scaler = StandardScaler()

IP = '192.168.0.100'
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
        self.decision_boundary = THRESHOLD_SVM

        # Let's use SVM for now
        # self.model = self.load_saved_model('./pretrained_models/my_model')
        self.model = self.train_model()
        print('predictor ready')

    '''
    window_len: seconds
    '''
    def set_auth_window(self, window_len):
        self.auth_window_len = window_len
        self.sensor_reading_count = self.auth_window_len * self.hz + self.hz  # let's keep one more second of readings just in case

    def load_saved_model(path: str):
        return pickle.loads(path)
    
    def train_model(self):
        print("Training the model...")
        session_map = MyDatasetHelper.read_person_session('my_dataset/001/s/01')
        session_map_2 = MyDatasetHelper.read_person_session('my_dataset/001/s/02')
        MyDatasetHelper.preprocess_session_data(session_map)
        MyDatasetHelper.preprocess_session_data(session_map_2)
        success, train_hmog_vector_1 = MyDatasetHelper.extract_hmog_features(session_map, MyDatasetHelper.SENSOR_LIST, MyDatasetHelper.DIMS_LIST)
        success_2, train_hmog_vector_2 = MyDatasetHelper.extract_hmog_features(session_map_2, MyDatasetHelper.SENSOR_LIST, MyDatasetHelper.DIMS_LIST)

        if success and success_2:
            train_hmog_vector = np.vstack((train_hmog_vector_1, train_hmog_vector_2))
            session_df = pd.DataFrame(train_hmog_vector,
                                      columns=HEADER)

            session_df[HMOG_HEADER] = scaler.fit_transform(session_df[HMOG_HEADER])

            model = OneClassSVM(kernel='rbf', gamma='auto', nu=0.01).fit(session_df[HMOG_HEADER].to_numpy())

            print("Model trained successfully!")
            return model
        else:
            print('could not train model')

    def predict(self):
        df_accel    = pd.DataFrame(self.accelerometer_list.copy(), columns=MyDatasetHelper.sensor_header + ['M'])
        df_gyro     = pd.DataFrame(self.gyroscope_list.copy(), columns=MyDatasetHelper.sensor_header + ['M'])
        df_keyboard = pd.DataFrame(self.keyboard_list.copy(), columns=MyDatasetHelper.keypress_header)

        session_map = {"accelerometer": df_accel,
                       "gyroscope": df_gyro,
                       "key_press_event": df_keyboard
                      }

        success, test_hmog_vector = MyDatasetHelper.extract_hmog_features(session_map, MyDatasetHelper.SENSOR_LIST, MyDatasetHelper.DIMS_LIST)

        if success:
            test_hmog_vector_full = pd.DataFrame(test_hmog_vector, columns=['timestamp'] + HMOG_HEADER)

            last_timestamp_ms = test_hmog_vector_full['timestamp'].iloc[-1]
            test_hmog_vector = test_hmog_vector_full[test_hmog_vector_full['timestamp'] >= (last_timestamp_ms - self.auth_window_len * 1000)]
            test_hmog_vector_np = scaler.transform(test_hmog_vector[HMOG_HEADER])
            print("Number of keypresses used: ", test_hmog_vector_np.shape)
            test_hmog_vector_mean = np.mean(test_hmog_vector_np, axis=0).reshape(1, -1)

            result = self.model.decision_function(test_hmog_vector_mean)

            print(f'{"Owner!" if result >= self.decision_boundary else "Someone else!"} ({result})')
        else:
            print("Something is wrong")

    def start_predicting(self, t_interval_seconds: int):
        while True:
            time.sleep(t_interval_seconds)
            self.predict()

class DataReceiver:
    def __init__(self, predictor) -> None:
        self.predictor = predictor

    def start_receiving(self):
        while True:
            data, addr = sock.recvfrom(1024)
            
            data_type = struct.unpack('!i', data[:4])[0]

            if data_type == DataType.AccelerometerReading.value:
                timestamp = struct.unpack('!Q', data[4:12])[0]
                x = struct.unpack('!d', data[12:20])[0]
                y = struct.unpack('!d', data[20:28])[0]
                z = struct.unpack('!d', data[28:36])[0]
                m = (x ** 2 + y ** 2 + z ** 2) ** 0.5

                self.predictor.accelerometer_list.append((timestamp, x, y, z, m))

                # round_print(timestamp, x, y, z)
            elif data_type == DataType.GyroscopeReading.value:
                timestamp = struct.unpack('!Q', data[4:12])[0]
                x = struct.unpack('!d', data[12:20])[0]
                y = struct.unpack('!d', data[20:28])[0]
                z = struct.unpack('!d', data[28:36])[0]
                m = (x ** 2 + y ** 2 + z ** 2) ** 0.5

                self.predictor.gyroscope_list.append((timestamp, x, y, z, m))

                # round_print(timestamp, x, y, z, m)
            elif data_type == DataType.KeyboardReading.value:
                press_time = struct.unpack('!q', data[4:12])[0]
                release_time = struct.unpack('!q', data[12:20])[0]

                self.predictor.keyboard_list.append((press_time, release_time))
                
                # round_print(press_time, release_time)

def round_print(*numbers):
    print(*[round(number, 10) for number in numbers], end='\t')
    print()

predictor = Predictor()
receiver = DataReceiver(predictor)
thread_receiver = threading.Thread(target=receiver.start_receiving, args=())
thread_receiver.start()

thread_predictor = threading.Thread(target=predictor.start_predicting, args=(5,))
thread_predictor.start()

