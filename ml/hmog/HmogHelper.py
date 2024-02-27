import os
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

dataset_path = 'hmog_public_dataset/'

activity_header = ['id', 'subject_id', 'session', 'start_time', 'end_time',
                   'rel_start_time', 'rel_end_time', 'gesture_scenario', 'task_id', 'content_id']

accelerometer_header = ['sys_time', 'event_time', 'activity_id', 'x', 'y', 'z', 'orientation']

gyroscope_header = accelerometer_header.copy()
magnetometer_header = accelerometer_header.copy()

touch_event_header = ['sys_time', 'event_time', 'activity_id', 'pointer_count', 'pointer_id',
                      'action_id', 'x', 'y', 'pressure', 'contact_size', 'orientation']

# activity_id and press_type should be swapped, there is a typo in dataset description
# it also seems that press_type should be reversed (1: finger down, 0: finger up)
key_press_event_header = ['sys_time', 'press_time', 'activity_id', 'press_type', 'key_id', 'orientation']

one_finger_touch_event_header = ['sys_time', 'press_time', 'activity_id', 'tap_id', 'tap_type', 'action_type',
                                 'x', 'y', 'pressure', 'contact_size', 'orientation']

pinch_event_header = ['sys_time', 'press_time', 'activity_id', 'event_type', 'pinch_id', 'time_delta',
                      'x_focus', 'y_focus', 'span', 'span_x', 'span_y', 'scale_factor', 'orientation']

scroll_event_header = ['sys_time', 'begin_time', 'current_time', 'activity_id', 'scroll_id', 'start_action_type',
                       'start_x', 'start_y', 'start_pressure', 'start_size', 'current_action_type',
                       'current_x', 'current_y', 'current_pressure', 'current_size', 'distance_x', 'distance_y',
                       'orientation']

stroke_event_header = ['sys_time', 'begin_time', 'end_time', 'activity_id', 'start_action_type', 'start_x', 'start_y',
                       'start_pressure', 'start_size', 'end_action_type', 'end_x', 'end_y', 'end_pressure', 'end_size',
                       'speed_x', 'speed_y', 'orientation']

WW_SESSION_TYPES = [4, 10, 16, 22]
WS_SESSION_TYPES = [3, 9, 15, 21]

SENSOR_LIST = ['accelerometer', 'gyroscope']
DIMS_LIST = ['x', 'y', 'z', 'M']
HMOG_FEATURES_LEN = len(SENSOR_LIST) * len(DIMS_LIST) * 8


def check_session_typing(path_to_session: str):
    # We are interested in sessions 3, 9, 15, 21 for w+s
    # and 4, 10, 16, 22 for w+w
    typing_task_ids = [3, 9, 15, 21, 4, 10, 16, 22]

    return check_session_type(path_to_session, typing_task_ids)

def check_session_type(path_to_session: str, type_ids: list):
    activity_df = pd.read_csv(os.path.join(path_to_session, 'Activity.csv'), names=activity_header)

    if activity_df['task_id'][0] in type_ids:
        return True
    return False

def filter_user_sessions_by_type(path_to_sessions: str, type_ids: list):
    session_folders = os.listdir(path_to_sessions)
    session_folders = [f for f in session_folders if not os.path.isfile(os.path.join(path_to_sessions, f))]

    typing_session_folders = []
    for session_folder in session_folders:
        if check_session_type(os.path.join(path_to_sessions, session_folder), type_ids):
            typing_session_folders.append(session_folder)

    return typing_session_folders

def list_folder(path_to_dataset: str):
    folders = os.listdir(path_to_dataset)
    folders = [f for f in folders if not os.path.isfile(os.path.join(path_to_dataset, f))]

    return folders

def read_person_session(path_to_session: str):
    activity_csv = pd.read_csv(os.path.join(path_to_session, 'Activity.csv'), names=activity_header)
    accelerometer_csv = pd.read_csv(os.path.join(path_to_session, 'Accelerometer.csv'), names=accelerometer_header)
    gyroscope_csv = pd.read_csv(os.path.join(path_to_session, 'Gyroscope.csv'), names=gyroscope_header)
    key_press_event_csv = pd.read_csv(os.path.join(path_to_session, 'KeyPressEvent.csv'), names=key_press_event_header)
    # magnetometer_csv = pd.read_csv(os.path.join(path_to_session, 'Magnetometer.csv'), names=magnetometer_header)
    # one_finger_touch_csv = pd.read_csv(os.path.join(path_to_session, 'OneFingerTouchEvent.csv'), names=one_finger_touch_event_header)
    # pinch_event_csv = pd.read_csv(os.path.join(path_to_session, 'PinchEvent.csv'), names=pinch_event_header)
    # scroll_event_csv = pd.read_csv(os.path.join(path_to_session, 'ScrollEvent.csv'), names=scroll_event_header)
    # stroke_event_csv = pd.read_csv(os.path.join(path_to_session, 'StrokeEvent.csv'), names=stroke_event_header)
    # touch_event_csv = pd.read_csv(os.path.join(path_to_session, 'TouchEvent.csv'), names=touch_event_header)

    session_map = {"activity": activity_csv,
                   "accelerometer": accelerometer_csv,
                   "gyroscope": gyroscope_csv,
                   "key_press_event": key_press_event_csv,
                #    "magnetometer": magnetometer_csv,
                #    "one_finger_touch": one_finger_touch_csv,
                #    "pinch_event": pinch_event_csv,
                #    "scroll_event": scroll_event_csv,
                #    "stroke_event": stroke_event_csv,
                #    "touch_event": touch_event_csv
                   }
    
    return session_map

