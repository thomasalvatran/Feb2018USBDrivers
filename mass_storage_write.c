/*  mass_storage_write.c
 *
 *  Created on: Sept 13, 2011
 *  	Author: Jan Axelson
 *
 *      Demonstrates how to write data to a file on a drive.
 *      Looks for a mount point on /dev/sda.
 *      Appends data to the file mydata.txt. Creates the file if it doesn't exist.
 *      Checks to be sure the drive is present before writing.
 *      This file is at www.Lvr.com/beagleboard.htm
 */

#include <mntent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int drive_available(char* drive_mount_point);
void get_mount_point(char *drive_mount_point, size_t max_length);

int main()
{
	#define MAX_PATH_LENGTH 255

	char *file_name_and_path = "my_data.txt";
	FILE *fp = NULL;
	char mount_point_and_file[MAX_PATH_LENGTH] = {0};
	int drive_mount_point_max_length = MAX_PATH_LENGTH -strlen(file_name_and_path) - 1;

	char drive_mount_point[drive_mount_point_max_length];

	memset(drive_mount_point, 0, drive_mount_point_max_length);

	get_mount_point(drive_mount_point, sizeof(drive_mount_point));

	if (drive_mount_point != NULL)
	{
		// A drive is available.
		// Append the file name to the mount point.
		// Example: mount_point_and_file = "/media/Lexar/mydata.txt"
		
		strncpy(mount_point_and_file, drive_mount_point, sizeof(drive_mount_point));
		strcat(mount_point_and_file, "/");
		strcat(mount_point_and_file, file_name_and_path);

		// The text to write to the file.
		
		char *write_buffer = "Mass storage write test. led1on, led1off, led2on, led2off\r\n";

		if (drive_available(drive_mount_point))
		{
			// The drive is still attached.
						
			fp = fopen(mount_point_and_file, "a");

			if (fp != NULL)
			{
				if (fwrite(write_buffer, strlen(write_buffer), 1, fp) > 0)
				{
					fclose(fp);
					
					// Call sync to write the data without delay.
									
					sync();
					printf("Data written to the file.\n");
				}
				else
				{
					printf("Write failed. Zero bytes written to the file.\n");
				}
			}
			else
			{
				printf("Write failed. Error opening the file.\n");
			}
		}
		else
		{
			printf("Write failed. Drive attached?\n");
		}
	}
	else
	{
		printf("Write failed. Drive attached?\n");
	}

  return 0;
}

// Detects if a drive is mounted.
// Searches /proc/mounts for a drive mount point previously obtained.
// drive_mount_point - a previously obtained drive mount point.
// Returns 1 if found, 0 if not found.

int drive_available(char *drive_mount_point)
{
	int drive_available = 0;
	struct mntent *mount_entry;

	FILE *fp = setmntent("/proc/mounts","r");

	do
	{
		// Read the next entry in the mntent structure.
				
		mount_entry = getmntent(fp);
		if (mount_entry !=  NULL)
		{
			// Look for the mount point.

			// Look for the mount point.
			drive_available = !(strcmp(mount_entry->mnt_dir,
					drive_mount_point));
		}
	}
	// Stop looking on finding the drive or reaching the end of the structure.
		
	while ((!drive_available) && (mount_entry != NULL));

	return drive_available;
}

// Detects if a drive is mounted.
// Searches /proc/mounts for an entry containing "dev/sda".
// drive_mount_point - char array that will hold the obtained drive mount point or null.
// max_length - sizeof(drive_mount_point)
// Returns 1 if found, 0 if not found.

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



