# sonic_pad_os
It is recommended to use Ubuntu 18.04 to compile sonic_pad_os
1. Start ***Terminal***
2. run command ***cd ~***
3. run command ***git clone https://github.com/CalDymos/sonic_pad_os.git*** (download the code)
4. run command ***cd sonic_pad_os***
5. run command ***sudo ./relatedPacks.sh*** (installs all related Package dependencies)
6. run command ***sudo ./dlPackages.sh*** (Downloads the dl packages and saves them in the directory sonic_pad_os/dl) 
5. Enter the root directory of sonic_pad_os and execute the script ***./scripts/prepare.sh***
4. Compile steps:
   - ***source build/envsetup.sh***
   - ***lunch 6***
   - ***make -j2 && pack***
   - ***swupdate_pack_swu -ab***
5. U flash drive upgrade method: Copy the config.ini and t800-sonic_lcd-ab_1.0.6.48.57.swu files in the sonic_pad_os/out/r818-sonic_lcd/ directory to the root directory of the U flash drive. The firmware version number must be greater than the current version number of the device, otherwise the upgrade box will not pop up.
6. Computer upgrade method: The image path generated after compilation is sonic_pad_os/out/r818-sonic_lcd/t800-sonic_lcd_uart0.img, please refer to the link for the burning tool and upgrade method: https://github.com/CrealityOfficial/Creality_Sonic_Pad_Firmware
