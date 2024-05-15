import re
import time

import dearpygui.dearpygui as dpg

LH_BYTE = 1
LV_BYTE = 0
RH_BYTE = 3
RV_BYTE = 2
BUTTONS_BYTE = 4

L1_BIT = 5
L2_BIT = 4
L3_BIT = 3
R1_BIT = 2
R2_BIT = 1
R3_BIT = 0

N_SAMPLES = 100


class ControllerVisualizer:
    def __init__(self, debug: bool = False):
        self.lh_joy = {'pattern': rf'Data{LH_BYTE}: (-?\d+)',
                       'label': 'Left Horizontal Joystick',
                       'values': [0.0] * N_SAMPLES,
                       'time': [0.0] * N_SAMPLES}
        self.lv_joy = {'pattern': rf'Data{LV_BYTE}: (-?\d+)',
                       'label': 'Left Vertical Joystick',
                       'values': [0.0] * N_SAMPLES,
                       'time': [0.0] * N_SAMPLES}
        self.rh_joy = {'pattern': rf'Data{RH_BYTE}: (-?\d+)',
                       'label': 'Right Horizontal Joystick',
                       'values': [0.0] * N_SAMPLES,
                       'time': [0.0] * N_SAMPLES}
        self.rv_joy = {'pattern': rf'Data{RV_BYTE}: (-?\d+)',
                       'label': 'Right Vertical Joystick',
                       'values': [0.0] * N_SAMPLES,
                       'time': [0.0] * N_SAMPLES}
        self.l1_butt = {'pattern': rf'Data{BUTTONS_BYTE}: (-?\d+)',
                        'label': 'L1 Button',
                        'bit': L1_BIT,
                        'values': [1.0] * N_SAMPLES,
                        'time': [0.0] * N_SAMPLES, }
        self.l2_butt = {'pattern': rf'Data{BUTTONS_BYTE}: (-?\d+)',
                        'label': 'L2 Button',
                        'bit': L2_BIT,
                        'values': [1.0] * N_SAMPLES,
                        'time': [0.0] * N_SAMPLES}
        self.l3_butt = {'pattern': rf'Data{BUTTONS_BYTE}: (-?\d+)',
                        'label': 'L3 Button',
                        'bit': L3_BIT,
                        'values': [1.0] * N_SAMPLES,
                        'time': [0.0] * N_SAMPLES}
        self.r1_butt = {'pattern': rf'Data{BUTTONS_BYTE}: (-?\d+)',
                        'label': 'R1 Button',
                        'bit': R1_BIT,
                        'values': [1.0] * N_SAMPLES,
                        'time': [0.0] * N_SAMPLES}
        self.r2_butt = {'pattern': rf'Data{BUTTONS_BYTE}: (-?\d+)',
                        'label': 'R2 Button',
                        'bit': R2_BIT,
                        'values': [1.0] * N_SAMPLES,
                        'time': [0.0] * N_SAMPLES}
        self.r3_butt = {'pattern': rf'Data{BUTTONS_BYTE}: (-?\d+)',
                        'label': 'R3 Button',
                        'bit': R3_BIT,
                        'values': [1.0] * N_SAMPLES,
                        'time': [0.0] * N_SAMPLES}

        self.start_time = time.time()
        self.debug = debug
        self.l_joy_set = 0
        self.r_joy_set = 0
        self.l_butt_set = 0
        self.r_butt_set = 0

    def extract_data(self, msg_data: str):
        """
        Extracts data from the provided message data and updates the class attributes accordingly.

        Args:
            msg_data (str): Message data containing controller input information.
        """
        debug_dict = {'lh_joy': None, 'lv_joy': None, 'rh_joy': None, 'rv_joy': None,
                      'l1_butt': None, 'l2_butt': None, 'l3_butt': None, 'r1_butt': None, 'r2_butt': None,
                      'r3_butt': None}
        for attr, value in self.__dict__.items():
            if isinstance(value, dict):
                data = re.findall(value['pattern'], msg_data)

                # If the data is not found, skip
                if not data:
                    continue

                # If it is button data, take only the one bit we are interested in based on the bit number
                if 'bit' in value:
                    v = int(data[0]) & (1 << value['bit'])
                    value['values'].append(True if v else False)
                    debug_dict[attr] = True if v else False
                else:
                    value['values'].append(int(data[0]))
                    debug_dict[attr] = int(data[0])

                # Get current timestamp
                value['time'].append(time.time() - self.start_time)

                # Set the series x and y to the last n samples
                dpg.set_value(attr, [value['time'][-N_SAMPLES:],
                                     value['values'][-N_SAMPLES:]])

                if attr in ['lv_joy', 'lh_joy']:
                    dpg.fit_axis_data(f'l_joy_x')
                    if self.l_joy_set < 6:
                        dpg.fit_axis_data(f'lh_joy_y')
                        dpg.fit_axis_data(f'lv_joy_y')
                        self.l_joy_set += 1
                    # dpg.fit_axis_data(f'lh_joy_y')
                    # dpg.fit_axis_data(f'rh_joy_y')
                elif attr in ['rh_joy', 'rv_joy']:
                    dpg.fit_axis_data(f'r_joy_x')
                    dpg.fit_axis_data(f'rh_joy_y')
                    dpg.fit_axis_data(f'rv_joy_y')
                elif attr in ['l1_butt', 'l2_butt', 'l3_butt']:
                    dpg.fit_axis_data(f'l_butt_x')
                    dpg.fit_axis_data(f'l1_butt_y')
                    dpg.fit_axis_data(f'l2_butt_y')
                    dpg.fit_axis_data(f'l3_butt_y')
                elif attr in ['r1_butt', 'r2_butt', 'r3_butt']:
                    dpg.fit_axis_data(f'r_butt_x')
                    dpg.fit_axis_data(f'r1_butt_y')
                    dpg.fit_axis_data(f'r2_butt_y')
                    dpg.fit_axis_data(f'r3_butt_y')
                else:
                    raise ValueError(f"Unknown attribute: {attr}")
        if self.debug:
            print(debug_dict)
