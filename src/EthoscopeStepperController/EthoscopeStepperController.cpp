// ----------------------------------------------------------------------------
// EthoscopeStepperController.cpp
//
//
// Authors:
// Peter Polidoro peterpolidoro@gmail.com
// ----------------------------------------------------------------------------
#include "../EthoscopeStepperController.h"


using namespace ethoscope_stepper_controller;

EthoscopeStepperController::EthoscopeStepperController()
{
}

void EthoscopeStepperController::setup()
{
  // Parent Setup
  StepDirController::setup();

  // Reset Watchdog
  resetWatchdog();

  // Variable Setup

  // Set Device ID
  modular_server_.setDeviceName(constants::device_name);

  // Add Hardware

  pinMode(constants::sleep_pin,OUTPUT);

  // Pins

  // Add Firmware
  modular_server_.addFirmware(constants::firmware_info,
    properties_,
    parameters_,
    functions_,
    callbacks_);

  // Properties
  modular_server::Property & channel_count_property = modular_server_.property(step_dir_controller::constants::channel_count_property_name);
  channel_count_property.disableFunctors();
  channel_count_property.setDefaultValue(constants::channel_count);
  channel_count_property.setRange(constants::channel_count,constants::channel_count);
  channel_count_property.reenableFunctors();

  modular_server::Property & steps_per_position_units_property = modular_server_.property(step_dir_controller::constants::steps_per_position_units_property_name);
  steps_per_position_units_property.setDefaultValue(constants::steps_per_position_units_default);
  steps_per_position_units_property.setRange(constants::steps_per_position_units_min,constants::steps_per_position_units_max);

  modular_server::Property & velocity_max_property = modular_server_.property(step_dir_controller::constants::velocity_max_property_name);
  velocity_max_property.setUnits(constants::degrees_per_second_units);
  velocity_max_property.setDefaultValue(constants::velocity_max_default);
  velocity_max_property.setRange(constants::velocity_max_min,constants::velocity_max_max);

  modular_server::Property & velocity_min_property = modular_server_.property(step_dir_controller::constants::velocity_min_property_name);
  velocity_min_property.setUnits(constants::degrees_per_second_units);
  velocity_min_property.setDefaultValue(constants::velocity_min_default);
  velocity_min_property.setRange(constants::velocity_min_min,constants::velocity_min_max);

  modular_server::Property & acceleration_max_property = modular_server_.property(step_dir_controller::constants::acceleration_max_property_name);
  acceleration_max_property.setUnits(constants::degrees_per_second_per_second_units);
  acceleration_max_property.setDefaultValue(constants::acceleration_max_default);
  acceleration_max_property.setRange(constants::acceleration_max_min,constants::acceleration_max_max);

  // Parameters
  modular_server::Parameter & position_parameter = modular_server_.parameter(step_dir_controller::constants::position_parameter_name);
  position_parameter.setUnits(constants::degrees_units);

  modular_server::Parameter & velocity_parameter = modular_server_.parameter(step_dir_controller::constants::velocity_parameter_name);
  velocity_parameter.setUnits(constants::degrees_per_second_units);
  velocity_parameter.setRange(constants::velocity_min_min,constants::velocity_max_max);

  modular_server::Parameter & duration_parameter = modular_server_.createParameter(constants::duration_parameter_name);
  duration_parameter.setUnits(constants::millisecond_units);
  duration_parameter.setTypeLong();

  setChannelCountHandler();

  // Functions
  modular_server::Function & get_positions_function = modular_server_.function(step_dir_controller::constants::get_positions_function_name);
  get_positions_function.setResultUnits(constants::degrees_units);

  modular_server::Function & get_target_positions_function = modular_server_.function(step_dir_controller::constants::get_target_positions_function_name);
  get_target_positions_function.setResultUnits(constants::degrees_units);

  modular_server::Function & get_velocities_function = modular_server_.function(step_dir_controller::constants::get_velocities_function_name);
  get_velocities_function.setResultUnits(constants::degrees_per_second_units);

  modular_server::Function & get_target_velocities_function = modular_server_.function(step_dir_controller::constants::get_target_velocities_function_name);
  get_target_velocities_function.setResultUnits(constants::degrees_per_second_units);

  modular_server::Function & move_all_at_function = modular_server_.createFunction(constants::move_all_at_function_name);
  move_all_at_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&EthoscopeStepperController::moveAllAtHandler));
  move_all_at_function.addParameter(velocity_parameter);

  modular_server::Function & move_at_for_function = modular_server_.createFunction(constants::move_at_for_function_name);
  move_at_for_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&EthoscopeStepperController::moveAtForHandler));
  move_at_for_function.addParameter(channel_parameter);
  move_at_for_function.addParameter(velocity_parameter);
  move_at_for_function.addParameter(duration_parameter);

  // Callbacks

  wake();
}

void EthoscopeStepperController::sleep()
{
  digitalWrite(constants::sleep_pin,LOW);
}

void EthoscopeStepperController::wake()
{
  digitalWrite(constants::sleep_pin,HIGH);
}

void EthoscopeStepperController::moveAllAt(long velocity)
{
  for (size_t channel=0; channel<getChannelCount(); ++channel)
  {
    moveAt(channel,velocity);
  }
}

void StepDirController::moveAtFor(size_t channel,
  long velocity,
  long duration)
{
  if (channel < getChannelCount())
  {
  }
}

// Handlers must be non-blocking (avoid 'delay')
//
// modular_server_.parameter(parameter_name).getValue(value) value type must be either:
// fixed-point number (int, long, etc.)
// floating-point number (float, double)
// bool
// const char *
// ArduinoJson::JsonArray *
// ArduinoJson::JsonObject *
// const ConstantString *
//
// For more info read about ArduinoJson parsing https://github.com/janelia-arduino/ArduinoJson
//
// modular_server_.property(property_name).getValue(value) value type must match the property default type
// modular_server_.property(property_name).setValue(value) value type must match the property default type
// modular_server_.property(property_name).getElementValue(element_index,value) value type must match the property array element default type
// modular_server_.property(property_name).setElementValue(element_index,value) value type must match the property array element default type

void EthoscopeStepperController::moveAllAtHandler()
{
  long velocity;
  modular_server_.parameter(step_dir_controller::constants::velocity_parameter_name).getValue(velocity);
  moveAllAt(velocity);
}
