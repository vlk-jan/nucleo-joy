import sys
import pickle
import atexit
import argparse
import threading

import dearpygui.dearpygui as dpg

from vis.robot_vis import RobotVisualizer
from vis.controller_vis import ControllerVisualizer
from vis.utils import listen_port, create_window, visualize_demo

C = ControllerVisualizer()
R = RobotVisualizer()

SAVE = False
LOAD = False

OBJECTS_DIR = 'data/'


@atexit.register
def save_objects():
    if not SAVE:
        return
    try:
        # Example: Saving C and R to temporary files
        with open(f'{OBJECTS_DIR}/controller_vis.pkl', 'wb') as f:
            pickle.dump(C, f)

        with open(f'{OBJECTS_DIR}/robot_vis.pkl', 'wb') as f:
            pickle.dump(R, f)

        print("INFO: Objects saved successfully.")
    except Exception as e:
        print(f"ERROR: {e}")


def main():
    # Argparse arguments
    parser = argparse.ArgumentParser(description='Visualize the data from the controller and robot.')
    parser.add_argument('port', type=str,
                        help='The port number of the serial connection.', default='/dev/ttyACM0')
    parser.add_argument('baud', type=int,
                        help='The baud rate of the serial connection.', default=115200)
    parser.add_argument('--save', action='store_true', help='Save the global objects.')
    parser.add_argument('--load', action='store_true', help='Load the global objects.')
    parser.add_argument('--debug', action='store_true', help='Print debug messages.')
    parser.add_argument('--demo', action='store_true', help='Run the demo.')
    args = parser.parse_args()

    if args.demo:
        visualize_demo()
        sys.exit(0)

    global SAVE, LOAD, C, R
    if args.save and args.load:
        raise ValueError("Please provide either the save or the load argument, not both.")
    if args.save:
        SAVE = True
    if args.load:
        LOAD = True
        C = pickle.load(open(f'{OBJECTS_DIR}/controller_vis.pkl', 'rb'))
        R = pickle.load(open(f'{OBJECTS_DIR}/robot_vis.pkl', 'rb'))

    # Write the arguments and sleep for two seconds
    print(f"====================\n")
    print(f"SPD Visualizer\n")
    print(f"====================\n")
    print(f"Arguments:")
    print(f"\tPort: {args.port}")
    print(f"\tBaud rate: {args.baud}")
    print(f"\tDebug: {args.debug}")

    dpg.create_context()
    create_window(C, R)

    dpg.create_viewport(title='SPD Visualizer', width=1920, height=1080)

    dpg.setup_dearpygui()
    dpg.show_viewport()

    if not LOAD:
        thread = threading.Thread(target=listen_port, args=(args.port, args.baud, C, R, args.debug))
        thread.start()

    dpg.start_dearpygui()
    dpg.destroy_context()


if __name__ == '__main__':
    main()
