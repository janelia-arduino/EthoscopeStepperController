// ----------------------------------------------------------------------------
// EthoscopeStepperController.h
//
//
// Authors:
// Peter Polidoro peterpolidoro@gmail.com
// ----------------------------------------------------------------------------
#ifndef ETHOSCOPE_STEPPER_CONTROLLER_H
#define ETHOSCOPE_STEPPER_CONTROLLER_H
#include <ArduinoJson.h>
#include <JsonStream.h>
#include <Array.h>
#include <Vector.h>
#include <ConstantVariable.h>
#include <Functor.h>

#include <EventController.h>

#include <ModularServer.h>
#include <ModularDeviceBase.h>
#include <StepDirController.h>

#include "EthoscopeStepperController/Constants.h"


class EthoscopeStepperController : public StepDirController
{
public:
  EthoscopeStepperController();
  virtual void setup();

  virtual void stop(size_t channel);
  virtual void stopAll();

  void wakeAll();
  void sleepAll();
  bool allSleeping();

  void moveAllAt(long velocity);
  void moveAtFor(size_t channel,
    long velocity,
    long duration_ms,
    long acceleration,
    long deceleration);

  void oscillate(size_t channel,
    long velocity,
    long duration_ms,
    long acceleration,
    long deceleration,
    long count);
  void oscillateAll(long velocity,
    long duration_ms,
    long acceleration,
    long deceleration,
    long count);

private:
  modular_server::Property properties_[ethoscope_stepper_controller::constants::PROPERTY_COUNT_MAX];
  modular_server::Parameter parameters_[ethoscope_stepper_controller::constants::PARAMETER_COUNT_MAX];
  modular_server::Function functions_[ethoscope_stepper_controller::constants::FUNCTION_COUNT_MAX];
  modular_server::Callback callbacks_[ethoscope_stepper_controller::constants::CALLBACK_COUNT_MAX];

  bool all_sleeping_;

  EventController<ethoscope_stepper_controller::constants::EVENT_COUNT_MAX> event_controller_;
  EventId event_ids_[ethoscope_stepper_controller::constants::CHANNEL_COUNT];

  long deceleration_[ethoscope_stepper_controller::constants::CHANNEL_COUNT];
  long deceleration_velocity_[ethoscope_stepper_controller::constants::CHANNEL_COUNT];

  // Handlers
  void allSleepingHandler();
  void moveAllAtHandler();
  void moveAtForHandler();
  void oscillateHandler();
  void oscillateAllHandler();
  void stopEventHandler(int channel);
  void restoreEventHandler(int channel);
  void wakeAllHandler(modular_server::Pin * pin_ptr);
  void sleepAllHandler(modular_server::Pin * pin_ptr);

};

#endif
