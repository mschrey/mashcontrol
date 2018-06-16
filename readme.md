## Mashcontrol ##

# Hardware Connections #
1) Remote controlled outlets (e.g. https://www.pollin.de/p/funksteckdosen-set-mit-3-steckdosen-550666)
2) 433MHz radio transmitter module (e.g. https://www.amazon.de/gp/product/B00OLI93IC/ref=oh_aui_detailpage_o05_s00?ie=UTF8&psc=1)
3) waterproof one wire temperature sensors (DS18S20, e.g. https://de.aliexpress.com/item/Direkten-wasserdicht-DS18B20-digitaler-temperatursensor-sonde-eine-gro-e-anzahl-von-original-spot/32675444739.html?spm=a2g0x.search0104.3.2.418b3d20hlIQLC&ws_ab_test=searchweb0_0%2Csearchweb201602_3_10320_10152_10321_10065_10151_10344_10068_5723115_5722815_10342_10547_10343_10322_10340_10341_10548_5722915_5722615_10193_10696_10194_10084_10083_10618_10304_10307_10820_10821_10302_5722715_10843_10059_100031_10319_10103_10624_10623_10622_5722515_10621_10620%2Csearchweb201603_30%2CppcSwitch_5&algo_expid=17f34d38-7b86-4514-994f-47daa466bb6e-0&algo_pvid=17f34d38-7b86-4514-994f-47daa466bb6e&transAbTest=ae803_1&priceBeautifyAB=0)

# Software Dependencies #
sudo apt-get install build-essential gcc git wiringpi apache2 php 

# Installation #
git clone https://github.com/mschrey/mashcontrol.git
gcc mashcontrol.c -o mashcontrol -lwiringPi


# Information #
As of version v0.5, it is not necessary anymore to define the mash steps in the source code. Instead, a php web front end has been developed. Here, the mash steps can be easily defined. By clicking "start mash control", a mash step file (.msf) is created. Afterwards the mash control c binary is started with the name of the mash step file as the parameter. 