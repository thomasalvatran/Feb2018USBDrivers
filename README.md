# Feb2018USBDrivers
USB Driver for USB Mass Storage
Please see http://www.tovantran.com/blog/?s=usb

Plug USB stick check dmesg 
[171267.445656] sd 36:0:0:0: Attached scsi generic sg6 type 0
[171267.447065] sd 36:0:0:0: [sdf] 31285248 512-byte logical blocks: (16.0 GB/14.9 GiB)
[171267.453994] sd 36:0:0:0: [sdf] Write Protect is off
[171267.453996] sd 36:0:0:0: [sdf] Mode Sense: 43 00 00 00
[171267.465916] sd 36:0:0:0: [sdf] Write cache: enabled, read cache: enabled, doesn't support DPO or FUA
[171267.492901]  sdf:
[171267.519182] sd 36:0:0:0: [sdf] Attached SCSI removable disk
[172011.617460] usb 3-2.1: USB disconnect, device number 11
[172011.622093] sd 36:0:0:0: [sdf] Synchronizing SCSI cache

Change mass_storage_write.c and mass_storage_read.c accordingly /dev/sdf and recompile
Data to write:
char *write_buffer = "Mass storage write test. led1on, led1off, led2on, led2off\r\n";

Run:
~/Ctest/k_r/kernel_hi11Bulk$ sudo ./mass_storage_write
/dev/sdf mounted on /media/66CC-0275
Data written to the file

tovantran@kubuntu-vm:~/Ctest/k_r/kernel_hi11Bulk$ sudo ./mass_storage_read
/dev/sdf mounted on /media/66CC-0275
data read from usb: Mass storage write test. led1on, led1off, led2on, led2off

led1on
led1off
led2on
led2off
tovantran@kubuntu-vm:~/Ctest/k_r/kernel_hi11Bulk$ 

mkdir /media/usb
sudo mount /dev/sdf /media/usb

ls /media/usb
hosts.txt  my_data.txt  sdcard_backup_BBB_8GB.img  System Volume Information

You can see my_data.txt 

Plugin USB kernel auto mount when click mount prompt at default for example /media/66CC-9275 
tovantran@kubuntu-vm:/media/66CC-0275$ ls
hosts.txt  my_data.txt  sdcard_backup_BBB_8GB.img  System Volume Information




