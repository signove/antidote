/*
 * Copyright (c) 2012 Itsaso Aranburu <itsasoaranburu@gmail.com>,
 *	 	      Signove Tecnologia S/A
 */

import java.io.*;
import org.freedesktop.dbus.DBusConnection;
import org.freedesktop.dbus.DBusInterface;
import org.freedesktop.dbus.DBusInterfaceName;
import org.freedesktop.dbus.exceptions.DBusException;
import com.signove.health.*;

public final class Main {
	public static void main(String[] args) throws IOException, DBusException {

		int data_types[] = {0x1004, 0x100, 0x1029, 0x100f};

		System.out.println("Creating D-Bus connection");
		DBusConnection conn = null;
		try {
			conn = DBusConnection.getConnection(DBusConnection.SYSTEM);
			System.out.println("DBusConnection conn: " +conn);
		} catch (DBusException DBe) {
			System.out.println("Impossible doing D-Bus connection");
		}


		// Manager object
		manager remoteObject;
                remoteObject = (manager) conn.getRemoteObject("com.signove.health","/com/signove/health", manager.class);

		boolean condition = false;

		//Agent object
		String agent_pid = "/com/signove/health/agent/" + ((int) (1 + Math.random() * 2000000000));
		System.out.println("agent_pid: " +agent_pid);
		agent agt = new MyAgent();

		conn.exportObject(agent_pid, agt);


		System.out.println("Configuring...");

		remoteObject.ConfigurePassive(agt, data_types);

		System.out.println("Waiting...");

		//while (!condition)
		{
			// Agent calls
			//agent.Associated(dev, xmldata);
			//String data = null;
			//agent.MeasurementData(dev, data);
			//agent.DeviceAttributes(dev, data);
              }
     }
}
