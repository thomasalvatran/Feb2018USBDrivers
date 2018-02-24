# Feb2018USBDrivers
USB Driver for USB Mass Storage
Please see http://www.tovantran.com/blog/?s=usb
<pre><code>
Plug USB stick check dmesg 
[171267.445656] sd 36:0:0:0: Attached scsi generic sg6 type 0 <br>
[171267.447065] sd 36:0:0:0: [sdf] 31285248 512-byte logical blocks: (16.0 GB/14.9 GiB)<br>
[171267.453994] sd 36:0:0:0: [sdf] Write Protect is off<br>
[171267.453996] sd 36:0:0:0: [sdf] Mode Sense: 43 00 00 00<br>
[171267.465916] sd 36:0:0:0: [sdf] Write cache: enabled, read cache: enabled, doesn't support DPO or FUA<br>
[171267.492901]  sdf:<br>
[171267.519182] sd 36:0:0:0: [sdf] Attached SCSI removable disk<br>
[172011.617460] usb 3-2.1: USB disconnect, device number 11<br>
[172011.622093] sd 36:0:0:0: [sdf] Synchronizing SCSI cache<br>

Change mass_storage_write.c and mass_storage_read.c accordingly /dev/sdf and recompile<br>
Data to write:<br>
char *write_buffer = "Mass storage write test. led1on, led1off, led2on, led2off\r\n";

Run:<br>
~/Ctest/k_r/kernel_hi11Bulk$ sudo ./mass_storage_write<br>
/dev/sdf mounted on /media/66CC-0275<br>
Data written to the file<br>
<br>
~/Ctest/k_r/kernel_hi11Bulk$ sudo ./mass_storage_read<br>
/dev/sdf mounted on /media/66CC-0275<br>
data read from usb: Mass storage write test. led1on, led1off, led2on, led2off<br>

led1on
led1off
led2on
led2off
~/Ctest/k_r/kernel_hi11Bulk$ <br>

mkdir /media/usb<br>
sudo mount /dev/sdf /media/usb<br>

ls /media/usb<br>
hosts.txt  my_data.txt  sdcard_backup_BBB_8GB.img  System Volume Information<br>
<br>
You can see my_data.txt <br>
<br>
Plugin USB kernel auto mount when click mount prompt at default for example /media/66CC-9275 <br>
/media/66CC-0275$ ls<br><br>
hosts.txt  my_data.txt  sdcard_backup_BBB_8GB.img  System Volume Information<br>

</code>


