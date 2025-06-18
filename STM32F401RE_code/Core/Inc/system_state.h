#ifndef __SYSTEM_STATE_H
#define __SYSTEM_STATE_H

typedef enum {
    SYSTEM_STOPPED = 0,
    SYSTEM_RUNNING = 1
} SystemState_t;

typedef enum {
    GAS_SAFE = 0,
    GAS_LOW = 1,
    GAS_HIGH = 2,
    GAS_DANGEROUS = 3
} GasAlertLevel_t;

#endif
