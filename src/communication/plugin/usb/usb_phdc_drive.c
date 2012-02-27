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
#include "src/util/log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * \cond Undocumented
 */

static const int MAX_BUFFER_SIZE = 1024;

static void (*schedule_timeout)(int);

static int *get_device_specializations(const unsigned char *buffer, int buffer_length,
		int *num_specs)
{
	int spec = 0;
	int len = 0;
	uint8_t type = 0;
	int index;
	int *spec_list = NULL;
	int i;

	for (index = 0; index < buffer_length; index += len) {
		len = (int) buffer[index];
		type = buffer[index + 1];

		if (type != PHDC_11073PHD_FUNCTION_DESCRIPTOR) {
			continue;
		}

		*num_specs = (int) buffer[index + 3];
		if (*num_specs <= 0) {
			continue;
		}

		spec_list = (int *) calloc(1, (*num_specs) * sizeof(int));
		for (i = 0; i < *num_specs; i++) {
			spec = (int) buffer[2 * i + index + 4]
				+ ((int) buffer[2 * i + index + 5]) * 256;
			spec_list[i] = spec;
		}

	}

	return spec_list;
}

static int get_phdc_device_attributes(libusb_device* device,
		struct libusb_device_descriptor device_descriptor, usb_phdc_device *phdc_device)
{
	unsigned char manufacturer[255];
	unsigned char product[255];
	unsigned char serial[255];
	int len;
	int i, j;
	int type;
	int direction;
	int found = 0;

	libusb_device_handle *dev_handler;
	const struct libusb_interface_descriptor *interface_desc;
	struct libusb_endpoint_descriptor ep;
	struct libusb_config_descriptor *config_desc;
	int number_of_specializations = 0;

	phdc_device->ok = 0;

	if (libusb_open(device, &dev_handler) != LIBUSB_SUCCESS) {
		return 0;
	}

	if (libusb_get_config_descriptor(device, 0, &config_desc) != LIBUSB_SUCCESS) {
		libusb_close(dev_handler);
		return 0;
	}

	for (j = 0; j < config_desc->bNumInterfaces; j++) {
		if (libusb_claim_interface(dev_handler, j) != LIBUSB_SUCCESS) {
			libusb_close(dev_handler);
			return 0;
		}

		interface_desc = &(config_desc->interface[j].altsetting[0]);

		if (interface_desc->bInterfaceClass != PHDC_INTERFACE_CLASS) {
			continue;
		}

		found = 1;

		phdc_device->health_interface = j;

		phdc_device->specializations = get_device_specializations(interface_desc->extra,
				interface_desc->extra_length, &number_of_specializations);
		phdc_device->number_of_specializations = number_of_specializations;

		for (i = 0; i < interface_desc->bNumEndpoints; i++) {
			ep = interface_desc->endpoint[i];
			type = (ep.bmAttributes & 0x03);
			direction = (ep.bEndpointAddress & 0x80);

			if (type == LIBUSB_TRANSFER_TYPE_BULK &&
						direction == LIBUSB_ENDPOINT_IN) {
				phdc_device->ep_bulk_in = ep.bEndpointAddress;
			} else if (type == LIBUSB_TRANSFER_TYPE_BULK &&
						direction == LIBUSB_ENDPOINT_OUT) {
				phdc_device->ep_bulk_out = ep.bEndpointAddress;
			}

			// TODO support for interrupt endpoints
			// TODO support for multiple bulk endpoints

			DEBUG("Ep addr: %d, type: %d, direction: %d",
				ep.bEndpointAddress, type, direction);
		}

		// keep focus on interface that we've found
		break;
	}

	if (! found) {
		DEBUG("Trouble finding PHDC interface");
		libusb_release_interface(dev_handler, j);
		libusb_close(dev_handler);
		return 0;
	}

	libusb_free_config_descriptor(config_desc);

	phdc_device->vendor_id = device_descriptor.idVendor;
	phdc_device->product_id = device_descriptor.idProduct;

