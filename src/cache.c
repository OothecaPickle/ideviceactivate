/*
 * cache.c
 * Load data from/add data to the activation info cache.
 *
 * Copyright (c) 2010 boxingsquirrel. All Rights Reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <plist/plist.h>
#include <libimobiledevice/lockdown.h>

#include "util.h"
#include "cache.h"

int write_file(const char *filename, char* data, int size) {
	int count = 0;

	FILE* fd = fopen(filename, "w");

	if (fd == NULL) {
		printf("ERROR: Could not open %s for writing\n", filename);
		return -1;

	} else {
		count = fwrite(data, 1, size, fd);
		if (count != size) {
			printf("ERROR: Could not write all data to find\n");
			fclose(fd);
			return -1;
		}
		fclose(fd);
	}

	return count;
}

int read_file(const char *filename, char* data, int size) {
	int count = 0;
	printf("DEBUG: Openning %s\n", filename);
	FILE* fd = fopen(filename, "r");

	if (fd == NULL) {
		printf("ERROR: Could not open %s for reading\n", filename);
		return -1;

	} else {
		count = fread(data, 1, size, fd);
		if(count <= 0) {
			printf("ERROR: Could not read the full file in\n");
			fclose(fd);
			return -1;
		}
		fclose(fd);
	}

	printf("\nINFO: Read %d bytes sucessfully from %s\n", count, filename);
	return count;
}

int cache(const char* fname, const char* what) {
	char data[BUFSIZE];
	char f_name[BUFSIZE];

	if (backup_to_cache) {
		snprintf(data, BUFSIZE-1, "%s", what);
		snprintf(f_name, BUFSIZE-1, "%s/%s", cachedir, fname);

		if (write_file(f_name, data, strlen(data)) <= 0) {
			return -1;
		}
	}
	return 0;
}

int cache_plist(const char *fname, plist_t plist) {
	if (backup_to_cache == 1) {
		uint32_t len = 0;
		char *xml = NULL;
		plist_to_xml(plist, &xml, &len);
		return cache(fname, (const char *) xml);

	} else {
		return -1;
	}
}

char* get_from_cache(const char *what) {
	char fname[BUFSIZE];
	snprintf(fname, BUFSIZE-1, "%s/%s", cachedir, what);

	char* data = malloc(BUFSIZE);
	if(data == NULL) {
		return NULL;
	}

	if(read_file(fname, data, BUFSIZE-1) <= 0) {
		return NULL;
	}

	return data;
}

/* Just prints a little notice about what caching actually does... */
void cache_warning() {
	printf(
			"The process of creating a cache is simple: perform a legit activation, storing all the required data. "
				"That way, you can borrow (or, I guess, steal (don't do that, though)) a sim for the carrier your iPhone "
				"is locked to, and be able to reactivate without having to get that sim back.\n\n"
				"This data is stored in a folder where you want it (hence the folder passed with -c/-r). "
				"It does not get sent to me (boxingsquirrel), p0sixninja, or anyone else. "
				"Plus, we really have better things to do than look at your activation data.\n\n"
				"This really isn't needed for iPod Touches or Wi-Fi only iPads (and I don't know if 3G iPad users need this, but be safe and do it).\n\n"
				"Press any key to continue or CONTROL-C to abort...\n\n"
			);

	// press any key to continue
	getchar();
}

/* Validates the cache to make sure it really is the cache for the connected device... */
int check_cache(lockdownd_client_t lockdownd) {
	char* uuid_from_device = NULL;
	char* uuid_from_cache = NULL;

	uuid_from_cache = get_from_cache("UUID");
	lockdownd_error_t error = lockdownd_get_device_uuid(lockdownd, &uuid_from_device);
	if(error != LOCKDOWN_E_SUCCESS) {
		return -1;
	}

	printf("Comparing %s and %s\n", uuid_from_cache, uuid_from_device);
	if(strcmp(uuid_from_cache, uuid_from_device)) {
		return 1;
	}

	return 0;

}
