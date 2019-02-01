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

  void sleep();
  void wake();

  void moveAllAt(long velocity);
  void moveAtFor(size_t channel,
    long velocity,
    long duration);

private:
  modular_server::Property properties_[ethoscope_stepper_controller::constants::PROPERTY_COUNT_MAX];
  modular_server::Parameter parameters_[ethoscope_stepper_controller::constants::PARAMETER_COUNT_MAX];
  modular_server::Function functions_[ethoscope_stepper_controller::constants::FUNCTION_COUNT_MAX];
  modular_server::Callback callbacks_[ethoscope_stepper_controller::constants::CALLBACK_COUNT_MAX];

  // Handlers
  void moveAllAtHandler();
  void moveAtForHandler();

};

#endif
