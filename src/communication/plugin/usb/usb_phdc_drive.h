/**
 * \file usb_phdc_drive.h
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

#ifndef USB_PHDC_DRIVE_
#define USB_PHDC_DRIVE_

#include <libusb-1.0/libusb.h>
#include <poll.h>

struct usb_phdc_device;

typedef struct usb_phdc_device {

	libusb_device *usb_device;
	libusb_device_handle *usb_device_handle;

	struct libusb_transfer *read_transfer;
	unsigned char *buffer_in;

	uint8_t ep_bulk_in;
	uint8_t ep_bulk_out;
	int health_interface;

	uint16_t vendor_id;
	uint16_t product_id;

	char *manufacturer;
	char *name;
	char *serial_number;

	int *specializations;
	int number_of_specializations;

	void (*data_read_cb)(struct usb_phdc_device *dev, unsigned char* buffer, int size);
	void (*error_read_cb)(struct usb_phdc_device *dev);
	void (*device_gone_cb)(struct usb_phdc_device *dev);

	int ok;

} usb_phdc_device;

typedef struct usb_phdc_context {
	libusb_context *usb_context;
	usb_phdc_device *device_list;
	int number_of_devices;
} usb_phdc_context;

int init_phdc_usb_plugin(usb_phdc_context *phdc_context,
			void (*added_fd)(int fd, short events, void *ctx),
			void (*removed_fd)(int fd, void *ctx),
			void (*sch_timeout)(int ms));

void search_phdc_devices(usb_phdc_context *phdc_context);

int usb_send_apdu(usb_phdc_device *phdc_device, unsigned char *data, int len);

int open_phdc_handle(usb_phdc_device *phdc_device, usb_phdc_context *ctx);

void release_phdc_resources(usb_phdc_context *phdc_context);

void print_phdc_info(usb_phdc_device *phdc_device);

void poll_phdc_device_pre(usb_phdc_device *phdc_device);

int poll_phdc_device(usb_phdc_device *phdc_device, usb_phdc_context *ctx);

void poll_phdc_device_post(usb_phdc_context *ctx);

void phdc_device_fd_event(usb_phdc_context *ctx);

void phdc_device_timeout_event(usb_phdc_context *ctx);

#endif /*USB_PHDC_DRIVE_*/
