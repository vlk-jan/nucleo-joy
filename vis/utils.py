import serial
from typing import Union

import dearpygui.demo as demo
import dearpygui.dearpygui as dpg

from vis.robot_vis import RobotVisualizer
from vis.controller_vis import ControllerVisualizer

SNIFFER_MODE = 0
CONTROLLER_MODE = 1
ROBOT_MODE = 2


def listen_port(port: str, baudrate: int,
                c_vis: ControllerVisualizer,
                r_vis: RobotVisualizer,
                debug: bool = False) -> None:
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

    The data we are interested in is the Data0, Data1, Data2, Data3, and
    Data4, but only if the mode is 1.

    The function is infinite loop that reads the data and waits for the mode to be 1.
    Then it reads the following 5 lines and extracts the data from them.

    :param port: The port number of the serial connection.
    :param baudrate: The baud rate of the serial connection.
    :param c_vis: The ControllerVisualizer object.
    :param r_vis: The RobotVisualizer object.
    :param out_file: The file to write the data to.
    :param debug: Print debug messages.
    """

    ser = serial.Serial(port, baudrate)
    while True:
        msg_line = ser.readline().decode().strip()
        if f'Mode: {CONTROLLER_MODE}' in msg_line:
            lines = []
            for _ in range(5):
                lines.append(ser.readline().decode().strip())
            msg_data = '\n'.join(lines)

            if debug:
                print(msg_data)

            c_vis.extract_data(msg_data)

        elif f'Mode: {ROBOT_MODE}' in msg_line:
            lines = []
            for _ in range(5):
                lines.append(ser.readline().decode().strip())
            msg_data = '\n'.join(lines)

            if debug:
                print(msg_data)

            r_vis.extract_data(msg_data)


def create_window(c_vis: ControllerVisualizer, r_vis: RobotVisualizer) -> None:
    """Creates a visualization window using the Dear PyGui library to display the extracted data.
    """
    with dpg.window(label='SPD Visualizer', tag='win', width=1920, height=1080):
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

        with dpg.tree_node(label='Left Joystick'):
            margin = 20
            with dpg.plot(label='Left Joystick', height=400, width=-1):
                dpg.add_plot_legend()
                dpg.add_plot_axis(dpg.mvXAxis, label='Time', tag='l_joy_x')

                with dpg.plot_axis(dpg.mvYAxis, label='Horizontal Value', tag='lh_joy_y'):
                    dpg.set_axis_limits('lh_joy_y', -427 - margin, 127 + margin)
                    dpg.add_stair_series(x=list(c_vis.lh_joy['time']), y=list(c_vis.lh_joy['values']),
                                         label='Horizontal Value', tag='lh_joy')

                with dpg.plot_axis(dpg.mvYAxis, label='Vertical Value', tag='lv_joy_y'):
                    dpg.set_axis_limits('lv_joy_y', -127 - margin, 427 + margin)
                    dpg.add_stair_series(x=list(c_vis.lv_joy['time']), y=list(c_vis.lv_joy['values']),
                                         label='Vertical Value', tag='lv_joy')

        with dpg.tree_node(label='Right Joystick'):
            margin = 20
            with dpg.plot(label='Right Joystick', height=400, width=-1):
                dpg.add_plot_legend()
                dpg.add_plot_axis(dpg.mvXAxis, label='Time', tag='r_joy_x')

                with dpg.plot_axis(dpg.mvYAxis, label='Horizontal Value', tag='rh_joy_y'):
                    dpg.set_axis_limits('rh_joy_y', -427 - margin, 127 + margin)
                    dpg.add_stair_series(x=list(c_vis.rh_joy['time']), y=list(c_vis.rh_joy['values']),
                                         label='Horizontal Value', tag='rh_joy')

                with dpg.plot_axis(dpg.mvYAxis, label='Vertical Value', tag='rv_joy_y'):
                    dpg.set_axis_limits('rv_joy_y', -127 - margin, 427 + margin)
                    dpg.add_stair_series(x=list(c_vis.rv_joy['time']), y=list(c_vis.rv_joy['values']),
                                         label='Vertical Value', tag='rv_joy')

        with dpg.tree_node(label='Left Buttons'):
            margin = 0.15
            with dpg.plot(label='Left Buttons', height=400, width=-1):
                dpg.add_plot_legend()
                dpg.add_plot_axis(dpg.mvXAxis, label='Time', tag='l_butt_x')

                with dpg.plot_axis(dpg.mvYAxis, label='L1 Button', tag='l1_butt_y'):
                    dpg.set_axis_limits('l1_butt_y', 0 - margin, 5 + margin)
                    dpg.add_stair_series(x=list(c_vis.l1_butt['time']), y=list(c_vis.l1_butt['values']),
                                         label='L1 Button', tag='l1_butt')

                with dpg.plot_axis(dpg.mvYAxis, label='L2 Button', tag='l2_butt_y'):
                    dpg.set_axis_limits('l2_butt_y', -2 - margin, 3 + margin)
                    dpg.add_stair_series(x=list(c_vis.l2_butt['time']), y=list(c_vis.l2_butt['values']),
                                         label='L2 Button', tag='l2_butt')

                with dpg.plot_axis(dpg.mvYAxis, label='L3 Button', tag='l3_butt_y'):
                    dpg.set_axis_limits('l3_butt_y', -4 - margin, 1 + margin)
                    dpg.add_stair_series(x=list(c_vis.l3_butt['time']), y=list(c_vis.l3_butt['values']),
                                         label='L3 Button', tag='l3_butt')

        with dpg.tree_node(label='Right Buttons'):
            margin = 0.15
            with dpg.plot(label='Right Buttons', height=400, width=-1):
                dpg.add_plot_legend()
                dpg.add_plot_axis(dpg.mvXAxis, label='Time', tag='r_butt_x')

                with dpg.plot_axis(dpg.mvYAxis, label='R1 Button', tag='r1_butt_y'):
                    dpg.set_axis_limits('r1_butt_y', 0 - margin, 5 + margin)
                    dpg.add_stair_series(x=list(c_vis.r1_butt['time']), y=list(c_vis.r1_butt['values']),
                                         label='R1 Button', tag='r1_butt')

                with dpg.plot_axis(dpg.mvYAxis, label='R2 Button', tag='r2_butt_y'):
                    dpg.set_axis_limits('r2_butt_y', -2 - margin, 3 + margin)
                    dpg.add_stair_series(x=list(c_vis.r2_butt['time']), y=list(c_vis.r2_butt['values']),
                                         label='R2 Button', tag='r2_butt')

                with dpg.plot_axis(dpg.mvYAxis, label='R3 Button', tag='r3_butt_y'):
                    dpg.set_axis_limits('r3_butt_y', -4 - margin, 1 + margin)
                    dpg.add_stair_series(x=list(c_vis.r3_butt['time']), y=list(c_vis.r3_butt['values']),
                                         label='R3 Button', tag='r3_butt')

        for attr, value in r_vis.__dict__.items():
            if isinstance(value, dict):
                with dpg.tree_node(label=value['label']):
                    with dpg.plot(label=value['label'], height=400, width=-1):
                        dpg.add_plot_legend()

                        dpg.add_plot_axis(dpg.mvXAxis, label=f'{attr}_x', tag=f'{attr}_x')
                        dpg.add_plot_axis(dpg.mvYAxis, label=f'{attr}_y', tag=f'{attr}_y')

                        if 'battery' in attr:
                            dpg.set_axis_limits(f'{attr}_y', -10, 110)
                        elif 'temp' in attr:
                            dpg.set_axis_limits(f'{attr}_y', -20, 140)
                        else:
                            raise ValueError(f"Unknown attribute: {attr}")

                        dpg.add_stair_series(x=list(value['time']), y=list(value['values']),
                                             label=value['label'], parent=f'{attr}_y',
                                             tag=attr)


def visualize_demo():
    dpg.create_context()
    dpg.create_viewport(title='Custom Title', width=600, height=600)

    demo.show_demo()

    dpg.setup_dearpygui()
    dpg.show_viewport()
    dpg.start_dearpygui()
    dpg.destroy_context()
