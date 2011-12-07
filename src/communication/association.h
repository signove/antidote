/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file association.h
 * \brief Association module header.
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
 * \date Jun 23, 2010
 */

#ifndef ASSOCIATING_H_
#define ASSOCIATING_H_

/**
 * \defgroup Association Association
 * \ingroup FSM
 * @{
 */

#include "src/asn1/phd_types.h"
#include "src/communication/fsm.h"


int association_accept_data_protocol_20601_in(Context *ctx,
					PhdAssociationInformation agent_assoc_information,
					int trans);

void association_unassociated_process_apdu(Context *ctx, APDU *apdu);

void association_unassociated_process_apdu_agent(Context *ctx, APDU *apdu);

void association_accept_config_tx(Context *ctx, fsm_events evt,
				  FSMEventData *data);

void association_aarq_tx(FSMContext *ctx, fsm_events evt, FSMEventData *data);

void association_agent_aare_rejected_permanent_tx(FSMContext *ctx, fsm_events evt, FSMEventData *data);

void association_unaccept_config_tx(Context *ctx, fsm_events evt,
				    FSMEventData *data);

/** @} */

#endif /* ASSOCIATING_H_ */
