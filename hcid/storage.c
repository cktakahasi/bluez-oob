/*
 *
 *  BlueZ - Bluetooth protocol stack for Linux
 *
 *  Copyright (C) 2004-2006  Marcel Holtmann <marcel@holtmann.org>
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/socket.h>

#include <bluetooth/bluetooth.h>

#include "textfile.h"
#include "hcid.h"

int write_local_name(bdaddr_t *bdaddr, char *name)
{
	char filename[PATH_MAX + 1], addr[18], str[249];
	int i;

	memset(str, 0, sizeof(str));
	for (i = 0; i < 248 && name[i]; i++)
		if (isprint(name[i]))
			str[i] = name[i];
		else
			str[i] = '.';

	ba2str(bdaddr, addr);
	snprintf(filename, PATH_MAX, "%s/%s/config", STORAGEDIR, addr);

	create_file(filename, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	return textfile_put(filename, "name", str);
}

int read_local_name(bdaddr_t *bdaddr, char *name)
{
	char filename[PATH_MAX + 1], addr[18], *str;
	int len;

	ba2str(bdaddr, addr);
	snprintf(filename, PATH_MAX, "%s/%s/config", STORAGEDIR, addr);

	str = textfile_get(filename, "name");
	if (!str)
		return -ENOENT;

	len = strlen(str);
	if (len > 248)
		str[248] = '\0';
	strcpy(name, str);

	free(str);

	return 0;
}

int write_local_class(bdaddr_t *bdaddr, uint8_t *class)
{
	char filename[PATH_MAX + 1], addr[18], str[9];

	sprintf(str, "0x%2.2x%2.2x%2.2x", class[2], class[1], class[0]);

	ba2str(bdaddr, addr);
	snprintf(filename, PATH_MAX, "%s/%s/config", STORAGEDIR, addr);

	create_file(filename, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	return textfile_put(filename, "class", str);
}

int read_local_class(bdaddr_t *bdaddr, uint8_t *class)
{
	char filename[PATH_MAX + 1], addr[18], *str;

	ba2str(bdaddr, addr);
	snprintf(filename, PATH_MAX, "%s/%s/config", STORAGEDIR, addr);

	str = textfile_get(filename, "class");
	if (!str)
		return -ENOENT;

	free(str);

	return 0;
}

int write_device_name(bdaddr_t *local, bdaddr_t *peer, char *name)
{
	char filename[PATH_MAX + 1], addr[18], str[249];
	int i;

	memset(str, 0, sizeof(str));
	for (i = 0; i < 248 && name[i]; i++)
		if (isprint(name[i]))
			str[i] = name[i];
		else
			str[i] = '.';

	ba2str(local, addr);
	snprintf(filename, PATH_MAX, "%s/%s/names", STORAGEDIR, addr);

	create_file(filename, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	ba2str(peer, addr);
	return textfile_put(filename, addr, str);
}

int read_device_name(bdaddr_t *local, bdaddr_t *peer, char *name)
{
	char filename[PATH_MAX + 1], addr[18], *str;
	int len;

	ba2str(local, addr);
	snprintf(filename, PATH_MAX, "%s/%s/names", STORAGEDIR, addr);

	ba2str(peer, addr);
	str = textfile_get(filename, addr);
	if (!str)
		return -ENOENT;

	len = strlen(str);
	if (len > 248)
		str[248] = '\0';
	strcpy(name, str);

	free(str);

	return 0;
}

int write_version_info(bdaddr_t *local, bdaddr_t *peer, uint16_t manufacturer, uint8_t lmp_ver, uint16_t lmp_subver)
{
	char filename[PATH_MAX + 1], addr[18], str[16];

	memset(str, 0, sizeof(str));
	sprintf(str, "%d %d %d", manufacturer, lmp_ver, lmp_subver);

	ba2str(local, addr);
	snprintf(filename, PATH_MAX, "%s/%s/manufacturers", STORAGEDIR, addr);

	create_file(filename, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	ba2str(peer, addr);
	return textfile_put(filename, addr, str);
}

int write_features_info(bdaddr_t *local, bdaddr_t *peer, unsigned char *features)
{
	char filename[PATH_MAX + 1], addr[18], str[17];
	int i;

	memset(str, 0, sizeof(str));
	for (i = 0; i < 8; i++)
		sprintf(str + (i * 2), "%2.2X", features[i]);

	ba2str(local, addr);
	snprintf(filename, PATH_MAX, "%s/%s/features", STORAGEDIR, addr);

	create_file(filename, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	ba2str(peer, addr);
	return textfile_put(filename, addr, str);
}

int write_lastseen_info(bdaddr_t *local, bdaddr_t *peer, struct tm *tm)
{
	char filename[PATH_MAX + 1], addr[18], str[24];

	memset(str, 0, sizeof(str));
	strftime(str, sizeof(str), "%Y-%m-%d %H:%M:%S %Z", tm);

	ba2str(local, addr);
	snprintf(filename, PATH_MAX, "%s/%s/lastseen", STORAGEDIR, addr);

	create_file(filename, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	ba2str(peer, addr);
	return textfile_put(filename, addr, str);
}

int write_lastused_info(bdaddr_t *local, bdaddr_t *peer, struct tm *tm)
{
	char filename[PATH_MAX + 1], addr[18], str[24];

	memset(str, 0, sizeof(str));
	strftime(str, sizeof(str), "%Y-%m-%d %H:%M:%S %Z", tm);

	ba2str(local, addr);
	snprintf(filename, PATH_MAX, "%s/%s/lastused", STORAGEDIR, addr);

	create_file(filename, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	ba2str(peer, addr);
	return textfile_put(filename, addr, str);
}

int write_link_key(bdaddr_t *local, bdaddr_t *peer, unsigned char *key, int type, int length)
{
	char filename[PATH_MAX + 1], addr[18], str[38];
	int i;

	memset(str, 0, sizeof(str));
	for (i = 0; i < 16; i++)
		sprintf(str + (i * 2), "%2.2X", key[i]);
	sprintf(str + 32, " %d %d", type, length);

	ba2str(local, addr);
	snprintf(filename, PATH_MAX, "%s/%s/linkkeys", STORAGEDIR, addr);

	create_file(filename, S_IRUSR | S_IWUSR);

	ba2str(peer, addr);

	if (length < 0) {
		char *tmp = textfile_get(filename, addr);
		if (tmp && strlen(tmp) > 34)
			memcpy(str + 34, tmp + 34, 3);
	}

	return textfile_put(filename, addr, str);
}

int read_link_key(bdaddr_t *local, bdaddr_t *peer, unsigned char *key)
{
	char filename[PATH_MAX + 1], addr[18], tmp[3], *str;
	int i;

	ba2str(local, addr);
	snprintf(filename, PATH_MAX, "%s/%s/linkkeys", STORAGEDIR, addr);

	ba2str(peer, addr);
	str = textfile_get(filename, addr);
	if (!str)
		return -ENOENT;

	memset(tmp, 0, sizeof(tmp));
	for (i = 0; i < 16; i++) {
		memcpy(tmp, str + (i * 2), 2);
		key[i] = (uint8_t) strtol(tmp, NULL, 16);
	}

	free(str);

	return 0;
}

int read_pin_length(bdaddr_t *local, bdaddr_t *peer)
{
	char filename[PATH_MAX + 1], addr[18], *str;
	int len;

	ba2str(local, addr);
	snprintf(filename, PATH_MAX, "%s/%s/linkkeys", STORAGEDIR, addr);

	ba2str(peer, addr);
	str = textfile_get(filename, addr);
	if (!str)
		return -ENOENT;

	if (strlen(str) < 36)
		return -ENOENT;

	len = atoi(str + 35);

	free(str);

	return len;
}

int read_pin_code(bdaddr_t *local, bdaddr_t *peer, char *pin)
{
	char filename[PATH_MAX + 1], addr[18], *str;
	int len;

	ba2str(local, addr);
	snprintf(filename, PATH_MAX, "%s/%s/pincodes", STORAGEDIR, addr);

	ba2str(peer, addr);
	str = textfile_get(filename, addr);
	if (!str)
		return -ENOENT;

	strncpy(pin, str, 16);
	len = strlen(pin);

	free(str);

	return len;
}
