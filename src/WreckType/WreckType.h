// WreckType: the direction a rider is thrown off in a wreck (JetskiVehicle::
// TriggerWreck, WaterWrecker's separation motions). Shared so that
// JetskiVehicle.h and WaterWrecker.h can be included together.
#ifndef WRECKTYPE_WRECKTYPE_H
#define WRECKTYPE_WRECKTYPE_H

enum WreckType {
    WRECK_FRONT = 0,
    WRECK_LEFT = 1,
    WRECK_RIGHT = 2,
    WRECK_REAR = 3,
    WRECK_UP = 4
};

#endif
