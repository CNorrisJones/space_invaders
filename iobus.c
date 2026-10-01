#include "iobus.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

bool IoBusIsValid(const IoBus *bus) {
    if (bus == NULL) {
        return false;
    }

    if (bus->hardware_context == NULL) {
        return false;
    }

    if (bus->read == NULL) {
        return false;
    }

    if (bus->write == NULL) {
        return false;
    }

    return true;
}

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

    if (!IoBusIsValid(io_bus)) {
        free(io_bus);
        return NULL;
    }

    return io_bus;
}

void IoBusShutdown(IoBus *io_bus) {
    if (io_bus == NULL) {
        return;
    }

    free(io_bus);
    return;
}
