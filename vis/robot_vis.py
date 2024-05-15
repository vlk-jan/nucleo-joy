import re
import time

import dearpygui.dearpygui as dpg

BATTERY_BYTE = 0
TEMP_BYTE = 1

N_SAMPLES = 100


class RobotVisualizer:
    def __init__(self, debug: bool = False):
        self.battery = {'pattern': rf'Data{BATTERY_BYTE}: (-?\d+)',
                        'label': 'Battery',
                        'values': [0.0] * N_SAMPLES,
                        'time': [0.0] * N_SAMPLES}
        self.temp = {'pattern': rf'Data{TEMP_BYTE}: (-?\d+)',
                     'label': 'Temperature',
                     'values': [0.0] * N_SAMPLES,
                     'time': [0.0] * N_SAMPLES}

        self.start_time = time.time()
        self.debug = debug

    def extract_data(self, msg_data: str):
        """
        Extracts data from the provided message data and updates the class attributes accordingly.

        Args:
            msg_data (str): Message data containing controller input information.
        """
        debug_dict = {'battery': None, 'temp': None}
        for attr, value in self.__dict__.items():
            if isinstance(value, dict):
                # print(f"DEBUG: {attr}")
                data = re.findall(value['pattern'], msg_data)

                # If the data is not found, skip
                if not data:
                    continue

                value['values'].append(int(data[0]))
                debug_dict[attr] = int(data[0])

                # Get current timestamp
                value['time'].append(time.time() - self.start_time)

                # Set the series x and y to the last n samples
                dpg.set_value(attr, [value['time'][-N_SAMPLES:],
                                     value['values'][-N_SAMPLES:]])

                dpg.fit_axis_data(f'{attr}_x')
                dpg.fit_axis_data(f'{attr}_y')
        if self.debug:
            print(debug_dict)
