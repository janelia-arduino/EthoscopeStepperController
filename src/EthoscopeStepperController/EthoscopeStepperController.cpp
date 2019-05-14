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

  // Event Controller Setup
  event_controller_.setup();

  // Variable Setup
  for (size_t channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    deceleration_[channel] = constants::acceleration_max_min;
    deceleration_velocity_[channel] = constants::velocity_max_max;
  }

  // Set Device ID
  modular_server_.setDeviceName(constants::device_name);

  // Add Hardware

  pinMode(constants::sleep_pin,OUTPUT);
  sleepAll();

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
  modular_server::Parameter & channel_parameter = modular_server_.parameter(step_dir_controller::constants::channel_parameter_name);

  modular_server::Parameter & position_parameter = modular_server_.parameter(step_dir_controller::constants::position_parameter_name);
  position_parameter.setUnits(constants::degrees_units);

  modular_server::Parameter & velocity_parameter = modular_server_.parameter(step_dir_controller::constants::velocity_parameter_name);
  velocity_parameter.setUnits(constants::degrees_per_second_units);
  velocity_parameter.setRange(-constants::velocity_max_max,constants::velocity_max_max);

  modular_server::Parameter & duration_parameter = modular_server_.createParameter(constants::duration_parameter_name);
  duration_parameter.setUnits(constants::millisecond_units);
  duration_parameter.setRange(constants::duration_min,constants::duration_max);
  duration_parameter.setTypeLong();

  modular_server::Parameter & acceleration_parameter = modular_server_.createParameter(constants::acceleration_parameter_name);
  acceleration_parameter.setUnits(constants::degrees_per_second_per_second_units);
  acceleration_parameter.setRange(constants::acceleration_max_min,constants::acceleration_max_max);
  acceleration_parameter.setTypeLong();

  modular_server::Parameter & deceleration_parameter = modular_server_.createParameter(constants::deceleration_parameter_name);
  deceleration_parameter.setUnits(constants::degrees_per_second_per_second_units);
  deceleration_parameter.setRange(constants::acceleration_max_min,constants::acceleration_max_max);
  deceleration_parameter.setTypeLong();

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

  modular_server::Function & all_sleeping_function = modular_server_.createFunction(constants::all_sleeping_function_name);
  all_sleeping_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&EthoscopeStepperController::allSleepingHandler));
  all_sleeping_function.setResultTypeBool();

  modular_server::Function & move_all_at_function = modular_server_.createFunction(constants::move_all_at_function_name);
  move_all_at_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&EthoscopeStepperController::moveAllAtHandler));
  move_all_at_function.addParameter(velocity_parameter);

  modular_server::Function & move_at_for_function = modular_server_.createFunction(constants::move_at_for_function_name);
  move_at_for_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&EthoscopeStepperController::moveAtForHandler));
  move_at_for_function.addParameter(channel_parameter);
  move_at_for_function.addParameter(velocity_parameter);
  move_at_for_function.addParameter(duration_parameter);
  move_at_for_function.addParameter(acceleration_parameter);
  move_at_for_function.addParameter(deceleration_parameter);

  // Callbacks
  modular_server::Callback & wake_all_callback = modular_server_.createCallback(constants::wake_all_callback_name);
  wake_all_callback.attachFunctor(makeFunctor((Functor1<modular_server::Pin *> *)0,*this,&EthoscopeStepperController::wakeAllHandler));

  modular_server::Callback & sleep_all_callback = modular_server_.createCallback(constants::sleep_all_callback_name);
  sleep_all_callback.attachFunctor(makeFunctor((Functor1<modular_server::Pin *> *)0,*this,&EthoscopeStepperController::sleepAllHandler));

}

void EthoscopeStepperController::stop(size_t channel)
{
  if (channel < getChannelCount())
  {
    event_controller_.remove(event_ids_[channel]);
  }
  StepDirController::stop(channel);
}

void EthoscopeStepperController::stopAll()
{
  for (size_t channel=0; channel<getChannelCount(); ++channel)
  {
    event_controller_.remove(event_ids_[channel]);
  }
  StepDirController::stopAll();
}

void EthoscopeStepperController::wakeAll()
{
  digitalWrite(constants::sleep_pin,HIGH);
  all_sleeping_ = false;
}

