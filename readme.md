# Mashcontrol #

Mashcontrol simplifies your home beer brewing. During mashing, a set of predefined mash steps have to be reached and then held. Mashcontrol can control the mash tun temperature and can run these predefined mash temperature steps. 

Mashcontrol is run on a Raspberry Pi (any model). It reads the mash tun temperature using one wire temperature sensors (DS18B20 oder DS18S20). It controls the mash tun temperature using remote controlled outlets. Turn on the heating element of your mash tun all the way and plug it into a remote controlled outlet. 

## Hardware Connections ##
1) Remote controlled outlets (e.g. [this one](https://www.pollin.de/p/funksteckdosen-set-mit-3-steckdosen-550666))
2) 433MHz radio transmitter module (e.g. [this one](https://www.amazon.de/gp/product/B00OLI93IC/ref=oh_aui_detailpage_o05_s00?ie=UTF8&psc=1))
3) waterproof one wire temperature sensors (DS18S20, e.g. [here](https://de.aliexpress.com/item/Direkten-wasserdicht-DS18B20-digitaler-temperatursensor-sonde-eine-gro-e-anzahl-von-original-spot/32675444739.html?spm=a2g0x.search0104.3.2.418b3d20hlIQLC&ws_ab_test=searchweb0_0%2Csearchweb201602_3_10320_10152_10321_10065_10151_10344_10068_5723115_5722815_10342_10547_10343_10322_10340_10341_10548_5722915_5722615_10193_10696_10194_10084_10083_10618_10304_10307_10820_10821_10302_5722715_10843_10059_100031_10319_10103_10624_10623_10622_5722515_10621_10620%2Csearchweb201603_30%2CppcSwitch_5&algo_expid=17f34d38-7b86-4514-994f-47daa466bb6e-0&algo_pvid=17f34d38-7b86-4514-994f-47daa466bb6e&transAbTest=ae803_1&priceBeautifyAB=0))

 * Connect VCC of temperature sensor to Raspberry Pin 1 (3.3V)
 * Gonnect Data of temperature sensor to Raspberry Pin 7 (GPIO 4)
 * Connect GND of temperature sensor to Raspberry Pin 6 (GND)
 * Connect 4,7kOhm resistor between DS18S20 VCC and DS18S20 Data
 * Connect VCC of radio transmitter to Raspberry Pin 4 (5V)
 * Connect DATA of radio transmitter to Raspberry Pin 11 (GPIO 17)
 * Connecto GND of radio transmitter to Raspberry Pin 6 (GND)

## Software Dependencies ##
``sudo apt-get install build-essential gcc git wiringpi apache2 php php-gd``

## Installation ##
```
git clone https://github.com/mschrey/mashcontrol.git
git clone https://github.com/xkonni/raspberry-remote
cd raspberry-remote
make send
cd ../mashcontrol
make
cp mashcontrol_frontend/* /var/www/html/
cd ..
sudo chmod 775 mashcontrol
sudo chmod 775 mashcontrol/mashcontrol
sudo usermod -a -G pi www-data
```
manually change path to temperature sensor device file
add `dtoverlay=w1-gpio,gpiopin=4` to `/boot/config.txt`
```
cd mashcontrol
make
```


## Information ##
As of version v0.5, it is not necessary anymore to define the mash steps in the source code. Instead, a php web front end has been developed. Here, the mash steps can be easily defined. By clicking "start Mashcontrol", a mash step file (.msf) is created. Afterwards the Mashcontrol binary is started with the name of the mash step file as the parameter. 

Use the Mashcontrol web frontend as follows:
1. Direct your browser to <ip_of_RaspberryPi>/mashcontrol.php
2. Enter your desired recipe name at "Recipe Name".
3. Activate and configure the mash steps as needed. 
4. Finally click "start Mashcontrol"

The binary is started and some status information is output. Two times you have to interact with the UI:
  * When the water temperature reached the mashin temperature, the temperature is kept until you added all the grist and acknowledge this by clicking "Continue"
  * When the mashout time is reached, the mashout temperature is kept (usually 78°C) until you click "Continue". Afterwards the binary terminates.

To "install" the mashcontrol PHP web frontend, 
 * make sure you have apache2 with php and GD installed and running. 
 * copy mashcontrol_frontend/* to /var/www/html
 * chmod 775 /home/pi/mashcontrol
 * sudo usermod -aG pi www-data
 
## Outlook ##
  * Control heating element of mash tun using PWM (in preparation)
  * Read mash steps from maischemalzundmehr.de json files (in preparation)
  * Include RCSwitch.cpp in mashcontrol.c to directly control outlets without having to start `send` binary.
  * Beautify web front end, possibly including live plotting of temperature. 
  * Automatically detect temperature sensor (no more hardcoding)

## Temperature sensors ##
Temperature sensors are detected automatically. Mashcontrol requires only a single sensor. If only a single sensor is connected to the Raspberry Pi, this sensor is used. If several sensors are connected, the sensor with label 'watertight' is used. For this second step, the file 'tempsensorlabels.txt' is needed with the correct sensor IDs. It is planned to do some code reuse with https://github.com/mschrey/fermentcontrol where several temperature sensors are needed.

## Troubleshooting ##
 * Sensors cannot be found  
Check `/sys/bus/w1/devices` if temperature sensor is properly recognized.  
`cat /sys/bus/w1/devices/10-000802bf634d/w1_slave` can be used to check for proper temperature data
