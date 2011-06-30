/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file disassociating.h
 * \brief Disassociating module header.
 * Copyright (C) 2010 Signove Tecnologia Corporation.
 * All rights reserved.
 * Contact: Signove Tecnologia Corporation (contact@signove.com)
 *
 * $LICENSE_TEXT:BEGIN$
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
 * $LICENSE_TEXT:END$
 *
 * \author Fabricio Silva
 * \date Jun 30, 2010
 */

/**
 * \defgroup Disassociating Disassociating
 * \ingroup FSM
 * @{
 */

#ifndef DISASSOCIATING_H_
#define DISASSOCIATING_H_

#include "src/asn1/phd_types.h"
#include "src/communication/communication.h"

void disassociating_process_apdu(Context *ctx, APDU *apdu);

void disassociating_process_apdu_agent(Context *ctx, APDU *apdu);

void disassociating_release_request_tx(Context *ctx, fsm_events evt,
				       FSMEventData *data);

void disassociating_release_request_normal_tx(Context *ctx, fsm_events evt,
						FSMEventData *data);

void disassociating_release_response_tx(Context *ctx, fsm_events evt,
					FSMEventData *data);

void disassociating_release_response_tx_normal(Context *ctx, fsm_events evt,
					FSMEventData *data);

void disassociating_release_proccess_completed(Context *ctx, fsm_events evt,
		FSMEventData *data);

/** @} */

#endif /* DISASSOCIATING_H_ */
