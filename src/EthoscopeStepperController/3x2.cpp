// ----------------------------------------------------------------------------
// 3x2.cpp
//
//
// Authors:
// Peter Polidoro peterpolidoro@gmail.com
// ----------------------------------------------------------------------------
#include "3x2.h"


#if defined(__MK20DX256__)

namespace ethoscope_stepper_controller
{
namespace constants
{
const long channel_count = CHANNEL_COUNT;

// Pins

// Units

// Properties
const long steps_per_position_units_default[CHANNEL_COUNT] =
{
  steps_per_position_units_element_default,
  steps_per_position_units_element_default,
  steps_per_position_units_element_default,
};

const long velocity_max_default[CHANNEL_COUNT] =
{
  velocity_max_element_default,
  velocity_max_element_default,
  velocity_max_element_default,
};

const long velocity_min_default[CHANNEL_COUNT] =
{
  velocity_min_element_default,
  velocity_min_element_default,
  velocity_min_element_default,
};

const long acceleration_max_default[CHANNEL_COUNT] =
{
  acceleration_max_element_default,
  acceleration_max_element_default,
  acceleration_max_element_default,
};

// Parameters

// Functions

// Callbacks

// Errors
}
}
#endif
