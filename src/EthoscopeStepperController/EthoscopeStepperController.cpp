// ----------------------------------------------------------------------------
// EthoscopeStepperController.cpp
//
//
// Authors:
// Peter Polidoro peter@polidoro.io
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
    oscillate_info_[channel].move_count = 0;
    oscillate_info_[channel].move_count_inc = 0;
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

  modular_server::Parameter & count_parameter = modular_server_.createParameter(constants::count_parameter_name);
  count_parameter.setRange(constants::count_min,constants::count_max);

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

  modular_server::Function & oscillate_function = modular_server_.createFunction(constants::oscillate_function_name);
  oscillate_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&EthoscopeStepperController::oscillateHandler));
  oscillate_function.addParameter(channel_parameter);
  oscillate_function.addParameter(velocity_parameter);
  oscillate_function.addParameter(duration_parameter);
  oscillate_function.addParameter(acceleration_parameter);
  oscillate_function.addParameter(deceleration_parameter);
  oscillate_function.addParameter(count_parameter);

  // Callbacks
  modular_server::Callback & wake_all_callback = modular_server_.createCallback(constants::wake_all_callback_name);
  wake_all_callback.attachFunctor(makeFunctor((Functor1<modular_server::Pin *> *)0,*this,&EthoscopeStepperController::wakeAllHandler));

  modular_server::Callback & sleep_all_callback = modular_server_.createCallback(constants::sleep_all_callback_name);
  sleep_all_callback.attachFunctor(makeFunctor((Functor1<modular_server::Pin *> *)0,*this,&EthoscopeStepperController::sleepAllHandler));

}

void EthoscopeStepperController::stop(size_t channel)
{
  if (channel >= getChannelCount())
  {
    return;
  }
  event_controller_.remove(move_event_ids_[channel]);
  StepDirController::stop(channel);
  oscillate_info_[channel].move_count = 0;
  oscillate_info_[channel].move_count_inc = 0;
}

void EthoscopeStepperController::stopAll()
{
  for (size_t channel=0; channel<getChannelCount(); ++channel)
  {
    stop(channel);
  }
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
  if ((channel >= getChannelCount()) || (duration_ms <= 0))
  {
    return;
  }
  long acceleration_upper_limit = getAccelerationUpperLimit(channel,velocity);
  if (acceleration_upper_limit > constants::acceleration_max_max)
  {
    acceleration_upper_limit = constants::acceleration_max_max;
  }
  long acceleration_lower_limit = getAccelerationLowerLimit(channel,velocity);
  if (acceleration_lower_limit < constants::acceleration_max_min)
  {
    acceleration_lower_limit = constants::acceleration_max_min;
  }
  acceleration = constrain(acceleration,acceleration_lower_limit,acceleration_upper_limit);
  deceleration = constrain(deceleration,acceleration_lower_limit,acceleration_upper_limit);

  long acceleration_duration_ms = abs(velocity * constants::milliseconds_per_second) / acceleration;
  long deceleration_duration_ms = abs(velocity * constants::milliseconds_per_second) / deceleration;
  long event_delay_ms = duration_ms - deceleration_duration_ms;
  deceleration_[channel] = deceleration;
  deceleration_velocity_[channel] = velocity;
  if (duration_ms < (acceleration_duration_ms + deceleration_duration_ms))
  {
    event_delay_ms = (duration_ms * acceleration_duration_ms) / (acceleration_duration_ms + deceleration_duration_ms);
  }
  // event_controller_.remove(move_event_ids_[channel]);
  // StepDirController::stop(channel);
  move_event_ids_[channel] = event_controller_.addEventUsingDelay(makeFunctor((Functor1<int> *)0,*this,&EthoscopeStepperController::stopMoveHandler),
    event_delay_ms,
    channel);
  temporarilySetLimits(channel,constants::velocity_min_min,velocity,acceleration);
  moveAt(channel,velocity);
  event_controller_.enable(move_event_ids_[channel]);
}

void EthoscopeStepperController::oscillate(size_t channel,
  long velocity,
  long duration_ms,
  long acceleration,
  long deceleration,
  long count)
{
  if ((channel >= getChannelCount()) ||
    (duration_ms <= 0) ||
    (count < constants::count_min) ||
    (count > constants::count_max))
  {
    return;
  }
  oscillate_info_[channel].move_count = count * 2;
  oscillate_info_[channel].move_count_inc = 1;
  oscillate_info_[channel].velocity = velocity;
  oscillate_info_[channel].duration = duration_ms;
  oscillate_info_[channel].acceleration = acceleration;
  oscillate_info_[channel].deceleration = deceleration;

  moveAtFor(channel,velocity,duration_ms,acceleration,deceleration);
}

// Handlers must be non-blocking (avoid 'delay')
//
// modular_server_.parameter(parameter_name).getValue(value) value type must be either:
// fixed-point number (int, long, etc.)
// floating-point number (float, double)
// bool
// const char *
// ArduinoJson::JsonArray
// ArduinoJson::JsonObject
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

void EthoscopeStepperController::oscillateHandler()
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

  long count;
  modular_server_.parameter(constants::count_parameter_name).getValue(count);

  oscillate(channel,
    velocity,
    duration,
    acceleration,
    deceleration,
    count);
}

void EthoscopeStepperController::stopMoveHandler(int channel)
{
  long deceleration = deceleration_[channel];
  long velocity = deceleration_velocity_[channel];
  temporarilySetLimits(channel,constants::velocity_min_min,velocity,deceleration);
  StepDirController::stop(channel);
  long event_delay = abs(velocity * constants::milliseconds_per_second) / deceleration;
  move_event_ids_[channel] = event_controller_.addEventUsingDelay(makeFunctor((Functor1<int> *)0,*this,&EthoscopeStepperController::finishMoveHandler),
    event_delay,
    channel);
  event_controller_.enable(move_event_ids_[channel]);
}

void EthoscopeStepperController::finishMoveHandler(int channel)
{
  if (atTargetVelocity(channel))
  {
    restoreLimits(channel);
    if (oscillate_info_[channel].move_count_inc < oscillate_info_[channel].move_count)
    {
      long direction = 1;
      if ((oscillate_info_[channel].move_count_inc % 2) != 0)
      {
        direction = -1;
      }
      ++oscillate_info_[channel].move_count_inc;
      moveAtFor(channel,
        (direction * oscillate_info_[channel].velocity),
        oscillate_info_[channel].duration,
        oscillate_info_[channel].acceleration,
        oscillate_info_[channel].deceleration);
    }
    else
    {
      oscillate_info_[channel].move_count = 0;
      oscillate_info_[channel].move_count_inc = 0;
    }
  }
  else
  {
    move_event_ids_[channel] = event_controller_.addEventUsingDelay(makeFunctor((Functor1<int> *)0,*this,&EthoscopeStepperController::finishMoveHandler),
      constants::finish_move_delay,
      channel);
    event_controller_.enable(move_event_ids_[channel]);
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