	len = libusb_get_string_descriptor_ascii(dev_handler, device_descriptor.iProduct, product,
			sizeof(product));
	if (len < 0) {
		DEBUG("Trouble getting USB product");
		libusb_release_interface(dev_handler, j);
		libusb_close(dev_handler);
		return 0;
	}

	phdc_device->name = (char *) calloc(1, len + 1);
	strncpy(phdc_device->name, (char *) product, len);

	len = libusb_get_string_descriptor_ascii(dev_handler, device_descriptor.iManufacturer,
					manufacturer, sizeof(manufacturer));

	if (len < 0) {
		DEBUG("Trouble getting USB manufacturer");
		libusb_release_interface(dev_handler, j);
		libusb_close(dev_handler);
		return 0;
	}

	phdc_device->manufacturer = (char *) calloc(1, len + 1);
	strncpy(phdc_device->manufacturer, (char *) manufacturer, len);

	len = libusb_get_string_descriptor_ascii(dev_handler, device_descriptor.iSerialNumber, serial,
			sizeof(serial));

	if (len < 0) {
		DEBUG("Trouble getting USB serial number");
		libusb_release_interface(dev_handler, j);
		libusb_close(dev_handler);
		return 0;
	}

	phdc_device->serial_number = (char *) calloc(1, len + 1);
	strncpy(phdc_device->serial_number, (char *) serial, len);

	libusb_release_interface(dev_handler, j);
	libusb_close(dev_handler);

	phdc_device->ok = 1;
	DEBUG("PHDC device ok");

	return 1;
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
					DEBUG("IEEE 11073 20601 Compatible Device");
					is_ieee11073 = 1;
				} else if (value == PHDC_VENDOR) {
					DEBUG("Vendor device, not IEEE 11073 compatible");
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
		libusb_close(devh);
		return 0;
	}

	ret = libusb_get_config_descriptor(device, 0, &config_desc);
	if (ret < 0) {
		libusb_release_interface(devh, 0);
		libusb_close(devh);
		return 0;
	}

	for (j = 0; j < config_desc->bNumInterfaces; j++) {
		interface_desc = &(config_desc->interface[j].altsetting[0]);
		if (interface_desc->bInterfaceClass == PHDC_INTERFACE_CLASS) {
			DEBUG("Medical device found");
			result = is_ieee11073_compatible(interface_desc->extra,
							interface_desc->extra_length);
			break;
		}
	}

	libusb_free_config_descriptor(config_desc);
	libusb_release_interface(devh, 0);
	libusb_close(devh);

	return result;
}

static void stop_phdc_device(usb_phdc_device *phdc_device);

static void request_usb_data_cb(struct libusb_transfer *transfer)
{
	usb_phdc_device *phdc_device = (usb_phdc_device *) transfer->user_data;

	if (transfer->status == LIBUSB_TRANSFER_COMPLETED) {
		phdc_device->data_read_cb(phdc_device, transfer->buffer, transfer->actual_length);
		// reschedule next read
		poll_phdc_device_pre(phdc_device);
	} else if (transfer->status == LIBUSB_TRANSFER_NO_DEVICE) {
		DEBUG("Data received with status: NO_DEVICE");
		stop_phdc_device(phdc_device);
		phdc_device->device_gone_cb(phdc_device);
	} else if (transfer->status == LIBUSB_TRANSFER_CANCELLED) {
		DEBUG("Data received with status: CANCELLED");
	} else {
		DEBUG("Data received with error status: %d", transfer->status);
		stop_phdc_device(phdc_device);
		phdc_device->error_read_cb(phdc_device);
	}
}

static void send_data_cb(struct libusb_transfer *transfer)
{
	DEBUG("send_data_cb completed: %d", transfer->status);
	if (transfer->status == 0) {
		// Not freeing when status != 0 leaks, but freeing it
		// makes valgrind complain about double freeing a
		// region. libusb bug?
		libusb_free_transfer(transfer);
	}
}

