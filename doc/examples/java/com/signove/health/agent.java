/*
 * Copyright (c) 2012 Itsaso Aranburu <itsasoaranburu@gmail.com>,
 *	 	      Signove Tecnologia S/A
 */

package com.signove.health;

import org.freedesktop.dbus.*;

@DBusInterfaceName("com.signove.health.agent")
public interface agent extends DBusInterface {
  	void Connected(String dev, String addr);
	void Associated(String dev, String data);
	void MeasurementData(String dev, String data);
	void DeviceAttributes(String dev, String data);
	void Disassociated(String dev);
	void Disconnected(String dev);
}
