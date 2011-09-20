/**
 * \file simple_main.c
 * \brief Main file to test implemented functions that handle phdc usb compatible devices
 *
 * Copyright (C) 2011 Signove Tecnologia Corporation.
 * All rights reserved.
 * Contact: Signove Tecnologia Corporation (contact@signove.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation and appearing
 * in the file LICENSE included in the packaging of this file; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 * \date Aug 31, 2011
 * \author Jose Luis do Nascimento
 */


#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "usb_phdc_drive.h"

usb_phdc_context *phdc_context = NULL;
usb_phdc_device *phdc_device = NULL;
int read_more_data = 1;

void print_read_data(unsigned char *buffer, int buffer_length);
void write_ieee_response(unsigned char *buffer_in, int buffer_length);

unsigned char association_response[] = { 0xe3, 0x00, 0x00, 0x2c,
		0x00,
		0x03, //accepted-unknown-config
		0x50, 0x79, 0x00, 0x26, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x08, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33,
		0x22, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

unsigned char association_release_response[] = { 0xE5, 0x00, 0x00, 0x02, 0x00, 0x00 };

unsigned char configuration_response[] = { 0xE7, 0x00, 0x00, 0x16, 0x00, 0x14, 0x00, 0x02, 0x02,
		0x01, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x1C, 0x00, 0x04, 0x40, 0x00,
		0x00, 0x00 };

void print_read_data(unsigned char *buffer, int buffer_length)
{
	int i;
	fprintf(stdout, "Data received length: %d \n", buffer_length);
	fprintf(stdout, "-----------------------------------------------\n");
	for (i = 0; i < buffer_length; i++) {
		fprintf(stdout, "0x%02x ", buffer[i]);
	}
	fprintf(stdout, "\n-----------------------------------------------\n");

	write_ieee_response(buffer, buffer_length);
	free(buffer);
}

//Fake write response
void write_ieee_response(unsigned char *buffer_in, int buffer_length)
{
	unsigned int msg_type;
	if (buffer_length <= 0) {
		return;
	}
	msg_type = (unsigned int) (buffer_in[0] & 0xFF);
	fprintf(stdout, "write_ieee_response for message type: %d\n", msg_type);

	if (msg_type == 0xE2) { //Association Request
		send_apdu_stream(phdc_device, association_response, sizeof(association_response));
	} else if (msg_type == 0xE7) { //Configuration received
		configuration_response[6] = buffer_in[6];
		configuration_response[7] = buffer_in[7];
		send_apdu_stream(phdc_device, configuration_response, sizeof(configuration_response));
	} else if (msg_type == 0xE4) { //Association Release Request
		send_apdu_stream(phdc_device, association_release_response,
				sizeof(association_release_response));
		read_more_data = 0; //Flag to exit from while main loop
	}

}

void request_exit(int s)
{
	fprintf(stdout, "Request Exit: %d\n", s);
	read_more_data = 0;
}

int main(int argc, char **argv)
{
	signal(SIGINT, request_exit);
	signal(SIGTERM, request_exit);
	signal(SIGQUIT, request_exit);

	phdc_context = (usb_phdc_context *) calloc(1, sizeof(usb_phdc_context));

	init_phdc_usb_plugin(phdc_context);
	search_phdc_devices(phdc_context);
	if (phdc_context->number_of_devices > 0) {
		phdc_device = &(phdc_context->device_list[0]); //Get the first device to read measurements

		phdc_device->data_read_cb = print_read_data;
		print_phdc_info(phdc_device);
		if (open_phdc_handle(phdc_device) == 1) {
			while (read_more_data == 1) {
				if (poll_phdc_device(phdc_device) == 1) {
				}
			}
			release_phdc_resources(phdc_context);
			free(phdc_context);
		}
	}

	return 0;
}
