#!/bin/bash
echo 58 > /sys/class/gpio/export
echo 59 > /sys/class/gpio/export
echo rising > /sys/class/gpio/gpio58_pi10/edge
echo rising > /sys/class/gpio/gpio59_pi11/edge
echo in > /sys/class/gpio/gpio58_pi10/direction
echo in > /sys/class/gpio/gpio59_pi11/direction
