/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * \file dim.c
 * \brief Domain Information Model implementation.
 *
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
 * \date Jun 10, 2010
 * \author Fabricio Silva
 */

/**
 * @defgroup ObjectClasses Object Classes
 * @brief The DIM characterizes information from an agent as a set of objects. Each
 * object has one or more attributes. Attributes describe measurement data that are
 * communicated to a manager as well as elements that control behavior and report
 * on the status of the agent.
 *
 * Personal health devices, within this standard, are defined using an object-oriented
 * model. This DIM defines several classes for modeling an agent. The model describes
 * an agent device as a set of objects that represent the data sources, as the elements
 * that a manager can use to control the behavior of the agent, and as the mechanism the
 * agent uses to report updates to the status of agent representation. Agent device objects
 * have attributes that represent information and status for the object.
 *
 * Manager devices communicate with agent device objects through the use of well-defined
 * methods, such as GET and SET, and are defined in each subclause describing an object.
 * Information, such as measurements, is sent from agent data objects to the manager
 * device using event reports. The information model for the domain of personal health
 * devices is an object-oriented model defining data objects of agents, including their
 * attributes and methods.
 *
 * The objects derived from classes defined in the information model represent all
 * data that an agent system can communicate to a manager system by means of the
 * application protocol defined in this standard. Such data are modeled in the
 * form of object attributes. Furthermore, the information model defines specific data
 * access services in the form of methods that are used for data exchange between
 * agent and manager systems. These services model the application protocol messages
 * (data access primitives) defined in this standard.
 *
 * Objects define the structure and the capabilities of the agent system. The manager
 * system accesses these objects to retrieve data and/or to control the agent system.
 * This standard does not define an information model of the manager system.
 *
 * The information model is a hierarchical model that defines the logical structure
 * and capability of a personal health device. At a top level, the MDS object
 * represents the properties and services of the device itself, independent of its health
 * data capabilities. Properties of the MDS object include attributes for device
 * identification and further technical descriptive and state data. The application-specific
 * data (e.g., health data and measurement data) provided by the personal health device
 * are modeled in the form of further information objects that are logically contained in
 * the MDS object. The set of object attributes, together with this containment relation,
 * describes the configuration and, as such, the capabilities of the personal health device.
 *
 * The following diagram uses Unified Modeling Language (UML) to represent the information
 * objects of a personal health agent along with class relationships.
 *
 * \image html dim_diagram.png
 *
 */
