/*
 * ledcontrol.c
 *
 *  Created on: Sept 13, 2011
 *  Author: Jan Axelson
 *  Demonstrates reading from a flash drive.
 *  Requires a file named config.txt on an attached flash drive.
 *  The file may contain any of the following commands:
 *  led1on, led1off, led2on, led2off
 *  The program reads from the file and turns the usr0 and usr1 LEDs on or off
 *  according to the commands found.
 *  Run as root to enable controlling the LEDs.
 *
 *  This file, led_control.h, and led_control.c are at www.Lvr.com/beagleboard.htm
 */

// To gain access to the LEDs, run the application with administrative privileges 
// (From the file's directory, use "sudo ./usb_serial_port".)
// Or set up a udev rule as described in led_control.c.

#include <mntent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include "led_control.h"

void get_mount_point(char* drive_mount_point, size_t max_length);
void led_control(int led_number, int led_state);
void led_initialize();

int main()
{
	#define MAX_READ_LENGTH 100
	#define MAX_PATH_LENGTH 255

	char buffer[MAX_READ_LENGTH + 1] = {0};
	int chars_read = 0;
	char *file_name_and_path = "my_data.txt";
	char mount_point_and_file[MAX_PATH_LENGTH] = {0};
	int drive_mount_point_max_length = MAX_PATH_LENGTH -strlen(file_name_and_path) - 1;
	char drive_mount_point[drive_mount_point_max_length];

	memset(drive_mount_point, 0, drive_mount_point_max_length);
	//led_initialize();
	FILE *fp = setmntent("/proc/mounts","r");
	get_mount_point(drive_mount_point, sizeof(drive_mount_point));
	if (drive_mount_point != NULL)
	{
		// A drive is available.
		// Append the file name to the mount point.
		// Example: file_name_and_path = "/media/Lexar/config.txt"
		
		strncpy(mount_point_and_file, drive_mount_point, sizeof(drive_mount_point));
		strcat(mount_point_and_file, "/");
		strcat(mount_point_and_file, file_name_and_path);
		fp = fopen(mount_point_and_file, "r");
		
		if (fp != NULL)
		{
			chars_read = fread (buffer, 1, MAX_READ_LENGTH, fp);
			fclose(fp);

			if (chars_read)
			{
				// Look for defined commands and take appropriate actions.
				printf("data read from usb: %s\n", buffer);
				if (strstr(buffer, "led1on") != NULL)
				{
					// led_control(0, 1);
					printf("led1on\n");
				}
				if (strstr(buffer, "led1off") != NULL)
				{
					// led_control(0, 0);
					printf("led1off\n");
				}
				if (strstr(buffer, "led2on") != NULL)
				{
					// led_control(1, 1);
					printf("led2on\n");
				}
				if (strstr(buffer, "led2off") != NULL)
				{
					// led_control(1, 0);
					printf("led2off\n");
				}
			}
			else
			{
				printf("Zero bytes read from the file.\n");
			}
		}
		else
		{
			printf("Error opening the file.\n");
		}
	}
  return 0;
}

/*
 * Detects if a drive is mounted.
 * Searches /proc/mounts for an entry containing "dev/sda".
 * drive_mount_point - char array that will hold the obtained drive mount point or null.
 * max_length - sizeof(drive_mount_point)
 * Returns 1 if found, 0 if not found.
 */
 
void get_mount_point(char *drive_mount_point, size_t max_length)
{
	char desired_mount_point[] = "dev/sdb";
	struct mntent *mount_entry;
	FILE *fp = setmntent("/proc/mounts","r");

	memset(drive_mount_point, 0, max_length);

	do
	{
		// Read the next entry in the mntent structure.
				
		mount_entry = getmntent(fp);
		if (mount_entry !=  NULL)
		{
			// The first drive is at /dev/sda.
			// Look for a mount point containing "dev/sda".
			// Can be /dev/sda, /dev/sda1, etc.
			
			if (strstr(mount_entry->mnt_fsname, desired_mount_point)
					!= NULL)
			{
				if (strlen(mount_entry->mnt_dir) < max_length)
				{
					strncpy(drive_mount_point, mount_entry -> mnt_dir,
						 max_length);
					printf("%s mounted on %s\n",mount_entry -> mnt_fsname,
						drive_mount_point);
				}
				else
				{
					printf("Error: mount point's path is too long.\n");
				}
			}
		}
	}
	// Stop looking on finding the drive or reaching the end of the structure.
		
	while ((drive_mount_point[0] == 0) && (mount_entry != NULL));
}
