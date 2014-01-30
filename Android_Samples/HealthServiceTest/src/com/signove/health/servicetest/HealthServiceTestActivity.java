package com.signove.health.servicetest;

import android.app.Activity;
import com.signove.health.service.HealthAgentAPI;
import com.signove.health.service.HealthServiceAPI;
import android.widget.TextView;
import android.os.Handler;
import android.os.IBinder;
import android.content.ComponentName;
import java.io.IOException;
import java.io.ByteArrayInputStream;
import android.os.RemoteException;
import android.content.Intent;
import android.os.Bundle;
import android.view.View.OnLongClickListener;
import android.view.View;
import android.util.Log;
import android.content.ServiceConnection;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.w3c.dom.NamedNodeMap;
import org.xml.sax.SAXException;
import java.util.Map;
import java.util.HashMap;

public class HealthServiceTestActivity extends Activity {
	int [] specs = {0x1004};
	Handler tm;
	HealthServiceAPI api;

	TextView status;
	TextView msg;
	TextView device;
	TextView data;

	Map <String, String> map;
	
	public void show(TextView field, String msg)
	{
		final TextView f = field;
		final String s = msg;
		tm.post(new Runnable() {
			public void run() {
				f.setText(s);
			}
		});
	}

	public Document parse_xml(String xml)
	{
		Document d = null;

		try {
			DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
			DocumentBuilder db = dbf.newDocumentBuilder();
			d = db.parse(new ByteArrayInputStream(xml.getBytes("UTF-8")));
		} catch (ParserConfigurationException e) {
			Log.w("Antidote", "XML parser error");
		} catch (SAXException e) {
			Log.w("Antidote", "SAX exception");
		} catch (IOException e) {
			Log.w("Antidote", "IO exception in xml parsing");
		}

		return d;
	}

	public void show_dev(String path)
	{
		if (map.containsKey(path)) {
			show(device, "Device " + map.get(path));
		} else {
			show(device, "Unknown device " + path);
		}
	}

	public void handle_packet_connected(String path, String dev)
	{
		map.put(path, dev);
		show_dev(path);
		show(msg, "Connected");
	}

	public void handle_packet_disconnected(String path)
	{
		show(msg, "Disconnected");
		show_dev(path);
	}

	public void handle_packet_associated(String path, String xml)
	{
		show(msg, "Associated");
		show_dev(path);
	}

	public void handle_packet_disassociated(String path)
	{
		show(msg, "Disassociated");
		show_dev(path);
	}

	public void handle_packet_description(String path, String xml)
	{
		show(msg, "MDS received");
		show_dev(path);
	}

	public String get_xml_text(Node n) {
		String s = null;
		NodeList text = n.getChildNodes();

		for (int l = 0; l < text.getLength(); ++l) {
			Node txt = text.item(l);
			if (txt.getNodeType() == Node.TEXT_NODE) {
				if (s == null) {
					s = "";
				}
				s += txt.getNodeValue();							
			}
		}

		return s;
	}


	public void handle_packet_measurement(String path, String xml)
	{
		String measurement = "";
		Document d = parse_xml(xml);

		if (d == null) {
			return;
		}

		NodeList datalists = d.getElementsByTagName("data-list");

		for (int i = 0; i < datalists.getLength(); ++i) {

			Log.w("Antidote", "processing datalist " + i);

			Node datalist_node = datalists.item(i);
			NodeList entries = ((Element) datalist_node).getElementsByTagName("entry");

			for (int j = 0; j < entries.getLength(); ++j) {

				Log.w("Antidote", "processing entry " + j);

				boolean ok = false;
				String unit = "";
				String value = "";

				Node entry = entries.item(j);

				// scan immediate children to dodge entry inside another entry
				NodeList entry_children = entry.getChildNodes();

				for (int k = 0; k < entry_children.getLength(); ++k) {					
					Node entry_child = entry_children.item(k);

					Log.w("Antidote", "processing entry child " + entry_child.getNodeName());

					if (entry_child.getNodeName().equals("simple")) {
						// simple -> value -> (text)
						NodeList simple = ((Element) entry_child).getElementsByTagName("value");
						Log.w("Antidote", "simple.value count: " + simple.getLength());
						if (simple.getLength() > 0) {
							String text = get_xml_text(simple.item(0));
							if (text != null) {
								ok = true;
								value = text;
							}
						}
					} else if (entry_child.getNodeName().equals("meta-data")) {
						// meta-data -> meta name=unit
						NodeList metas = ((Element) entry_child).getElementsByTagName("meta");

						Log.w("Antidote", "meta-data.meta count: " + metas.getLength());

						for (int l = 0; l < metas.getLength(); ++l) {
							Log.w("Antidote", "Processing meta " + l);
							NamedNodeMap attr = metas.item(l).getAttributes();
							if (attr == null) {
								Log.w("Antidote", "Meta has no attributes");
								continue;
							}
							Node item = attr.getNamedItem("name");
							if (item == null) {
								Log.w("Antidote", "Meta has no 'name' attribute");
								continue;
							}

							Log.w("Antidote", "Meta attr 'name' is " + item.getNodeValue());

							if (item.getNodeValue().equals("unit")) {
								Log.w("Antidote", "Processing meta unit");
								String text = get_xml_text(metas.item(l));
								if (text != null) {
									unit = text;
								}
							}
						}

					}
				}

				if (ok) {
					if (unit != "")
						measurement += value + " " + unit + "\n";
					else
						measurement += value + " ";
				}
			}
		}

		show(data, measurement);
		show(msg, "Measurement");
		show_dev(path);	 
	}

