#include <EthoscopeStepperController.h>


EthoscopeStepperController dev;

void setup()
{
  dev.setup();
  dev.startServer();
}

void loop()
{
  dev.update();
}
