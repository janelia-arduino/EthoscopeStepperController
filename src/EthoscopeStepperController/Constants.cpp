// ----------------------------------------------------------------------------
// Constants.cpp
//
//
// Authors:
// Peter Polidoro peterpolidoro@gmail.com
// ----------------------------------------------------------------------------
#include "Constants.h"


namespace ethoscope_stepper_controller
{
namespace constants
{
CONSTANT_STRING(device_name,"ethoscope_stepper_controller");

CONSTANT_STRING(firmware_name,"EthoscopeStepperController");
// Use semantic versioning http://semver.org/
const modular_server::FirmwareInfo firmware_info =
{
  .name_ptr=&firmware_name,
  .version_major=1,
  .version_minor=1,
  .version_patch=0,
};

// Pins

// Units
CONSTANT_STRING(degrees_units,"degrees");
CONSTANT_STRING(degrees_per_second_units,"degrees/s");
CONSTANT_STRING(degrees_per_second_per_second_units,"degrees/s/s");
CONSTANT_STRING(millisecond_units,"ms");

// Properties
// (2048 steps/rev * 16 usteps/step) / 360 degrees = 91 usteps/deg
const long steps_per_position_units_min = 91;
const long steps_per_position_units_max = 91;
const long steps_per_position_units_element_default = 91;

const long velocity_max_min = 10;
const long velocity_max_max = 2520;
const long velocity_max_element_default = 360;

const long velocity_min_min = -2520;
const long velocity_min_max = -10;
const long velocity_min_element_default = -360;

const long acceleration_max_min = 10;
const long acceleration_max_max = 2520;
const long acceleration_max_element_default = 180;

// Parameters
CONSTANT_STRING(duration_parameter_name,"duration");
const long duration_min = 10;
const long duration_max = 2147483647;

// Functions
CONSTANT_STRING(all_sleeping_function_name,"allSleeping");
CONSTANT_STRING(move_all_at_function_name,"moveAllAt");
CONSTANT_STRING(move_at_for_function_name,"moveAtFor");

// Callbacks
CONSTANT_STRING(wake_all_callback_name,"wakeAll");
CONSTANT_STRING(sleep_all_callback_name,"sleepAll");

// Errors
}
}
