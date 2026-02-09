# Encoder Reading Program for STM32

## Overview
This program reads a rotary encoder connected to an STM32 microcontroller and outputs position and angle data via UART (USART2) at 115200 baud rate.

## Hardware Setup

### Pin Configuration
- **Encoder CLK (Channel A)**: PC1 (GPIOC, Pin 1)
- **Encoder DT (Channel B)**: PC0 (GPIOC, Pin 0)

### Connection Notes
1. Both encoder pins must be connected with pull-up resistors enabled internally
2. Encoder power supply should match the microcontroller's logic level (3.3V typical)

## Encoder Specifications
- **Counts per Revolution**: 80 counts
- **Debounce Time**: 2 ms
- **Output Resolution**: 1 count = 45° (3600/80 = 45 angle units)

## Software Features
- Quadrature decoding with 4 states per cycle
- Software debouncing
- Position tracking (32-bit signed integer)
- Angle calculation (position × 45°)
- Serial output in CSV format: `t_ms,pos,angle_x10`

## Output Format
The program outputs data every 10ms in CSV format:
```
timestamp_ms,position,angle_x10
```
Where:
- `angle_x10` = angle in degrees × 10 (for integer precision)

## Compilation & Usage
1. Configure your STM32 project with the provided pin definitions
2. Ensure USART2 is configured for 115200 baud, 8N1
3. Connect serial monitor to view the output
4. The program starts outputting data immediately after initialization

## Notes
- Maximum rotation speed is limited by the 2ms debounce time
- Position counter will wrap around after ±2,147,483,647 counts
- No external interrupts are used - polling method only
