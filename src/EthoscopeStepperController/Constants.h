// ----------------------------------------------------------------------------
// Constants.h
//
//
// Authors:
// Peter Polidoro peterpolidoro@gmail.com
// ----------------------------------------------------------------------------
#ifndef ETHOSCOPE_STEPPER_CONTROLLER_CONSTANTS_H
#define ETHOSCOPE_STEPPER_CONTROLLER_CONSTANTS_H
#include <ConstantVariable.h>
#include <StepDirController.h>


namespace ethoscope_stepper_controller
{
namespace constants
{
//MAX values must be >= 1, >= created/copied count, < RAM limit
enum{PROPERTY_COUNT_MAX=1};
enum{PARAMETER_COUNT_MAX=1};
enum{FUNCTION_COUNT_MAX=1};
enum{CALLBACK_COUNT_MAX=1};

extern ConstantString device_name;

extern ConstantString firmware_name;
extern const modular_server::FirmwareInfo firmware_info;

extern const long channel_count;

// Pins

// Units

// Properties
// Property values must be long, double, bool, long[], double[], bool[], char[], ConstantString *, (ConstantString *)[]
extern const long steps_per_position_units_min;
extern const long steps_per_position_units_max;
extern const long steps_per_position_units_element_default;

extern const long velocity_max_min;
extern const long velocity_max_max;
extern const long velocity_max_element_default;

extern const long velocity_min_min;
extern const long velocity_min_max;
extern const long velocity_min_element_default;

extern const long acceleration_max_min;
extern const long acceleration_max_max;
extern const long acceleration_max_element_default;

// Parameters

// Functions

// Callbacks

// Errors
}
}
#include "5x3.h"
#include "3x2.h"
#endif
