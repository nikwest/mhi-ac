[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)  [![Gitter](https://badges.gitter.im/cesanta/mongoose-os.svg)](https://gitter.im/cesanta/mongoose-os?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge)

# MHI-AC lib for Mongoose OS

***Mitsubishi Heavy Industries*** AC (air condition unit) control by SPI using amazing IoT platform - ***Mongoose OS***

Inspired by (https://github.com/mongoose-os-libs/mel-ac) and (https://github.com/absalom-muc/MHI-AC-Ctrl)

## Hardware



## Compatability

TBD

## Mounting

TBD

## Software

### Comfig chema
TBD

```javascript
"mel_ac": {
  "enable": true,         // Enable MEL-AC library
  "uart_no": 0,           // UART number
  "uart_baud_rate": 2400, // Do not change this, unless required
  "period_ms": 250,       // Packets handler timer, ms
  "rpc_enable": true      // Enable MEL-AC RPC handlers 
}
```

### RPC
TBD
If RPC is present in your project, MEL-AC will add it's command handlers: `MEL-AC.GetParams` and `MEL-AC.SetParams` 

```javascript
$ mos --port mqtt://192.168.1.9/esp32_585E14 call RPC.list
[
    "MEL-AC.SetParams",
    "MEL-AC.GetParams",
    "Sys.SetDebug",
    "Sys.GetInfo",
    "Sys.Reboot",
    "RPC.Ping",
    "RPC.Describe",
    "RPC.List"
]
```
#### Read current params from HVAC

##### WebSockets
TBD
```javascript
$ mos call --port ws://192.168.1.216/rpc MEL-AC.GetParams
{
"connected": true,
"power": 0,
"mode": 3,
"setpoint": 21.0,
"fan": 3,
"vane_vert": 0,
"vane_horiz": 3,
"isee": true,
"operating": false,
"room": 25.0
}
```
##### HTTP
TBD
```javascript
http://192.168.1.216/rpc/MEL-AC.GetParams
{"connected": true, "power": 0, "mode": 1, "setpoint": 20.0, "fan": 2, "vane_vert": 0, "vane_horiz": 3, "isee": true, "operating": false, "room": 22.0}
```

##### MQTT
TBD
```javascript
$ mos --port mqtt://192.168.1.9/esp32_585E14 call MEL-AC.GetParams 
{
"connected": true,
"power": 0,
"mode": 3,
"setpoint": 21.0,
"fan": 3,
"vane_vert": 0,
"vane_horiz": 3,
"isee": true,
"operating": false,
"room": 25.0
}
```

#### Set new params to HVAC

##### WebSockets

```javascript
$ mos call --port ws://192.168.1.216/rpc MEL-AC.SetParams '{"mode":1,"fan":2,"setpoint":20}'
{
  "success": true,
  "power": 0,
  "mode": 1,
  "setpoint": 20.0,
  "fan": 2,
  "vane_vert": 0,
  "vane_horiz": 3
}
```
##### HTTP

```javascript
$ curl -d '{"mode":1,"fan":2,"setpoint":20}' 192.168.1.216/rpc/MEL-AC.SetParams
{"success": true, "power": 0, "mode": 1, "setpoint": 20.0, "fan": 2, "vane_vert": 0, "vane_horiz": 3} 
```

##### MQTT

```javascript
$ mos --port mqtt://192.168.1.9/esp32_585E14 call MEL-AC.SetParams '{"power":1,"mode":1,"fan":2,"setpoint":20}'
{
  "success": true,
  "power": 1,
  "mode": 1,
  "setpoint": 20.0,
  "fan": 2,
  "vane_vert": 0,
  "vane_horiz": 3
}
```

### Primitives

The library init makes a timer is set up at `mel_ac.period_ms` milliseconds intervals to handle the device.
The handler quering the HVAC params in a loop and sends new params when there are changes happen.

#### Events

Library triggering events as follows:
*   `MGOS_MEL_AC_EV_INITIALIZED`: when the service initialized successfully
*   `MGOS_MEL_AC_EV_CONNECTED`: when `connected` state changed. New state goes to `*ev_data`
*   `MGOS_MEL_AC_EV_PACKET_WRITE`: after packet sent to HVAC
*   `MGOS_MEL_AC_EV_PACKET_READ`: after new packet has been read from HVAC
*   `MGOS_MEL_AC_EV_PACKET_READ_ERROR`: when there was a problem in packet from HVAC
*   `MGOS_MEL_AC_EV_PARAMS_CHANGED`: when new changed params loaded from HVAC
*   `MGOS_MEL_AC_EV_ROOMTEMP_CHANGED`: when room temperature change received from HVAC
*   `MGOS_MEL_AC_EV_TIMERS_CHANGED`: when timers changed on HVAC
*   `MGOS_MEL_AC_EV_OPERATING_CHANGED`: when operating state changed on HVAC
*   `MGOS_MEL_AC_EV_PARAMS_SET`: when new params successfully set to HVAC
*   `MGOS_MEL_AC_EV_TIMER`: before every service timer handler running
*   `MGOS_MEL_AC_EV_CONNECT_ERROR`: when handshake packet returned error

#### To read params from HVAC

```c
void mgos_mel_ac_get_params(struct mgos_mel_ac_params *params);
```
#### To write params to HVAC

```c
bool mgos_mel_ac_set_power(enum mgos_mel_ac_param_power power);
bool mgos_mel_ac_set_mode(enum mgos_mel_ac_param_mode mode);
bool mgos_mel_ac_set_setpoint(float setpoint);
bool mgos_mel_ac_set_ext_temp(float temp);
bool mgos_mel_ac_set_fan(enum mgos_mel_ac_param_fan fan);
bool mgos_mel_ac_set_vane_vert(enum mgos_mel_ac_param_vane_vert vane_vert);
bool mgos_mel_ac_set_vane_horiz(enum mgos_mel_ac_param_vane_horiz vane_horiz);
void mgos_mel_ac_set_params(struct mgos_mel_ac_params *params);
```

Setters return `false` in case of invalid argument value

It's currently tested on ***ESP8266*** and ***ESP32*** platforms

For a complete demonstration of the driver, look at this [Mongoose App](https://github.com/mongoose-os-apps/mel-ac-demo)

Larger project is [mel-ac-homekit](https://github.com/mongoose-os-apps/mel-ac-homekit)

## Extra

A sample case 3D design for WiFi-NodeM:

![](https://github.com/mongoose-os-libs/mel-ac/blob/master/docs/wifi-nodem-case.gif)

Ready to print STL images available for [top](https://github.com/mongoose-os-libs/mel-ac/blob/master/docs/nodem-top-case-v24.stl) and [bottom](https://github.com/mongoose-os-libs/mel-ac/blob/master/docs/nodem-bottom-case-v24.stl) part
