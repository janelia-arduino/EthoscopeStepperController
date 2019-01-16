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
  .version_minor=0,
  .version_patch=0,
};

// Pins

// Units

// Properties
const long steps_per_position_units_min = 2048;
const long steps_per_position_units_max = 2048;
const long steps_per_position_units_element_default = 2048;

const long velocity_max_min = 10;
const long velocity_max_max = 100;
const long velocity_max_element_default = 32;

const long velocity_min_min = -100;
const long velocity_min_max = -10;
const long velocity_min_element_default = -32;

const long acceleration_max_min = 10;
const long acceleration_max_max = 100;
const long acceleration_max_element_default = 10;

// Parameters

// Functions
CONSTANT_STRING(move_all_at_function_name,"moveAllAt");

// Callbacks

// Errors
}
}
