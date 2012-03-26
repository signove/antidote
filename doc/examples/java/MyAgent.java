/*
 * Copyright (c) 2012 Itsaso Aranburu <itsasoaranburu@gmail.com>,
 *	 	      Signove Tecnologia S/A
 */

import java.io.*;
import org.freedesktop.dbus.*;
import org.freedesktop.dbus.exceptions.DBusException;
import com.signove.health.*;

public class MyAgent implements agent {
	public boolean isRemote() { return false; }  

	public void Connected(String dev, String addr) {
		System.out.println("Connected dev " + dev);
		System.out.println("Connect addr " + addr);
	}

	public void Associated(String dev, String data) {
		System.out.println("Associated dev " + dev);
		System.out.println("Associated data " + data);
	}

	public void MeasurementData(String dev, String data) {
		System.out.println("MeasurementData dev " + dev);
		System.out.println("MeasurementData data " + data);
	}

	public void DeviceAttributes(String dev, String data){
		System.out.println("DeviceAttributes dev " + dev);
		System.out.println("DeviceAttributes data " + data);
	}

	public void Disassociated(String dev){
		System.out.println("Disassociated dev " + dev);
	}

	public void Disconnected(String dev) {
		System.out.println("Disconnected " + dev);
	}
}
