# Kingswood Environment Monitor environment sensor firmware

This is the firmware for a sensor for measuring temperature, pressure, humidity, light
and atmospheric pressure and, as a LoRa client, relaying it over a LoRa link to a LoRa server.

## Protobuf

To regenerate the schema, run this command:

```
mkdir -p lib/proto
python .pio/libdeps/feather32u4/Nanopb/generator/nanopb_generator.py -I proto -D lib/proto proto/packet.proto
```

## Changes
0.1.0 Initial version, JsonArduino packet format