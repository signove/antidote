/**
 * \file usb_phdc_definitions.h
 * \brief Definitions for USB PHDC devices
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

#ifndef USB_PHDC_DEFINITIONS_
#define USB_PHDC_DEFINITIONS_

const uint8_t PHDC_INTERFACE_CLASS = 0x0F;
/*
 *	PHDC Descriptor Types
 */
const uint8_t PHDC_CLASSFUNCTION_DESCRIPTOR = 0x20;
const uint8_t PHDC_QOS_DESCRIPTOR = 0x21;
const uint8_t PHDC_METADATA_DESCRIPTOR = 0x22;
const uint8_t PHDC_11073PHD_FUNCTION_DESCRIPTOR = 0x30;

/*
 * PHDC Request Types
 */
const uint8_t GET_STATUS = 0x00;
const uint8_t CLEAR_FEATURE = 0x01;
const uint8_t SET_FEATURE = 0x03;

/*
 * PHDC Feature Types
 */
const uint8_t FEATURE_PHDC_QOS = 0x01;

/*
 *	Personal Healthcare Data and Messaging Formats
 */
const uint8_t PHDC_VENDOR = 0x01;
const uint8_t PHDC_11073_20601 = 0x02;

#endif /*USB_PHDC_DEFINITIONS_*/
