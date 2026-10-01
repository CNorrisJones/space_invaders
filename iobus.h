#ifndef IOBUS_H
#define IOBUS_H

#include <stdint.h>
#include <stdbool.h>

typedef uint8_t (*IoBusReadCallback)(void *context, uint8_t port);
typedef void (*IoBusWriteCallback)(void *context, uint8_t port, uint8_t value);

typedef struct IoBus {
    void *hardware_context;
    IoBusReadCallback read;
    IoBusWriteCallback write;
} IoBus;



bool IoBusIsValid(const IoBus *bus);
IoBus *IoBusInit(void *hardware_context, IoBusReadCallback read, IoBusWriteCallback write);
void IoBusShutdown(IoBus *io_bus);

#endif
