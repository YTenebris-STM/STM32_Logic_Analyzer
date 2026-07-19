import serial
import threading
import time
import sys


def get_port():
    while True:
        try:
            com = input("write port as COM0:\n").strip()
            baud_input = input("write baudrate:\n").strip()
            baud = int(baud_input)
        except ValueError:
            print("error: must be an integer")
            print("try again\n")
            continue
        except KeyboardInterrupt:
            print("done")
            sys.exit(0)

        try:
            port = serial.Serial(com.upper(), baudrate=baud, timeout=None)
            print(f"port {com.upper()} was opened successfuly on {baud} baud.")
            return port
        except serial.SerialException as e:
            print(f"port {e} is not found")
            print("try again\n")
        except Exception as e:
            print(f"unexeptable error {e}")
            print("try again\n")
        except KeyboardInterrupt:
            print("\ndone")
            sys.exit(0)


port = get_port()
running = True

def stopper():
    input("press enter to stop...")
    global running
    running = False
    port.close()

thread = threading.Thread(target=stopper, daemon=True)
thread.start()

try:
    with open('data.log', 'a', encoding='utf-8') as f:
        while running:
            try:
                data = port.read(4)
                if data:
                    f.write(data.decode('utf-8') + '\n')
            except serial.SerialException:
                print("port is disabled")
                break
except KeyboardInterrupt:
    port.close()
finally:
    if port is not None and port.is_open:
        port.close()
    print("done")
