# InfiniSolarP18

This project provides a library to communicate with InfiniSolar inverters that implement the RS232 protocol version P18.

The aim is to be able to send queries and commands to InfiniSolar inverters from an ESP32 or an Arduino, via an RS232 Serial communication cable.

You will typically need an RS232 to TTL converter between the Serial cable and your microprocessor to appropriately flip/scale the voltages.

## Library limitations

* Not all queries/commands are implemented.
* The lib uses readBytesUntil() which is a blocking function
* The inverter is assumed to be a single module, so parallel id is assumed 0.

# Examples

## Thingsboard

The thingsboard example uploads queried data to a Thingsboard account (whose details must be provided), and also has RPC callbacks for RPC commands defined in the thingsboard account. Hopefully I'll get around to adding the requisite files on the Thingboard side as well.

### Configuring Sensitive defines

Sensitive details like your Wifi SSID/Password and your Thingsboard username/password are read from `infinisolar_p18_esp32_monitor_defs.h`.

Please set the defines inside this file to their appropriate values.

### Example Limitations

* The config/secrets like Wifi password etc. have to be hardcoded