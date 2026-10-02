#include "iobus.h"
#include <stddef.h>
#include <stdlib.h>

IoBus *IoBusInit(void *hardware_context, IoBusReadCallback read, IoBusWriteCallback write) {
    if (hardware_context == NULL || read == NULL || write == NULL) {
        return NULL;
    }

    IoBus *io_bus = malloc(sizeof(*io_bus));
    if (io_bus == NULL) {
        return NULL;
    }

    io_bus->hardware_context = hardware_context;
    io_bus->read = read;
    io_bus->write = write;

    return io_bus;
}

void IoBusShutdown(IoBus *io_bus) {
    if (io_bus == NULL) {
        return;
    }

    free(io_bus);
    return;
}