def add_M(sensor_data):
    sensor_data['M'] = (sensor_data['x'] ** 2 + sensor_data['y'] ** 2 + sensor_data['z'] ** 2) ** 0.5

def preprocess_session_data(session_map: dict):
    # Let's add M = sqrt(x^2 + y^2 + z^2) column to sensors
    add_M(session_map['accelerometer'])
    add_M(session_map['gyroscope'])

def compute_t_min(sensor_data, value_of: str, t_end_timestamp: int, avg100msBefore: np.float64):
    TIME_AFTER_END_OFFSET = 200  # ms

    sensor_data = sensor_data[(sensor_data['sys_time'] >= t_end_timestamp) & (sensor_data['sys_time'] <= t_end_timestamp + TIME_AFTER_END_OFFSET)]

    n = len(sensor_data.index)
    avgDiffs = np.empty(n)
    sums = np.zeros(n)

    # Indices are not updated after taking a slice of a dataframe!
    sums[n - 1] = abs(sensor_data.iloc[n - 1][value_of] - avg100msBefore)
    for index, entry in sensor_data.iloc[-2::-1].iterrows():
        index = index - sensor_data.index[0]  # TODO: optimize?

        sums[index] = abs(entry[value_of] - avg100msBefore) + sums[index + 1]
    
    min_index = 0
    min_val = 10000000
    for i in range(n):
        avgDiffs[i] = sums[i] / (n - i)

        if avgDiffs[i] < min_val:
            min_val = avgDiffs[i]
            min_index = i
            # print(f'min_index, min_val: {min_index}, {min_val}')

    min_index = np.argmin(avgDiffs)
    return sensor_data.iloc[min_index]['sys_time']

def extract_hmog_for_keypress(sensor_data, key_down_timestamp: int, key_up_timestamp: int, value_of: str):
    TIME_BEFORE_BOUNDARY = 100  # ms
    TIME_AFTER_BOUNDARY = 100   # ms
    TIME_AFTER_FOR_T_MIN = 200  # ms

    t_max = -1
    max_during = -10000000000

    vals_before = []
    vals_during = []
    vals_after = []

    sensor_data = sensor_data[(sensor_data['sys_time'] >= (key_down_timestamp - TIME_BEFORE_BOUNDARY)) & (sensor_data['sys_time'] <= (key_up_timestamp + TIME_AFTER_FOR_T_MIN))]

    for _, entry in sensor_data.iterrows():
        if entry['sys_time'] >= (key_down_timestamp - TIME_BEFORE_BOUNDARY) and entry['sys_time'] < key_down_timestamp:
            vals_before.append(entry[value_of])
        elif entry['sys_time'] >= key_down_timestamp and entry['sys_time'] <= key_up_timestamp:
            vals_during.append(entry[value_of])
            if entry[value_of] > max_during:
                max_during = entry[value_of]
                t_max = entry['sys_time']
        elif entry['sys_time'] > key_up_timestamp and entry['sys_time'] <= (key_up_timestamp + TIME_AFTER_BOUNDARY):
            vals_after.append(entry[value_of])
    
    # vals_* might be empty, which will result in division by zero, and, consequently, incorrect hmog features
    # We could eg. try appending the last value of vals_before to vals_during in case len(vals_during) == 0,
    # however, for now I suppose it's better to totally reject such keypresses

    if len(vals_before) == 0 or len(vals_during) == 0 or len(vals_after) == 0:
        return (False, []) 

    # HMOG features
    #
    # "Grasp Resistance"
    # 1. Mean during taps
    mean_during = np.mean(vals_during, dtype=np.float64)

    # 2. Standard deviation during taps
    # std_deviation_during = ( sum([(x - mean_during) ** 2 for x in vals_during]) / len(vals_during) ) ** 0.5
    std_deviation_during = np.std(vals_during, dtype=np.float64)

    # 3. avg100msAfter - avg100msBefore

    # avg100msAfter = sum(vals_after) / len(vals_after)
    avg100msAfter = np.mean(vals_after, dtype=np.float64)

    # avg100msBefore = sum(vals_before) / len(vals_before)
    avg100msBefore = np.mean(vals_before, dtype=np.float64)

    diff_readings = avg100msAfter - avg100msBefore

    # 4. avgTap - avg100msBefore
    net_change = mean_during - avg100msBefore
    # 5. max change during tap
    max_change_during = max_during - avg100msBefore

    #
    # "Grasp Stability"
    # 1
    t_min = compute_t_min(sensor_data, value_of, key_up_timestamp, avg100msBefore)
    time_to_stabilize = t_min - key_up_timestamp

    # 2
    t_after_center = key_up_timestamp + 50     # ms
    t_before_center = key_down_timestamp - 50  # ms
    # TODO: ok?
    if avg100msAfter - avg100msBefore != 0:
        d_duration = (t_after_center - t_before_center) / (avg100msAfter - avg100msBefore)
    else:
        d_duration = 0

    # 3
    # TODO: ok?
    if avg100msAfter - max_during != 0:
        d_max_to_avg = (t_after_center - t_max) / (avg100msAfter - max_during)
    else:
        d_max_to_avg = 0

    return (True, [mean_during, std_deviation_during, diff_readings, net_change, max_change_during,
            time_to_stabilize, d_duration, d_max_to_avg])

