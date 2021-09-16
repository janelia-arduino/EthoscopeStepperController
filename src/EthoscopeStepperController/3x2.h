// ----------------------------------------------------------------------------
// 3x2.h
//
//
// Authors:
// Peter Polidoro peter@polidoro.io
// ----------------------------------------------------------------------------
#ifndef ETHOSCOPE_STEPPER_CONTROLLER_3X2_CONSTANTS_H
#define ETHOSCOPE_STEPPER_CONTROLLER_3X2_CONSTANTS_H
#include "Constants.h"


#if defined(__MK20DX256__)

namespace ethoscope_stepper_controller
{
namespace constants
{
enum{CHANNEL_COUNT=3};

// Pins

// Units

// Properties
// Property values must be long, double, bool, long[], double[], bool[], char[], ConstantString *, (ConstantString *)[]
extern const long steps_per_position_units_default[CHANNEL_COUNT];

extern const long velocity_max_default[CHANNEL_COUNT];

extern const long velocity_min_default[CHANNEL_COUNT];

extern const long acceleration_max_default[CHANNEL_COUNT];

// Parameters

// Functions

// Callbacks

// Errors
}
}
#endif
#endif
