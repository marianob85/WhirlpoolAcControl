# Whirlpool AirCondition AMC 996 YJ1B Tester    
 
## RaspberryPi prepare
### Packages
apt-get install lirc
#### Add the following lines to /etc/modules file
* lirc_dev
* lirc_rpi gpio_in_pin=18 gpio_out_pin=17
#### Update the following line in /boot/config.txt
* dtoverlay=lirc-rpi,gpio_in_pin=18,gpio_out_pin=17
#### Update the following lines in /etc/lirc/lirc_options.conf
* driver = default
* device = /dev/lirc0