# This can be optimised, for now this is coded to present everything as simple as possible
# dims_list: x, y, z, M
def extract_hmog_features(session_map: dict, sensor_list: list, dims_list: list):
    # "Grasp Resistance" features

    PRESS_TYPE_DOWN = 1
    PRESS_TYPE_UP = 0

    hmog_vectors = []
    key_down_timestamps = []

    for key_press_event_index in range(len(session_map['key_press_event']) - 1):
        event = session_map['key_press_event'].iloc[key_press_event_index]
        event_down_time = event['sys_time']

        if event['press_type'] == PRESS_TYPE_DOWN:
            next_event = session_map['key_press_event'].iloc[key_press_event_index + 1]

            # Let's focus on cases when user lifts the finger before pressing another key
            if next_event['press_type'] != PRESS_TYPE_UP:
                continue
            
            event_up_time = next_event['sys_time']

            curr_key_press_hmog_vectors = []

            key_press_hmog_ext_success = True
            for sensor in sensor_list:
                for dim in dims_list:
                    extraction_result, tap_hmog_vector = extract_hmog_for_keypress(session_map[sensor], event_down_time,
                                                                                     event_up_time, dim)
                    if extraction_result:
                        curr_key_press_hmog_vectors.append(tap_hmog_vector)
                    else:
                        key_press_hmog_ext_success = False
                        break
                
                if not key_press_hmog_ext_success:
                    break
            
            if key_press_hmog_ext_success:
                key_down_timestamps.append([event_down_time])
                hmog_vectors.append(curr_key_press_hmog_vectors)
    if len(key_down_timestamps) > 0:
        hmog_vectors_numpy = np.array(hmog_vectors).reshape(-1, HMOG_FEATURES_LEN)
        hmog_vectors_numpy = np.concatenate((np.array(key_down_timestamps), hmog_vectors_numpy), axis=1)
        return (True, hmog_vectors_numpy)
    return (False, [])

def calc_std_and_scale(user_session_matrix):
    # matrix: (n_samples, n_features)

    # (1, n_features)
    std_deviations_features = np.std(user_session_matrix, axis=0, dtype=np.float64)

    return std_deviations_features, scale_by(user_session_matrix, std_deviations_features)

def scale_by(user_session_matrix, std_deviation_vector):
    assert std_deviation_vector.shape[0] == user_session_matrix.shape[1]

    result = np.zeros_like(user_session_matrix, dtype=np.float64)

    result[:, 0] = user_session_matrix[:, 0]
    # column 0 is a timestamp, we don't want to scale it.
    for i in range(1, user_session_matrix.shape[1]):
        result[:, i] = user_session_matrix[:, i] / std_deviation_vector[i]

    return result

def test_hmog(session_hmog_vector, correct_key: int, svm):
    predictions = svm.predict(session_hmog_vector)

    vals, counts = np.unique(predictions, return_counts=True)
    counts = dict(zip(vals, counts))

    if correct_key in counts:
        accuracy = counts[correct_key] / len(predictions)
    else:
        accuracy = 0
    return accuracy

'''
hmog_vector: must be with a timestamp
'''

def test_hmog_windowed(hmog_vectors, t_window: int, model):
    predictions_windowed = []

    curr_window_begin_t = hmog_vectors[0][0]
    curr_window_begin_i = 0

    hmog_entries_count = hmog_vectors.shape[0]
    for i in range(1, hmog_entries_count):
        curr_t = hmog_vectors[i][0]
        if curr_t - curr_window_begin_t > t_window:
            if i != curr_window_begin_i:
                hmog_slice = hmog_vectors[curr_window_begin_i:i, 1:]
                auth_vector = np.mean(hmog_slice, axis=0).reshape(1, -1)

                predictions_windowed.append(model.decision_function(auth_vector))
            curr_window_begin_i = i
            curr_window_begin_t = curr_t
    
    return predictions_windowed
