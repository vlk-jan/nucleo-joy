#!/usr/bin/env python3

import sys
import time
import serial
import psutil


BATTERY_SEND_INTERVAL = 10


def open_port_safe(port: str = None, baudrate: int = 115200) -> serial.Serial:
    if port is None:
        raise ValueError("Please provide the port number")

    try:
        serial_port = serial.Serial(f"{port}", baudrate=baudrate, timeout=BATTERY_SEND_INTERVAL)
    except serial.SerialException as e:
        print(e)
        sys.exit(1)

    return serial_port


def main(port: str = None, baudrate: int = 115200) -> None:
    # Open the serial port
    serial_port = open_port_safe(port, baudrate)

    last_send_time = time.time()
    # Main loop
    try:
        while serial_port.isOpen():
            print(serial_port.readline().decode('utf-8'), end="")
            if time.time() - last_send_time > BATTERY_SEND_INTERVAL:
                battery = psutil.sensors_battery()
                msg = f"bat{chr(battery.percent)}\0".encode('utf-8')
                serial_port.write(msg)
                last_send_time = time.time()
    except KeyboardInterrupt:  # Exit the program when Ctrl+C is pressed
        serial_port.close()
        print("Serial port is closed")
        sys.exit(0)
    except serial.SerialException as e:
        print(e)
        sys.exit(1)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python3 robot.py port baudrate")
        sys.exit(1)

    port = sys.argv[1]
    baudrate = int(sys.argv[2])

    main(port, baudrate)
