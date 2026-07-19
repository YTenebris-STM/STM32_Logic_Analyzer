# 8-Channel Logic Analyzer on STM32F103

**8-channel logic analyzer** based on the STM32F103C8T6 microcontroller with a 1 KHz sampling rate. The project is written in pure C using CMSIS (no HAL) for maximum performance and compactness. Data is compressed using RLE (Run-Length Encoding) and transmitted to a PC via UART, where it is visualized using Python.

## Features

- **8 digital channels** for signal analysis.
- **1 KHz sampling rate** (configurable).
- **Button** to start/stop capture on PA0.
- **RLE compression** — reduces transmitted data by up to 1000x for long idle periods.
- **Python visualization** — pyserial GUI for displaying all 8 channels.

## Technologies

- **MCU:** STM32F103C8T6 (Cortex-M3)
- **IDE:** Keil uVision / STM32CubeIDE / any GCC ARM toolchain
- **Language:** C (CMSIS, register-level access)
- **Compression:** RLE (Run-Length Encoding)
- **Communication:** UART (115200 baud)
- **Visualization:** Python 3 + pyserial

### Connection to PC
1. Connect **TX (PA9)** of STM32 to **RX** of the USB-UART adapter.
2. Connect **GND** of STM32 to **GND** of the adapter.
3. Plug the adapter into your PC's USB port.

### Signal Connections

| STM32 Pin | Function |
| :--- | :--- |
| **PA0 – PA7** | Signal inputs (8 channels) |
| **PB0** | Button |
| **PB2** | Status LED |
| **PA9** | TX (data to PC) |
| **PA10** | RX (data from PC, optional) |

> **Important:** All inputs are **3.3V tolerant only**. For 5V logic signals, use a voltage divider or level shifter.

## Build & Run

### 1. Build the Firmware
1. Open the project in your IDE (Keil, STM32CubeIDE, or use the Makefile).
2. Select the target MCU: **STM32F1xx**.
3. Build the project.
4. Flash the resulting .hex or .bin file using ST-Link, USB-UART, or another programmer.

### 2. Python Setup
1. Install Python 3 if you don't have it.
2. Install dependencies:
   pip install pyserial matplotlib numpy
3. Open logic_analyzer_gui.py and set the correct COM port:
   PORT = 'COM3'          # Windows
   # PORT = '/dev/ttyUSB0'  # Linux / macOS
4. Run the script:
   python logic_analyzer_gui.py

## How It Works

1. **Clock:** External 8 MHz crystal is multiplied via PLL to 72 MHz.
2. **Data capture:** Timer TIM2 generates 1 KHz ticks. On each tick, DMA (circular mode) copies data from GPIOA to the buffer.
3. **Processing:** DMA Half-Transfer (HT) and Transfer-Complete (TC) interrupts signal the main loop when data is ready.
4. **Compression:** RLE replaces repeated byte values with [value][count - 1] pairs.
5. **Button^** EXTI0 interrupt (PB0) starts/stops capture.
6. **Transmission:** Compressed data is sent via UART to the PC.
7. **Visualization:** Python sends data to data.log file.


## License

This project is distributed under the MIT License. See the LICENSE file for details.

## Author

**Yurii Ambartsumyan**
GitHub: https://github.com/YTenebris-STM
