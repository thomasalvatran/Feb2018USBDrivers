# Feb2018USBDrivers
USB Driver for USB Mass Storage
Please see http://www.tovantran.com/blog/?s=usb
and https://www.pjrc.com/tech/8051/ide/fat32.html

klm is kernel linux module for usb stick
mass_storage_write mass_storage_read read and write file into usb
penapp is read write raw data into usb
penapp_write is write raw data into usb
penapp_read is read raw data from usb

<b>Write a file into USB</b>
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

ls /media/usb
hosts.txt  my_data.txt  sdcard_backup_BBB_8GB.img  System Volume Information<br>

You can see my_data.txt 

Plugin USB kernel auto mount when click mount prompt at default for example /media/66CC-9275 <br>
/media/66CC-0275$ ls<br><br>
hosts.txt  my_data.txt  sdcard_backup_BBB_8GB.img  System Volume Information<br>
</code></pre>
__**To run wireshark:**__<br>
sudo modprobe usbmon <br>
gksu wireshark  <br>
then select interface capture for example usbmon3. Here is usb protocol snapshot when ./mass_storage_write
<pre>
From wireshar monitor usbmon3
21	4.443707	host	3.12.1	USBMS	95	SCSI: Write(10) LUN: 0x00 (LBA: 0x00808070, Len: 1)
22	4.444065	3.12.1	host	USB	64	URB_BULK out
23	4.444073	host	3.12.1	USB	576	URB_BULK out
24	4.445949	3.12.1	host	USB	64	URB_BULK out
25	4.445958	host	3.12.2	USB	64	URB_BULK in
26	4.446294	3.12.2	host	USBMS	77	
</pre>

__**RAW write data into sector:**__<br>
The following will write into sector 2 of USB using USB product ID. <br>
handle = libusb_open_device_with_vid_pid(NULL, 0x05dc, 0xa838); <br>
<pre><code>
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


Read back sector 2 of USB (Sector 0 is FAT1 for FAT16, Sector 1 FAT2 for FAT32 which using 2 sectors) so the above code write into sector 2
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
</code></pre>

__Kernel load module: klm directory can read/write sector the same using linux kernel module__<br>
Finally, linux kernel mode /klm <br>
make<br>
pen_driver.ko this linux kernel module<br>
Unplugged USB<br>
sudo rmmod usb_storage<br>
insmod pen_driver.ko  (insert our driver to replace usb_storage so probe_function will work correctly)<br>
Plugged USB stick<br>
check: dmesg  to see if everything ok<br>
sudo ./userapp  following insstruction to read and write into USB <br>
check: dmesg to see if a read or a write is happened
<pre><code>
   dmesg | tail
[21472.003053] Minor obtained: 0
[21472.003066] usbcore: registered new interface driver pen_driver
[21506.735067] pen_write-------
[21506.818493] this is a test
[21512.095124] pen_read--------this is a test
</pre></code>
Note: ./userapp (need sudo otherwise error /dev/pen0 does not exit or lock by another process)<br>


Following the instruction https://www.pjrc.com/tech/8051/ide/fat32.html to read FAT32 File system and <br>
to find out the root directory:<br>
<pre><code>
tovantran@kubuntu-vm:~/Ctest/k_r/kernel_hi11Bulk$ sudo dd if=/dev/sdg bs=512 count=1 skip=32768 |xxd
0000000: 4220 0049 006e 0066 006f 000f 0072 7200  B .I.n.f.o...rr.
0000010: 6d00 6100 7400 6900 6f00 0000 6e00 0000  m.a.t.i.o...n...
0000020: 0153 0079 0073 0074 0065 000f 0072 6d00  .S.y.s.t.e...rm.
0000030: 2000 5600 6f00 6c00 7500 0000 6d00 6500   .V.o.l.u...m.e.
0000040: 5359 5354 454d 7e31 2020 2016 0084 7d60  SYSTEM~1   ...}`
0000050: 484c 484c 0000 8060 484c 0300 0000 0000  HLHL...`HL......
0000060: 484f 5354 5320 2020 5458 5420 18b9 a860  HOSTS   TXT ...`
0000070: 484c 484c 0000 7867 0949 0600 c803 0000  HLHL..xg.I......
0000080: 425f 0042 0042 0042 005f 000f 0030 3800  B_.B.B.B._...08.
0000090: 4700 4200 2e00 6900 6d00 0000 6700 0000  G.B...i.m...g...
00000a0: 0173 0064 0063 0061 0072 000f 0030 6400  .s.d.c.a.r...0d.
00000b0: 5f00 6200 6100 6300 6b00 0000 7500 7000  _.b.a.c.k...u.p.
00000c0: 5344 4341 5244 7e31 494d 4720 0064 7a85  SDCARD~1IMG .dz.
00000d0: 484c 594c 0000 7a85 484c 0800 ffff ffff  HLYL..z.HL......
00000e0: 416d 0079 005f 0064 0061 000f 002b 7400  Am.y._.d.a...+t.
00000f0: 6100 2e00 7400 7800 7400 0000 0000 ffff  a...t.x.t.......
0000100: 4d59 5f44 4154 4120 5458 5420 0000 9766  MY_DATA TXT ...f
0000110: 584c 584c 0800 9766 584c 0900 9d01 0000  XLXL...fXL......
0000120: 4168 0065 006c 006c 006f 000f 00dd 2e00  Ah.e.l.l.o......
0000130: 6300 0000 ffff ffff ffff 0000 ffff ffff  c...............
0000140: 4845 4c4c 4f20 2020 4320 2020 0064 8540  HELLO   C   .d.@
0000150: 594c 594c 0800 8540 594c 0a00 3f00 0000  YLYL...@YL..?..
</code></pre>