static void query_phdc(usb_phdc_device *phdc_device)
{
	if (! phdc_device->ok)
		return;

	phdc_device->buffer_in = (unsigned char *) calloc(1, MAX_BUFFER_SIZE * sizeof(unsigned char));

	phdc_device->read_transfer = libusb_alloc_transfer(0);

	// TODO match timeout with IEEE
	libusb_fill_bulk_transfer(phdc_device->read_transfer,
					phdc_device->usb_device_handle,
					phdc_device->ep_bulk_in,
					phdc_device->buffer_in, MAX_BUFFER_SIZE,
					request_usb_data_cb,
					(void *) phdc_device, 0);
}

int open_phdc_handle(usb_phdc_device *phdc_device, usb_phdc_context *ctx)
{
	int error_handler;
	int ret = 0;

	phdc_device->usb_device_handle = libusb_open_device_with_vid_pid(
			ctx->usb_context, phdc_device->vendor_id,
			phdc_device->product_id);

	if (phdc_device->usb_device_handle == NULL) {
		DEBUG("libusb_open_device failed");
		goto release_device;
	}

	error_handler = libusb_claim_interface(phdc_device->usb_device_handle,
						phdc_device->health_interface);
	if (error_handler != LIBUSB_SUCCESS) {
		DEBUG("libusb_claim_interface failure %d",
			error_handler);
		goto close_device;
	}

	libusb_reset_device(phdc_device->usb_device_handle);

	query_phdc(phdc_device);

	ret = 1;

	return ret;

close_device:
	libusb_close(phdc_device->usb_device_handle);

release_device:
	libusb_unref_device(phdc_device->usb_device);

	return ret;
}

int usb_send_apdu(usb_phdc_device *phdc_device, unsigned char *data, int len)
{
	int ret;
	struct libusb_transfer *transfer;

	DEBUG("usb_send_apdu");

	if (! phdc_device->ok) {
		DEBUG("usb_send_apdu: device not ok");
		return 0;
	}

	transfer = libusb_alloc_transfer(0);

	// TODO match timeout with IEEE
	libusb_fill_bulk_transfer(transfer, phdc_device->usb_device_handle,
			phdc_device->ep_bulk_out,
			data, len, send_data_cb, NULL, 10000);

	ret = libusb_submit_transfer(transfer);

	DEBUG("libusb_bulk_transfer status: %d", ret);

	return ret == LIBUSB_SUCCESS ? 1 : 0;
}

void search_phdc_devices(usb_phdc_context *phdc_context)
{
	libusb_device **device_list;
	libusb_device *device;
	int i = 0;

	libusb_get_device_list(phdc_context->usb_context, &device_list);

	while ((device = device_list[i++]) != NULL) {
		usb_phdc_device new_device;
		struct libusb_device_descriptor desc;
		int r = libusb_get_device_descriptor(device, &desc);

		memset(&new_device, '\0', sizeof(usb_phdc_device));

		if (r < 0) {
			DEBUG("Failed to get device descriptor");
			continue;
		}

		if (is_phdc_11073_device(device, desc) != 1) {
			continue;
		}

		if (! get_phdc_device_attributes(device, desc, &new_device)) {
			continue;
		}

		phdc_context->number_of_devices++;
		phdc_context->device_list =
			(usb_phdc_device *) realloc(phdc_context->device_list,
			phdc_context->number_of_devices * sizeof(usb_phdc_device));

		new_device.usb_device = libusb_ref_device(device);
		phdc_context->device_list[phdc_context->number_of_devices - 1] = new_device;
	}

	libusb_free_device_list(device_list, 1);
}

static void schedule_next_timeout(usb_phdc_context *ctx)
{
	struct timeval tv;
	if (libusb_get_next_timeout(ctx->usb_context, &tv) == 1) {
		int ms = tv.tv_sec * 1000 + tv.tv_usec / 1000;
		schedule_timeout(ms);
		DEBUG("Scheduled next timeout to %d ms", ms);
	}
}