void EthoscopeStepperController::sleepAll()
{
  digitalWrite(constants::sleep_pin,LOW);
  all_sleeping_ = true;
}

bool EthoscopeStepperController::allSleeping()
{
  return all_sleeping_;
}

void EthoscopeStepperController::moveAllAt(long velocity)
{
  for (size_t channel=0; channel<getChannelCount(); ++channel)
  {
    moveAt(channel,velocity);
  }
}

void EthoscopeStepperController::moveAtFor(size_t channel,
  long velocity,
  long duration_ms,
  long acceleration,
  long deceleration)
{
  if ((channel < getChannelCount()) && (duration_ms > 0))
  {
    if (acceleration < constants::acceleration_max_min)
    {
      acceleration = constants::acceleration_max_min;
    }
    if (deceleration < constants::acceleration_max_min)
    {
      deceleration = constants::acceleration_max_min;
    }

    long acceleration_duration_ms = (velocity * constants::milliseconds_per_second) / acceleration;
    long deceleration_duration_ms = (velocity * constants::milliseconds_per_second) / deceleration;
    long event_delay_ms = duration_ms - deceleration_duration_ms;
    deceleration_[channel] = deceleration;
    deceleration_velocity_[channel] = velocity;
    if (duration_ms < (acceleration_duration_ms + deceleration_duration_ms))
    {
      event_delay_ms = (duration_ms * acceleration_duration_ms) / (acceleration_duration_ms + deceleration_duration_ms);
    }
    event_ids_[channel] = event_controller_.addEventUsingDelay(makeFunctor((Functor1<int> *)0,*this,&EthoscopeStepperController::stopEventHandler),
      event_delay_ms,
      channel);
    temporarilySetLimits(channel,constants::velocity_min_min,velocity,acceleration);
    moveAt(channel,velocity);
    event_controller_.enable(event_ids_[channel]);
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

void EthoscopeStepperController::allSleepingHandler()
{
  modular_server_.response().returnResult(allSleeping());
}

void EthoscopeStepperController::moveAllAtHandler()
{
  long velocity;
  modular_server_.parameter(step_dir_controller::constants::velocity_parameter_name).getValue(velocity);

  moveAllAt(velocity);
}

void EthoscopeStepperController::moveAtForHandler()
{
  long channel;
  modular_server_.parameter(step_dir_controller::constants::channel_parameter_name).getValue(channel);

  long velocity;
  modular_server_.parameter(step_dir_controller::constants::velocity_parameter_name).getValue(velocity);

  long duration;
  modular_server_.parameter(constants::duration_parameter_name).getValue(duration);

  long acceleration;
  modular_server_.parameter(constants::acceleration_parameter_name).getValue(acceleration);

  long deceleration;
  modular_server_.parameter(constants::deceleration_parameter_name).getValue(deceleration);

  moveAtFor(channel,
    velocity,
    duration,
    acceleration,
    deceleration);
}

void EthoscopeStepperController::stopEventHandler(int channel)
{
  long deceleration = deceleration_[channel];
  long velocity = deceleration_velocity_[channel];
  temporarilySetLimits(channel,constants::velocity_min_min,velocity,deceleration);
  StepDirController::stop(channel);
  long event_delay = (velocity * constants::milliseconds_per_second) / deceleration;
  event_ids_[channel] = event_controller_.addEventUsingDelay(makeFunctor((Functor1<int> *)0,*this,&EthoscopeStepperController::restoreEventHandler),
    event_delay,
    channel);
  event_controller_.enable(event_ids_[channel]);
}

void EthoscopeStepperController::restoreEventHandler(int channel)
{
  if (atTargetVelocity(channel))
  {
    restoreLimits(channel);
  }
  else
  {
    event_ids_[channel] = event_controller_.addEventUsingDelay(makeFunctor((Functor1<int> *)0,*this,&EthoscopeStepperController::restoreEventHandler),
      constants::restore_event_delay,
      channel);
    event_controller_.enable(event_ids_[channel]);
  }
}

void EthoscopeStepperController::wakeAllHandler(modular_server::Pin * pin_ptr)
{
  wakeAll();
}

void EthoscopeStepperController::sleepAllHandler(modular_server::Pin * pin_ptr)
{
  sleepAll();
}
