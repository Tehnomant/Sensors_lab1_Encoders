# Encoder Reading with Timer (TIM2) for STM32

## Overview
This program reads a rotary encoder using STM32's hardware timer (TIM2) in encoder mode and outputs position and angle data via UART (USART2) at 115200 baud.

## Hardware Setup

### Pin Configuration (Timer Encoder Mode)
- **Encoder Channel A**: TIM2_CH1 pin (depends on MCU - typically PA0 or PA5)
- **Encoder Channel B**: TIM2_CH2 pin (depends on MCU - typically PA1 or PA6)

### Connection Notes
1. Connect encoder pins to the TIM2 channel pins of your STM32
2. No internal pull-up needed - timer handles signal conditioning
3. Encoder power supply: 3.3V (match MCU logic level)

## Encoder Specifications
- **Counts per Revolution**: 80 counts (for KY-040 with x4 counting)
- **Output Resolution**: 1 count = 45° (3600/80 = 45 angle units)
- **Timer Settings**: 
  - TIM2 with 32-bit counter (0-4,294,967,295)
  - No prescaler (direct counting)
  - Encoder mode: TI12 (counting on both edges)
  - Digital filter: 4 (noise reduction)

## Software Features
- Hardware quadrature decoding via TIM2
- Automatic direction detection
- Position tracking with 32-bit counter
- Angle calculation (position × 45°)
- Serial output in CSV format: `t_ms,pos,angle_x10`
- Polling-based position reading (no interrupts)

## Output Format
The program outputs data every 10ms in CSV format:
```
timestamp_ms,position,angle_x10
```
Where:
- `angle_x10` = angle in degrees × 10 (for integer precision)

## Configuration Notes
1. **TIM2 Setup**:
   - Encoder mode: TIM_ENCODERMODE_TI12
   - Both channels: Rising edge polarity
   - Input filter: 4 (adjust for your encoder)
   - Counter period: 4294967295 (32-bit max)

2. **GPIO**: Set to input mode (pull-up not needed in timer mode)

3. **COUNT_PER_REV**: Adjust this define based on your encoder:
   - KY-040: 20 pulses/rev × 4 = 80 counts
   - Other encoders: Calculate accordingly

## Compilation & Usage
1. Configure TIM2 in encoder mode with correct channel pins
2. Set USART2 for 115200 baud, 8N1
3. Connect serial monitor to view output
4. The program starts immediately after initialization

## Advantages over Software Decoding
- Higher maximum rotation speed
- No CPU load for counting
- Built-in noise filtering
- More reliable at high speeds
- 32-bit counter prevents overflow

## Notes
- Counter wraps automatically after 4,294,967,295 counts
- Filter value (4) can be adjusted for noise vs. response time
- Check your MCU datasheet for TIM2 pin mapping
- Works with any quadrature encoder (not just KY-040)