int init_phdc_usb_plugin(usb_phdc_context *phdc_context,
			void (*added_fd)(int fd, short events, void *ctx),
			void (*removed_fd)(int fd, void *ctx),
			void (*sch_timeout)(int ms))
{
	int ret;
	const struct libusb_pollfd** lpfds;
	int i;

	ret = libusb_init(&phdc_context->usb_context);
	if (ret != LIBUSB_SUCCESS) {
		DEBUG("libusb_init failure %d", ret);
		exit(1);
	}

	lpfds = libusb_get_pollfds(phdc_context->usb_context);

	for (i = 0; lpfds[i] != NULL; ++i) {
		if (added_fd) {
			added_fd(lpfds[i]->fd, lpfds[i]->events, NULL);
		}
	}
	
	free(lpfds);

	libusb_set_pollfd_notifiers(phdc_context->usb_context, added_fd, removed_fd, NULL);

	schedule_timeout = sch_timeout;
	schedule_next_timeout(phdc_context);

	return ret;
}

void print_phdc_info(usb_phdc_device *phdc_device)
{
	int i;

	DEBUG("Device name: %s",
			phdc_device->name);
	DEBUG("Device manufacturer: %s",
			phdc_device->manufacturer);
	DEBUG("Device serial: %s",
			phdc_device->serial_number);
	DEBUG("Number of specializations: %d",
			phdc_device->number_of_specializations);

	for (i = 0; i < phdc_device->number_of_specializations; i++) {
		DEBUG("Specialization %d: %d",
				i, phdc_device->specializations[i]);
	}
}

static void stop_phdc_device(usb_phdc_device *phdc_device)
{
	if (phdc_device->ok) {
		phdc_device->ok = 0;

		if (phdc_device->read_transfer != NULL) {
			DEBUG("USB device stopped (because of error/disconn)");
			libusb_cancel_transfer(phdc_device->read_transfer);
		}
	}
}

static void release_phdc_device(usb_phdc_device *phdc_device)
{
	stop_phdc_device(phdc_device);

	if (phdc_device->read_transfer != NULL) {
		libusb_free_transfer(phdc_device->read_transfer);
		phdc_device->read_transfer = NULL;
	}

	if (phdc_device->health_interface != -1) {
		libusb_release_interface(phdc_device->usb_device_handle,
					phdc_device->health_interface);
		phdc_device->health_interface = -1;
	}

	if (phdc_device->usb_device_handle != NULL) {
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

	libusb_set_pollfd_notifiers(phdc_context->usb_context, NULL, NULL, NULL);
	libusb_exit(phdc_context->usb_context);
}

static void request_usb_data(usb_phdc_device *phdc_device)
{
	libusb_submit_transfer(phdc_device->read_transfer);
}

void poll_phdc_device_pre(usb_phdc_device *phdc_device)
{
	if (phdc_device->ok)
		request_usb_data(phdc_device);
}

void poll_phdc_device_post(usb_phdc_context *ctx)
{
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	libusb_handle_events_timeout(ctx->usb_context, &tv);
	schedule_next_timeout(ctx);
}

void phdc_device_fd_event(usb_phdc_context *ctx)
{
	poll_phdc_device_post(ctx);
}

void phdc_device_timeout_event(usb_phdc_context *ctx)
{
	DEBUG("USB timeout alarm");
	poll_phdc_device_post(ctx);
}

int poll_phdc_device(usb_phdc_device *phdc_device, usb_phdc_context *ctx)
{
	int has_events = 0;
	int evt_count = 0;
	int fds_count;
	int i;
	const struct libusb_pollfd** lpfds;
	struct pollfd* fds;

	DEBUG("poll_phdc_device");

	poll_phdc_device_pre(phdc_device);

	lpfds = libusb_get_pollfds(ctx->usb_context);

	for (i = 0, fds_count = 0; lpfds[i] != NULL; ++i) {
		++fds_count;
	}

	fds = (struct pollfd*) calloc(1, fds_count * sizeof(struct pollfd));
	
	for (i = 0; i < fds_count; ++i) {
		fds[i].fd = lpfds[i]->fd;
		fds[i].events = lpfds[i]->events;
	}

	free(lpfds);

	evt_count = poll(fds, fds_count, -1);

	free(fds);

	if (evt_count > 0) {
		has_events = 1;
	}

	if (has_events) {
		poll_phdc_device_post(ctx);
	}

	return has_events;
}

/**
 * \endcond
 */