	private void RequestConfig(String dev)
	{	
		try {
			Log.w("HST", "Getting configuration ");
			String xmldata = api.GetConfiguration(dev);
			Log.w("HST", "Received configuration");
			Log.w("HST", ".." + xmldata);
		} catch (RemoteException e) {
			Log.w("HST", "Exception (RequestConfig)");
		}
	}

	private void RequestDeviceAttributes(String dev)
	{	
		try {
			Log.w("HST", "Requested device attributes");
			api.RequestDeviceAttributes(dev);
		} catch (RemoteException e) {
			Log.w("HST", "Exception (RequestDeviceAttributes)");
		}
	}


	private HealthAgentAPI.Stub agent = new HealthAgentAPI.Stub() {
		@Override
		public void Connected(String dev, String addr) {
			Log.w("HST", "Connected " + dev);
			Log.w("HST", "..." + addr);
			handle_packet_connected(dev, addr);
		}

		@Override
		public void Associated(String dev, String xmldata) {
			final String idev = dev;
			Log.w("HST", "Associated " + dev);			
			Log.w("HST", "...." + xmldata);			
			handle_packet_associated(dev, xmldata);

			Runnable req1 = new Runnable() {
				public void run() {
					RequestConfig(idev);
				}
			};
			Runnable req2 = new Runnable() {
				public void run() {
					RequestDeviceAttributes(idev);
				}
			};
			tm.postDelayed(req1, 1); 
			tm.postDelayed(req2, 500); 
		}
		@Override
		public void MeasurementData(String dev, String xmldata) {
			Log.w("HST", "MeasurementData " + dev);
			Log.w("HST", "....." + xmldata);
			handle_packet_measurement(dev, xmldata);
		}
		@Override
		public void DeviceAttributes(String dev, String xmldata) {
			Log.w("HST", "DeviceAttributes " + dev);			
			Log.w("HST", ".." + xmldata);
			handle_packet_description(dev, xmldata);
		}

		@Override
		public void Disassociated(String dev) {
			Log.w("HST", "Disassociated " + dev);						
			handle_packet_disassociated(dev);
		}

		@Override
		public void Disconnected(String dev) {
			Log.w("HST", "Disconnected " + dev);
			handle_packet_disconnected(dev);
		}
	};

	private ServiceConnection serviceConnection = new ServiceConnection() {
		@Override
		public void onServiceConnected(ComponentName name, IBinder service) {
			Log.w("HST", "Service connection established");

			// that's how we get the client side of the IPC connection
			api = HealthServiceAPI.Stub.asInterface(service);
			try {
				Log.w("HST", "Configuring...");
				api.ConfigurePassive(agent, specs);
			} catch (RemoteException e) {
				Log.e("HST", "Failed to add listener", e);
			}
		}

		@Override
		public void onServiceDisconnected(ComponentName name) {
			Log.w("HST", "Service connection closed");
		}
	};


	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		setContentView(R.layout.main);
		
		OnLongClickListener l = new OnLongClickListener() {
		    public boolean onLongClick(View v) {
		    	finish();
		    	return true;
		    }
		};
		
		status = (TextView) findViewById(R.id.status);
		msg = (TextView) findViewById(R.id.msg);
		device = (TextView) findViewById(R.id.device);
		data = (TextView) findViewById(R.id.data);

		status.setOnLongClickListener(l);
		msg.setOnLongClickListener(l);
		device.setOnLongClickListener(l);
		data.setOnLongClickListener(l);

		map = new HashMap<String, String>();

		tm = new Handler();
		Intent intent = new Intent("com.signove.health.service.HealthService");
		startService(intent);
		bindService(intent, serviceConnection, 0);
		Log.w("HST", "Activity created");

		status.setText("Ready");
		msg.setText("--");
		device.setText("--");
		data.setText("--");
	}

	@Override
	public void onDestroy()
	{
		super.onDestroy();
		try {
			Log.w("HST", "Unconfiguring...");
			api.Unconfigure(agent);
		} catch (Throwable t) {
			Log.w("HST", "Erro tentando desconectar");
		}
		Log.w("HST", "Activity destroyed");
		unbindService(serviceConnection);

	}
}
