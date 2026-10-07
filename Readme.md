Digitally Regulated Boost Converter (Arduino Uno PID)

A closed-loop DC-DC Step-Up (Boost) Converter driven by an Arduino Uno R3. The system continuously samples output voltage through a $10\text{ k}\Omega + 10\text{ k}\Omega$ feedback divider, processes the reading through an Exponential Moving Average (EMA) filter, executes a $1\text{ kHz}$ discrete PID loop with anti-windup, and updates a custom $10\text{ kHz}$ Fast PWM signal on Timer 1.
---

## Features

* **Firmware:** Written for Arduino Uno R3 (ATmega328P).
* **Simulation:** Full circuit simulation in Proteus.
* **Hardware:** Custom PCB designed in KiCad.
* **Power Supply:** Input range `3V–4.2V DC` via terminal block.

---
