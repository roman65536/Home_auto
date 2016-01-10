# Home_auto 
This is my home automatization mini Node, base on attiny84.
Communication is based via Softwart Uart over RS422 kinda bus.
Every node is connected to the Network and have to have his unique Address.
Since every node will polled once in a while, only one Node is transmitting at the time, based on his address.
Protocol is very simple, see main.c
So far it has been tested with NTC Resistor, LDR, PIR Sensor and also with sth71.

As a Server an arduino can be used. I used it with eth and mqtt.

