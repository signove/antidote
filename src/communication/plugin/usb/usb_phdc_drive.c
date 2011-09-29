/**
 * \file usb_phdc_drive.c
 * \brief Functions to read/write data to USB PHDC compatible devices
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


#include "usb_phdc_drive.h"
#include "usb_phdc_definitions.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const int MAX_BUFFER_SIZE = 1024;

static int *get_device_specializations(const unsigned char *buffer, int buffer_length,
		int *num_specs)
{
	int spec = 0;
	int len = 0;
	uint8_t type = 0;
	int index = 0;
	int *spec_list = NULL;
	int i;

	if (buffer_length > 0) {
		do {
			len = (int) buffer[index];
			type = buffer[index + 1];
			if (type == PHDC_11073PHD_FUNCTION_DESCRIPTOR) {
				*num_specs = (int) buffer[index + 3];
				if (*num_specs > 0) {
					spec_list = (int *) calloc(1, (*num_specs) * sizeof(int));
					for (i = 0; i < *num_specs; i++) {
						spec = (int) buffer[2 * i + index + 4]
								+ ((int) buffer[2 * i + index + 5]) * 256;
						spec_list[i] = spec;
					}
				}

			}
			index += len;
		} while (index < buffer_length);

	}

	return spec_list;
}

static void get_phdc_device_attributes(libusb_device* device,
		struct libusb_device_descriptor device_descriptor, usb_phdc_device *phdc_device)
{
	unsigned char manufacturer[255];
	unsigned char product[255];
	unsigned char serial[255];
	int len;
	int i, j;
	int type;
	int direction;

	libusb_device_handle *dev_handler;
	const struct libusb_interface_descriptor *interface_desc;
	struct libusb_endpoint_descriptor ep;
	struct libusb_config_descriptor *config_desc;
	int number_of_specializations = 0;

	if (libusb_open(device, &dev_handler) != LIBUSB_SUCCESS) {
		return;
	}

	if (libusb_get_config_descriptor(device, 0, &config_desc) != LIBUSB_SUCCESS) {
		libusb_close(dev_handler);
		return;
	}

	for (j = 0; j < config_desc->bNumInterfaces; j++) {

		if (libusb_claim_interface(dev_handler, j) != LIBUSB_SUCCESS) {
			libusb_close(dev_handler);
			return;
		}

		interface_desc = &(config_desc->interface[j].altsetting[0]);

		if (interface_desc->bInterfaceClass == PHDC_INTERFACE_CLASS) {
			phdc_device->health_interface = j;

			phdc_device->specializations = get_device_specializations(interface_desc->extra,
					interface_desc->extra_length, &number_of_specializations);
			phdc_device->number_of_specializations = number_of_specializations;

			for (i = 0; i < interface_desc->bNumEndpoints; i++) {
				ep = interface_desc->endpoint[i];
				type = (ep.bmAttributes & 0x03);
				direction = (ep.bEndpointAddress & 0x80);
				if (type == LIBUSB_TRANSFER_TYPE_BULK && direction == LIBUSB_ENDPOINT_IN) {
					phdc_device->ep_bulk_in = ep.bEndpointAddress;
				} else if (type == LIBUSB_TRANSFER_TYPE_BULK && direction == LIBUSB_ENDPOINT_OUT) {
					phdc_device->ep_bulk_out = ep.bEndpointAddress;
				}
				fprintf(stdout, "Ep address: %d, type: %d, direction: %d\n", ep.bEndpointAddress,
						type, direction);
			}
			break;
		}
		libusb_release_interface(dev_handler, j);
	}

	libusb_free_config_descriptor(config_desc);

	phdc_device->vendor_id = device_descriptor.idVendor;
	phdc_device->product_id = device_descriptor.idProduct;

	len = libusb_get_string_descriptor_ascii(dev_handler, device_descriptor.iProduct, product,
			sizeof(product));

	phdc_device->name = (char *) calloc(1, len + 1);
	strncpy(phdc_device->name, (char *) product, len);

	len = libusb_get_string_descriptor_ascii(dev_handler, device_descriptor.iManufacturer, manufacturer,
			sizeof(manufacturer));

	phdc_device->manufacturer = (char *) calloc(1, len + 1);
	strncpy(phdc_device->manufacturer, (char *) manufacturer, len);

	len = libusb_get_string_descriptor_ascii(dev_handler, device_descriptor.iSerialNumber, serial,
			sizeof(serial));

	phdc_device->serial_number = (char *) calloc(1, len + 1);
	strncpy(phdc_device->serial_number, (char *) serial, len);

	libusb_release_interface(dev_handler, j);
	libusb_close(dev_handler);
}

static int is_ieee11073_compatible(const unsigned char *buffer, int buffer_length)
{
	int ret = 0;
	int number_of_specs = 0;
	int is_ieee11073 = 0;
	int len = 0;
	uint8_t type = 0;
	uint8_t value = 0;
	int index = 0;

	if (buffer_length > 0) {
		do {
			len = (int) buffer[index];
			type = (buffer[index + 1] & 0xFF);
			if (type == PHDC_CLASSFUNCTION_DESCRIPTOR) {
				value = (buffer[index + 2] & 0xFF);
				if (value == PHDC_11073_20601) {
					fprintf(stdout, "IEEE 11073 20601 Compatible Device\n");
					is_ieee11073 = 1;
				} else if (value == PHDC_VENDOR) {
					fprintf(stdout, "Vendor device, not IEEE 11073 compatible\n");
					is_ieee11073 = 0;
				}
			} else if (type == PHDC_11073PHD_FUNCTION_DESCRIPTOR) {
				number_of_specs = (int) (buffer[index + 3] & 0xFF);
			}
			index += len;
		} while (index < buffer_length);

	}

	ret = (number_of_specs > 0 && is_ieee11073 == 1);

	return ret;
}

static int is_phdc_11073_device(libusb_device *device,
		struct libusb_device_descriptor device_descriptor)
{
	libusb_device_handle *devh;
	const struct libusb_interface_descriptor *interface_desc;
	struct libusb_config_descriptor *config_desc;
	int ret;
	int result = 0;
	int j;

	ret = libusb_open(device, &devh);

	if (ret < 0) {
		return 0;
	}

	ret = libusb_claim_interface(devh, 0);
	if (ret < 0) {
		return 0;
	}

	ret = libusb_get_config_descriptor(device, 0, &config_desc);
	if (ret < 0) {
		return 0;
	}

	for (j = 0; j < config_desc->bNumInterfaces; j++) {
		interface_desc = &(config_desc->interface[j].altsetting[0]);
		if (interface_desc->bInterfaceClass == PHDC_INTERFACE_CLASS) {
			fprintf(stdout, "Medical device found\n");
			result = is_ieee11073_compatible(interface_desc->extra, interface_desc->extra_length);
			break;
		}
	}

	libusb_free_config_descriptor(config_desc);
	libusb_release_interface(devh, 0);
	libusb_close(devh);

	return result;
}

static void request_usb_data_cb(struct libusb_transfer *transfer)
{
	usb_phdc_device *phdc_device = (usb_phdc_device *) transfer->user_data;
	fprintf(stdout, "Data received with status: %d\n", transfer->status);

	if (transfer->status == LIBUSB_TRANSFER_COMPLETED) {
		phdc_device->data_read_cb(phdc_device, transfer->buffer, transfer->actual_length);
	} else if (transfer->status == LIBUSB_TRANSFER_ERROR) {
		phdc_device->error_read_cb(phdc_device);
	}
}

static void send_data_cb(struct libusb_transfer *transfer)
{
	fprintf(stdout, "send_data_cb completed: %d\n", transfer->status);
	libusb_free_transfer(transfer);
}

static void query_phdc_fds(usb_phdc_device *phdc_device)
{
	int i = 0;
	int total_events = 0;
	const struct libusb_pollfd** lpfds = libusb_get_pollfds(phdc_device->usb_device_context);

	while (lpfds[i] != NULL) {
		i++;
	}

	total_events = i;

	phdc_device->fds = (struct pollfd*) calloc(1, total_events * sizeof(struct pollfd));
	phdc_device->fds_count = total_events;

	for (i = 0; i < total_events; i++) {
		phdc_device->fds[i].fd = lpfds[i]->fd;
		phdc_device->fds[i].events = lpfds[i]->events;
		fprintf(stdout, "File descriptor watched %d, evt %d\n", lpfds[i]->fd, lpfds[i]->events);
	}

	free(lpfds);

	phdc_device->buffer_in = (unsigned char *) calloc(1, MAX_BUFFER_SIZE * sizeof(unsigned char));

	phdc_device->read_transfer = libusb_alloc_transfer(0);

	libusb_fill_bulk_transfer(phdc_device->read_transfer, phdc_device->usb_device_handle,
			phdc_device->ep_bulk_in, phdc_device->buffer_in, MAX_BUFFER_SIZE, request_usb_data_cb,
			(void *) phdc_device, 0);
}

int open_phdc_handle(usb_phdc_device *phdc_device)
{
	int error_handler;
	int ret = 0;

	error_handler = libusb_init(&phdc_device->usb_device_context);
	if (error_handler != LIBUSB_SUCCESS) {
		fprintf(stdout, "libusb_init failure %d\n", error_handler);
		goto init_failure;
	}

	phdc_device->usb_device_handle = libusb_open_device_with_vid_pid(
			phdc_device->usb_device_context, phdc_device->vendor_id,
			phdc_device->product_id);
	if (phdc_device->usb_device_handle == NULL) {
		fprintf(stdout, "libusb_open failure %d\n", error_handler);
		goto release_device;
	}

	error_handler = libusb_claim_interface(phdc_device->usb_device_handle, phdc_device->health_interface);
	if (error_handler != LIBUSB_SUCCESS) {
		fprintf(stdout, "libusb_claim_interface failure %d\n", error_handler);
		goto close_device;
	}

	libusb_reset_device(phdc_device->usb_device_handle);

	query_phdc_fds(phdc_device);

	ret = 1;

	return ret;

close_device:
	libusb_close(phdc_device->usb_device_handle);

release_device:
	libusb_unref_device(phdc_device->usb_device);

init_failure:
	return ret;
}

int usb_send_apdu(usb_phdc_device *phdc_device, unsigned char *data, int len)
{
	int ret;
	struct libusb_transfer *transfer;

	fprintf(stdout, "network_usb_send_apdu_stream\n");

	transfer = libusb_alloc_transfer(0);

	libusb_fill_bulk_transfer(transfer, phdc_device->usb_device_handle, phdc_device->ep_bulk_out,
			data, len, send_data_cb, NULL, 0);

	ret = libusb_submit_transfer(transfer);

	fprintf(stdout, "libusb_bulk_transfer status: %d\n", ret);

	return ret == LIBUSB_SUCCESS ? 1 : 0;
}

void search_phdc_devices(usb_phdc_context *phdc_context)
{
	libusb_device **device_list;
	ssize_t number_of_devices;
	libusb_device *device;
	usb_phdc_device *phdc_device;
	int i = 0;

	number_of_devices = libusb_get_device_list(phdc_context->usb_context, &device_list);

	while ((device = device_list[i++]) != NULL) {
		struct libusb_device_descriptor desc;
		int r = libusb_get_device_descriptor(device, &desc);
		if (r < 0) {
			fprintf(stderr, "Failed to get device descriptor\n");
			continue;
		}
		if (is_phdc_11073_device(device, desc) == 1) {
			phdc_context->number_of_devices++;
			phdc_context->device_list = (usb_phdc_device *) realloc(phdc_context->device_list,
					phdc_context->number_of_devices * sizeof(usb_phdc_device));

			phdc_device = &(phdc_context->device_list[phdc_context->number_of_devices - 1]);
			phdc_device->usb_device = libusb_ref_device(device);
			get_phdc_device_attributes(device, desc, phdc_device);

		}
	}

	libusb_free_device_list(device_list, 1);
}

int init_phdc_usb_plugin(usb_phdc_context *phdc_context)
{
	int ret;
	ret = libusb_init(&phdc_context->usb_context);
	return ret;
}

void print_phdc_info(usb_phdc_device *phdc_device)
{
	int i;

	fprintf(stdout, "Device name: %s\n", phdc_device->name);
	fprintf(stdout, "Device manufacturer: %s\n", phdc_device->manufacturer);
	fprintf(stdout, "Device serial: %s\n", phdc_device->serial_number);
	fprintf(stdout, "Number of specializations: %d\n", phdc_device->number_of_specializations);

	for (i = 0; i < phdc_device->number_of_specializations; i++) {
		fprintf(stdout, "Specialization %d: %d\n", i, phdc_device->specializations[i]);
	}

}

static void release_phdc_device(usb_phdc_device *phdc_device)
{
	if (phdc_device->usb_device_handle != NULL) {
		libusb_release_interface(phdc_device->usb_device_handle, phdc_device->health_interface);
		libusb_close(phdc_device->usb_device_handle);
		phdc_device->usb_device_handle = NULL;
	}

	if (phdc_device->usb_device != NULL) {
		libusb_unref_device(phdc_device->usb_device);
		phdc_device->usb_device = NULL;
	}

	if (phdc_device->manufacturer != NULL) {
		free(phdc_device->manufacturer);
		phdc_device->manufacturer = NULL;
	}

	if (phdc_device->name != NULL) {
		free(phdc_device->name);
		phdc_device->name = NULL;
	}

	if (phdc_device->serial_number != NULL) {
		free(phdc_device->serial_number);
		phdc_device->serial_number = NULL;
	}

	if (phdc_device->specializations != NULL) {
		free(phdc_device->specializations);
		phdc_device->specializations = NULL;
	}

	if (phdc_device->fds_count > 0) {
		phdc_device->fds_count = 0;
		free(phdc_device->fds);
		phdc_device->fds = NULL;
	}

	if (phdc_device->read_transfer != NULL) {
		libusb_free_transfer(phdc_device->read_transfer);
		phdc_device->read_transfer = NULL;
	}

	if (phdc_device->usb_device_context != NULL) {
		libusb_exit(phdc_device->usb_device_context);
		phdc_device->usb_device_context = NULL;
	}

	if (phdc_device->buffer_in != NULL) {
		free(phdc_device->buffer_in);
		phdc_device->buffer_in = NULL;
	}

}

void release_phdc_resources(usb_phdc_context *phdc_context)
{
	int k;
	usb_phdc_device *phdc_device;
	for (k = 0; k < phdc_context->number_of_devices; k++) {
		phdc_device = &(phdc_context->device_list[k]);
		release_phdc_device(phdc_device);
	}
	libusb_exit(phdc_context->usb_context);
}

static void request_usb_data(usb_phdc_device *phdc_device)
{
	libusb_submit_transfer(phdc_device->read_transfer);
}

void poll_phdc_device_pre(usb_phdc_device *phdc_device)
{	
	// TODO is this necessary every time a poll() returns?
	request_usb_data(phdc_device);
}

void poll_phdc_device_post(usb_phdc_device *phdc_device)
{
	libusb_handle_events(phdc_device->usb_device_context);
}

int poll_phdc_device(usb_phdc_device *phdc_device)
{

	int has_events = 0;
	int evt_count = 0;

	fprintf(stdout, "poll_phdc_device\n");

	poll_phdc_device_pre(phdc_device);

	evt_count = poll(phdc_device->fds, phdc_device->fds_count, -1);

	if (evt_count > 0) {
		has_events = 1;
	}

	if (has_events) {
		poll_phdc_device_post(phdc_device);
	}

	return has_events;
}
