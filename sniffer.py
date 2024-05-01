import re
import sys
import time
import serial
import threading

import dearpygui.dearpygui as dpg

DEBUG = True

PORT = '/dev/ttyACM0'
BAUD_RATE = 115200

ROBOT_MODE = 1

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


class ControllerData:
    """
    ControllerData class manages data extraction and visualization for controller inputs.

    Attributes:
        lh_joy (dict): Dictionary containing data for the left horizontal joystick.
        lv_joy (dict): Dictionary containing data for the left vertical joystick.
        rh_joy (dict): Dictionary containing data for the right horizontal joystick.
        rv_joy (dict): Dictionary containing data for the right vertical joystick.
        l1_butt (dict): Dictionary containing data for the L1 button.
        l2_butt (dict): Dictionary containing data for the L2 button.
        l3_butt (dict): Dictionary containing data for the L3 button.
        r1_butt (dict): Dictionary containing data for the R1 button.
        r2_butt (dict): Dictionary containing data for the R2 button.
        r3_butt (dict): Dictionary containing data for the R3 button.
        start_time (float): The start time of data extraction.

    Methods:
        extract_data(msg_data: str) -> None:
            Extracts data from the provided message data and updates the class attributes accordingly.

        create_window() -> None:
            Creates a visualization window using the Dear PyGui library to display the extracted data.
    """

    def __init__(self):
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
                        'time': [0.0] * N_SAMPLES}
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
                dpg.fit_axis_data(f'{attr}_x')
                dpg.fit_axis_data(f'{attr}_y')
        if DEBUG:
            print(debug_dict)

    def create_window(self):
        """
        Creates a visualization window using the Dear PyGui library to display the extracted data.
        """
        with dpg.window(label='Tutorial', tag='win', width=800, height=600):
            with dpg.menu_bar():
                with dpg.menu(label="Tools"):
                    dpg.add_menu_item(label="Show About", callback=lambda: dpg.show_tool(dpg.mvTool_About))
                    dpg.add_menu_item(label="Show Metrics", callback=lambda: dpg.show_tool(dpg.mvTool_Metrics))
                    dpg.add_menu_item(label="Show Documentation", callback=lambda: dpg.show_tool(dpg.mvTool_Doc))
                    dpg.add_menu_item(label="Show Debug", callback=lambda: dpg.show_tool(dpg.mvTool_Debug))
                    dpg.add_menu_item(label="Show Style Editor", callback=lambda: dpg.show_tool(dpg.mvTool_Style))
                    dpg.add_menu_item(label="Show Font Manager", callback=lambda: dpg.show_tool(dpg.mvTool_Font))
                    dpg.add_menu_item(label="Show Item Registry",
                                      callback=lambda: dpg.show_tool(dpg.mvTool_ItemRegistry))

            for attr, value in self.__dict__.items():
                if isinstance(value, dict):
                    with dpg.tree_node(label=value['label']):
                        # create plot
                        with dpg.plot(label=value['label'], height=400, width=-1):
                            dpg.add_plot_legend()

                            dpg.add_plot_axis(dpg.mvXAxis, label=f'{attr}_x', tag=f'{attr}_x')
                            dpg.add_plot_axis(dpg.mvYAxis, label=f'{attr}_y', tag=f'{attr}_y')

                            if 'bit' in value:
                                dpg.set_axis_limits(f'{attr}_y', -1, 2)
                            else:
                                dpg.set_axis_limits(f'{attr}_y', -140, 140)

                            dpg.add_stair_series(x=list(value['time']), y=list(value['values']),
                                                 label=value['label'], parent=f'{attr}_y',
                                                 tag=attr)


def read_data(controller_data: ControllerData):
    """ Reads data from the serial port and extracts the data.
    The message format is as follows:

    11183s562:RssiValue=-33 dBm, SnrValue=12dB
    11183s562:Received message content:
    11183s562:Mode: 1
    11183s562:Data0: 0
    11183s562:Data1: 0
    11183s562:Data2: 0
    11183s562:Data3: 0
    11183s562:Data4: 63
    11183s562:Data5: 0

    The data we are interested in is the Data0, Data1, Data2, Data3, and Data4, but only if the mode is 1.

    The function is infinite loop that reads the data and waits for the mode to be 1. Then it reads the following
    5 lines and extracts the data from them.

    :param controller_data: The ControllerData object to store the data.
    """
    ser = serial.Serial(PORT, BAUD_RATE)
    while True:
        msg_line = ser.readline().decode().strip()
        if f'Mode: {ROBOT_MODE}' in msg_line:
            lines = []
            for _ in range(5):
                lines.append(ser.readline().decode().strip())
            msg_data = '\n'.join(lines)

            if DEBUG:
                print(msg_data)

            controller_data.extract_data(msg_data)


def main():
    PORT = sys.argv[1] if len(sys.argv) > 1 else '/dev/ttyACM0'

    c = ControllerData()

    dpg.create_context()
    c.create_window()

    dpg.create_viewport(title='SPD Visualizer', width=850, height=640)

    dpg.setup_dearpygui()
    dpg.show_viewport()

    thread = threading.Thread(target=read_data, args=(c,))
    thread.start()

    dpg.start_dearpygui()
    dpg.destroy_context()


if __name__ == "__main__":
    main()
