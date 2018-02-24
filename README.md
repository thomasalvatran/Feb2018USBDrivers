# Feb2018USBDrivers
USB Driver for USB Mass Storage
Please see http://www.tovantran.com/blog/?s=usb
<pre><code>
klm is kernel linux module for usb stick
mass_storage_write mass_storage_read read and write file into usb
penapp is read write raw data into usb
penapp_write is write raw data into usb
penapp_read is read raw data from usb

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
gksu wireshark 
then select interface capture for example usbmon3. Here is usb protocol snapshot when ./mass_storage_write

From wireshar monitor usbmon3
21	4.443707	host	3.12.1	USBMS	95	SCSI: Write(10) LUN: 0x00 (LBA: 0x00808070, Len: 1)
22	4.444065	3.12.1	host	USB	64	URB_BULK out
23	4.444073	host	3.12.1	USB	576	URB_BULK out
24	4.445949	3.12.1	host	USB	64	URB_BULK out
25	4.445958	host	3.12.2	USB	64	URB_BULK in
26	4.446294	3.12.2	host	USBMS	77	

RAW data into sector 2 of USB using USB product ID.
handle = libusb_open_device_with_vid_pid(NULL, 0x05dc, 0xa838); 

- penapp 
sudo ./penapp
[sudo] password for tovantran: 

Device Opened
Claimed Interface
To be sent : abcdefgh
Xfer returned with 0 -> LIBUSB_SUCCESS
Sent CDB 31 bytes with string: abcdefgh

Xfer returned with 0 -> LIBUSB_SUCCESS
Received STB 13 bytes with string: USBSefgh
string2 = USBSefgh
tovantran@kubuntu-vm:~/Ctest/k_r/kernel_hi11Bulk$

- penapp_write penapp_read
tovantran@kubuntu-vm:~/Ctest/k_r/kernel_hi11Bulk$ sudo ./penapp_read (blank)

Device Opened
Kernel Driver Active
Kernel Driver Detached!
Claimed Interfacecdb_len ===========10
   sent 10 CDB bytes

Xfer returned with 0 -> LIBUSB_SUCCESS
Received STB 512 bytes with string: 
string2 = 00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000055ffffffaa
tovantran@kubuntu-vm:~/Ctest/k_r/kernel_hi11Bulk$ sudo ./penapp_write (write)

Device Opened
Claimed Interfacecdb_len ===========10
   sent 10 CDB bytes

Xfer returned with 0 -> LIBUSB_SUCCESS
Received STB 13 bytes with string: USBSU
string2 = 555342535510000000
tovantran@kubuntu-vm:~/Ctest/k_r/kernel_hi11Bulk$ sudo ./penapp_read (read back)

Device Opened
Claimed Interfacecdb_len ===========10
   sent 10 CDB bytes

Xfer returned with 0 -> LIBUSB_SUCCESS
Received STB 512 bytes with string: abcdefghxxxxtest write into usb mass storage using libusb
string2 = 6162636465666768787878787465737420777269746520696e746f20757362206d6173732073746f72616765207573696e67206c696275736200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000


Read back sector 2 of USB (Sector is FAT Sector 1 FAT2 if FAT32 which using 2 sectors) be safe the above code write into sector 2.
To verify using dd, remove usb and plug usb check dmesg for which device for example /dev/sdg. We can see what we wrote into it.

kernel_hi11Bulk$ sudo dd if=/dev/sdh bs=512 count=1 skip=2 |xxd
0000000: 6162 6364 6566 6768 7878 7878 7465 7374  abcdefghxxxxtest
0000010: 2077 7269 7465 2069 6e74 6f20 7573 6220   write into usb 
0000020: 6d61 7373 2073 746f 7261 6765 2075 7369  mass storage usi
0000030: 6e67 206c 6962 7573 6200 0000 0000 0000  ng libusb.......
0000040: 0000 0000 0000 0000 0000 0000 0000 0000  ................
0000050: 0000 0000 0000 0000 0000 0000 0000 0000  ................
0000060: 0000 0000 0000 0000 0000 0000 0000 0000  ................
0000070: 0000 0000 0000 0000 0000 0000 0000 0000  ................
0000080: 0000 0000 0000 0000 0000 0000 0000 0000  ................
0000090: 0000 0000 0000 0000 0000 0000 0000 0000  ................
00000a0: 0000 0000 0000 0000 0000 0000 0000 0000  ................
00000b0: 0000 0000 0000 0000 0000 0000 0000 0000  ................
00000c0: 0000 0000 0000 0000 0000 0000 0000 0000  ................
00000d0: 0000 0000 0000 0000 0000 0000 0000 0000  ................
00000e0: 0000 0000 0000 0000 0000 0000 0000 0000  ................
00000f0: 0000 0000 0000 0000 0000 0000 0000 0000  ................
0000100: 0000 0000 0000 0000 0000 0000 0000 0000  ................
0000110: 0000 0000 0000 0000 0000 0000 0000 0000  ................
0000120: 0000 0000 0000 0000 0000 0000 0000 0000  ................
0000130: 0000 0000 0000 0000 0000 0000 0000 0000  ................
0000140: 0000 0000 0000 0000 0000 0000 0000 0000  ................
0000150: 0000 0000 0000 0000 0000 0000 0000 0000  ................
0000160: 0000 0000 0000 0000 0000 0000 0000 0000  ................
0000170: 0000 0000 0000 0000 0000 0000 0000 0000  ................
0000180: 0000 0000 0000 0000 0000 0000 0000 0000  ................
0000190: 0000 0000 0000 0000 0000 0000 0000 0000  ................
00001a0: 0000 0000 0000 0000 0000 0000 0000 0000  ................
00001b0: 0000 0000 0000 0000 0000 0000 0000 0000  ................
00001c0: 0000 0000 0000 0000 0000 0000 0000 0000  ................
00001d0: 0000 0000 0000 0000 0000 0000 0000 0000  ................
00001e0: 0000 0000 0000 0000 0000 0000 0000 0000  ................
00001f0: 0000 0000 0000 0000 0000 0000 0000 0000  ................
1+0 records in
1+0 records out
512 bytes (512 B) copied, 0.000533462 s, 960 kB/s


</code>


