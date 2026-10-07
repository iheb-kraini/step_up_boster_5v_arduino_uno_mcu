#include <Arduino.h>

const uint8_t FB_PIN  = A0;
const uint8_t PWM_PIN = 9;

const uint16_t PWM_TOP = 1599;
const uint16_t PWM_MIN = 80;     // ~5%
const uint16_t PWM_MAX = 1439;   // ~90%

const float DIV_RATIO = 2.0;     // 10k + 10k divider
const float DUTY_FF   = 150;   //12% //640.0;   // ~40% feedforward

// Stable PID gains for 1 kHz loop
float Kp = 20;                 
float Ki = 5000.0;                
float Kd = 0.0;                 // D-term damps oscillation/overshoot

float target_voltage = 5.0;
float integral = 0.0;
float prev_error = 0.0;
float v_fb_filtered = 0.0;
float output_pwm = DUTY_FF;

unsigned long last_time = 0;
const unsigned int sample_interval_us = 1000; // 1 kHz

void setup() {
  pinMode(PWM_PIN, OUTPUT);

  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  TCCR1A |= (1 << COM1A1) | (1 << WGM11);
  TCCR1B |= (1 << WGM13) | (1 << WGM12) | (1 << CS10);
  ICR1  = PWM_TOP;
  OCR1A = (uint16_t)DUTY_FF;

  ADCSRA = (ADCSRA & 0xF8) | 0x04; // Prescaler 16 (~105 us per ADC read)
  last_time = micros();
}

void loop() {
  unsigned long now = micros();
  if (now - last_time < sample_interval_us) return;

  float dt = (now - last_time) / 1000000.0;
  last_time = now;

  // Single fast read + Exponential Moving Average filter (70% old, 30% new)
  float raw_v = (analogRead(FB_PIN) * 5.0 / 1023.0) * DIV_RATIO;
  v_fb_filtered = (0.7 * v_fb_filtered) + (0.3 * raw_v);

  float error = target_voltage - v_fb_filtered;

  // Integral accumulation with anti-windup clamp
  float next_integral = integral + error * dt;
  next_integral = constrain(next_integral, -1.5, 1.5);

  // Derivative calculation for damping
  float derivative = (error - prev_error) / dt;
  prev_error = error;

  // PID Output
  output_pwm = DUTY_FF + (Kp * error) + (Ki * next_integral) + (Kd * derivative);

  // Update integral only if output is not saturated (Anti-Windup)
  if (output_pwm >= PWM_MIN && output_pwm <= PWM_MAX) {
    integral = next_integral;
  }

  output_pwm = constrain(output_pwm, PWM_MIN, PWM_MAX);
  OCR1A = (uint16_t)output_pwm;
}