# Feb2018USBDrivers
USB Driver for USB Mass Storage
Please see http://www.tovantran.com/blog/?s=usb
<pre><code>
Plug USB stick check dmesg 
[171267.445656] sd 36:0:0:0: Attached scsi generic sg6 type 0 
[171267.447065] sd 36:0:0:0: [sdf] 31285248 512-byte logical blocks: (16.0 GB/14.9 GiB)
[171267.453994] sd 36:0:0:0: [sdf] Write Protect is off
[171267.453996] sd 36:0:0:0: [sdf] Mode Sense: 43 00 00 00
[171267.465916] sd 36:0:0:0: [sdf] Write cache: enabled, read cache: enabled, doesn't support DPO or FUA
[171267.492901]  sdf:<br>
[171267.519182] sd 36:0:0:0: [sdf] Attached SCSI removable disk
[172011.617460] usb 3-2.1: USB disconnect, device number 11
[172011.622093] sd 36:0:0:0: [sdf] Synchronizing SCSI cache

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

To run wireshark:
sudo modprobe usbmon
gksu wireshark then select interface capture for example usbmon3
From wireshar monitor usbmon3
21	4.443707	host	3.12.1	USBMS	95	SCSI: Write(10) LUN: 0x00 (LBA: 0x00808070, Len: 1)
22	4.444065	3.12.1	host	USB	64	URB_BULK out
23	4.444073	host	3.12.1	USB	576	URB_BULK out
24	4.445949	3.12.1	host	USB	64	URB_BULK out
25	4.445958	host	3.12.2	USB	64	URB_BULK in
26	4.446294	3.12.2	host	USBMS	77	


</code>


