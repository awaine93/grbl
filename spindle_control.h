/*
  spindle_control.h - spindle control methods
  Part of Grbl

  Copyright (c) 2012-2016 Sungeun K. Jeon for Gnea Research LLC
  Copyright (c) 2009-2011 Simen Svale Skogsrud

  Grbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Grbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Grbl.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef spindle_control_h
#define spindle_control_h

#define SPINDLE_NO_SYNC false
#define SPINDLE_FORCE_SYNC true

#define SPINDLE_STATE_DISABLE  0  // Must be zero.
#define SPINDLE_STATE_CW       bit(0)
#define SPINDLE_STATE_CCW      bit(1)


// Initializes spindle pins and hardware PWM, if enabled.
// BTS7960: Configures Timer2A (RPWM/D11 forward) and Timer1B (LPWM/D10 reverse).
// Both outputs are initialised LOW. R_EN and L_EN must be permanently tied to 5V.
void spindle_init();

// Returns current spindle output state. Overrides may alter it from programmed states.
uint8_t spindle_get_state();

#ifdef VARIABLE_SPINDLE

  // Called by g-code parser when setting spindle state and requires a buffer sync.
  void spindle_sync(uint8_t state, float rpm);

  // Sets spindle running state with direction, enable, and spindle PWM.
  // BTS7960: Always transitions through a dead-stop (both outputs zero) before
  // changing direction. RPWM and LPWM are NEVER active simultaneously.
  void spindle_set_state(uint8_t state, float rpm);

  // Sets RPWM (forward/D11) PWM output for stepper ISR and forward operation.
  // Also called by spindle_set_state(). LPWM is guaranteed zero before this runs.
  // NOTE: 328p PWM register is 8-bit.
  void spindle_set_speed(uint8_t pwm_value);

  // Sets LPWM (reverse/D10) PWM output for reverse operation.
  // RPWM is guaranteed zero before this runs.
  // NOTE: 328p PWM register is 8-bit.
  void spindle_set_speed_reverse(uint8_t pwm_value);

  // Computes 328p-specific PWM register value for the given RPM for quick updating.
  uint8_t spindle_compute_pwm_value(float rpm);

#else

  // Called by g-code parser when setting spindle state and requires a buffer sync.
  #define spindle_sync(state, rpm) _spindle_sync(state)
  void _spindle_sync(uint8_t state);

  // Sets spindle running state with direction and enable.
  #define spindle_set_state(state, rpm) _spindle_set_state(state)
  void _spindle_set_state(uint8_t state);

#endif

// Immediately stops both RPWM and LPWM outputs and disables both PWM timers.
// SAFETY: This is the only safe way to transition between forward and reverse.
// Called by spindle_init(), spindle_set_state(), spindle_set_speed(),
// spindle_set_speed_reverse(), and mc_reset().
void spindle_stop();


#endif