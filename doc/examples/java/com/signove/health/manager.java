/*
 * Copyright (c) 2012 Itsaso Aranburu <itsasoaranburu@gmail.com>,
 *	 	      Signove Tecnologia S/A
 */

package com.signove.health;

import java.io.*;
import org.freedesktop.dbus.DBusConnection;
import org.freedesktop.dbus.DBusInterface;
import org.freedesktop.dbus.DBusInterfaceName;
import org.freedesktop.dbus.exceptions.DBusException;

@DBusInterfaceName("com.signove.health.manager")
public interface manager extends DBusInterface {
  	void ConfigurePassive(agent agt, int data_types[]);
	void Configure(agent agt, String addr, int data_types[]);
}
