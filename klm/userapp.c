////home/tovantran/Ctest/k_r/kernel_hi8usbdriver/kernel_modules/test_char_device/userapp.c --> 2015-04-03 by ./.tv  owner: tovantran
//youtube.com SolidusCode
//http://stackoverflow.com/questions/1247989/how-do-you-allow-spaces-to-be-entered-using-scanf
/* user application which reads and writes to a device */
/* 
   Usage:
   % make
   % ./userapp
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define DEVICE "/dev/pen0"
//#define DEVICE "/dev/sdc1"
// #define DEVICE "/dev/pan0"
//#define DEVICE "/dev/mem"
//#define DEVICE "/dev/solidusdevice"

int main(int argc, char *argv[])
{
  int i, fd;
  char ch, write_buf[100], read_buf[100];

  fd = open(DEVICE, O_RDWR);	/* open for read/write */

  if(fd == -1){
    printf("file %s either doesnot exit, or locked by another process\n", DEVICE);
    exit(-1);
  }
  printf("r - read from device,\nw- write from device\nEnter command: ");
  scanf("%c", &ch);
/*  ch = getchar();*/
  switch (ch){    
  case 'w':
 		printf ("enter data: ");
		fgets(write_buf,100, stdin);        // read AT echo
/*    scanf("%s", write_buf);*/
		scanf("%[^\n]", write_buf);
    write(fd, write_buf, sizeof(write_buf)); //user to kernel writebuf ->fd
    break;

  case 'r':
    read(fd, read_buf, sizeof(read_buf));
//    read(fd, read_buf, sizeof(read_buf));
    printf ("device: %s\n", read_buf);
    break;

  default:
    printf ("command not recognized\n");
    break;
  }

  close(fd);
  return 0;
}

