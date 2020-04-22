# Kingswood Environment Monitor environment sensor firmware

This is the firmware for a sensor for measuring temperature, pressure, humidity, light
and atmospheric pressure and, as a LoRa client, relaying it over a LoRa link to a LoRa server.

## Protobuf

To regenerate the schema, run this command:

```
mkdir -p lib/proto
python .pio/libdeps/feather32u4/Nanopb/generator/nanopb_generator.py -I proto -D lib/proto proto/packet.proto
```

There is a compiler namespace DEFINE collision on `atmelavr` platforms with the `UENUM` type. 
As a workaround, insert the following in `packet.pb.h`:

```
/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.2-dev */

#ifndef PB_PACKET_PB_H_INCLUDED
#define PB_PACKET_PB_H_INCLUDED
#include <pb.h>

#ifdef UENUM
#undef UENUM
#endif

etc
```


## Changes
0.1.0 Initial version, JsonArduino packet format