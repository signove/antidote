package com.signove.oss.antidoteclient;

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;
import android.os.Handler;
import java.lang.Runnable;
import java.net.Socket;
import java.io.IOException;
import java.io.ByteArrayInputStream;
import java.net.UnknownHostException;
import android.util.Log;
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

public class AntidoteClient extends Activity {
	TextView status;
	TextView msg;
	TextView device;
	TextView data;
	Handler tm;
	Socket sk;
	String buffer;
	Map <String, String> map;
	String ADDRESS = "127.0.0.1";
	int PORT = 9005;
	boolean running = true;
	
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
			device.setText("Device " + map.get(path));
		} else {
			device.setText("Unknown device " + path);
		}
	}
	
	public void handle_packet_connected(String path, String dev)
	{
		map.put(path, dev);
		show_dev(path);
		msg.setText("Connected");
	}
	
	public void handle_packet_disconnected(String path, String dummy)
	{
		msg.setText("Disconnected");
		show_dev(path);
	}
	
	public void handle_packet_associated(String path, String dummy)
	{
		msg.setText("Associated");
		show_dev(path);
	}

	public void handle_packet_disassociated(String path, String dummy)
	{
		msg.setText("Disassociated");
		show_dev(path);
	}
	
	public void handle_packet_description(String path, String xml)
	{
		msg.setText("Description");
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
					measurement += value + " " + unit + "\n";
				}
			}
		}

		data.setText(measurement);
		msg.setText("Measurement");
		show_dev(path);	 
	}

	public String extract_packet()
	{
		String packet = null;
		int pos;
		
		while ((pos = buffer.indexOf('\n')) >= 0 && packet == null) {
			if (pos > 0) {
				packet = buffer.substring(0, pos);
			}
			buffer = buffer.substring(pos + 1);
		}
		
		return packet;
	}
	
	public String[] tokenize_packet(String packet)
	{
		int pos1 = packet.indexOf('\t');
		int pos2 = packet.indexOf('\t', pos1 + 1);
		
		Log.w("Antidote", "token tabs: " + pos1 + " " + pos2);
		
		if (pos1 < 0 || pos2 <= pos1) {
			return null;
		}
		
		String[] ret = new String[3];
		ret[0] = packet.substring(0, pos1);
		ret[1] = packet.substring(pos1 + 1, pos2);
		ret[2] = packet.substring(pos2 + 1);
		
		return ret;
	}

	public void new_data()
	{
        Log.w("Antidote", "New data");
        
        String packet = extract_packet();
        if (packet == null) {
        	Log.w("Antidote", "no message yet");
        	return;
        }
        
        String[] tokens = tokenize_packet(packet);
        if (tokens == null) {
        	Log.w("Antidote", "could not tokenize");
        	return;
        }

        if (tokens[0].equals("CONNECTED")) {   
        	handle_packet_connected(tokens[1], tokens[2]);
        } else if (tokens[0].equals("ASSOCIATED")) {
        	handle_packet_associated(tokens[1], tokens[2]);
        } else if (tokens[0].equals("DESCRIPTION")) {
        	handle_packet_description(tokens[1], tokens[2]);
        } else if (tokens[0].equals("MEASUREMENT")) {
        	handle_packet_measurement(tokens[1], tokens[2]);
        } else if (tokens[0].equals("DISASSOCIATE")) {
        	handle_packet_disassociated(tokens[1], tokens[2]);
        } else if (tokens[0].equals("DISCONNECT")) {
        	handle_packet_disconnected(tokens[1], tokens[2]);
        } else {
        	Log.w("Antidote", "Invalid message command");
        }
	}
	
	public void connect_error(String err) {
		Log.w("Antidote", "connection error");
		status.setText(err + "... trying again soon");
		
	    Runnable do_connect = new Runnable() {
	     	public void run() {
	        	connect();
	        }
	    };
	        
	   tm.postDelayed(do_connect, 10000); 
	}
	
	public void disconnected() {
		status.setText("Disconnected");
        Log.w("Antidote", "Disconnected");
		
	    Runnable do_connect = new Runnable() {
	     	public void run() {
	        	connect();
	        }
	    };
	        
	   tm.postDelayed(do_connect, 10000); 
	}
		
	public void connect() {
        Log.w("Antidote", "Connecting");
        
        try {
        	sk = new Socket(ADDRESS, PORT);
        } catch (UnknownHostException e) {
        	connect_error("Unknown host");
        	return;
        } catch (IOException e) {
        	connect_error("Network error");
        	return;
        }
        
        status.setText("Connected to service");
        
        Runnable handle_sk = new Runnable() {
        	public void run() {
        		byte[] data = new byte[256];
        		int len;

        		while (running) {
        			Log.w("Antidote", "Reading...");
        		
        			try {
        				len = sk.getInputStream().read(data);
        			} catch (IOException e) {
        				len = -1;
        			}
        			
        			if (! running)
        				break;
        		
        			if (len <= 0) {
        				Runnable r = new Runnable() {
        					public void run() {
        						Log.w("Antidote", "Disconn runnable");
        						disconnected();
        					}
        				};
             
        				tm.postDelayed(r, 0);
        				break;
        			}
        		
        			buffer = buffer + new String(data, 0, len);

        			Runnable r = new Runnable() {
        				public void run() {
        					Log.w("Antidote", "New data runnable");
        					new_data();
        				}
        			};
        			
                    tm.postDelayed(r, 0);
                }
            
        		try {
        			sk.close();
        		} catch (Exception e) {
        	
        		}
                Log.w("Antidote", "Thread end");
        	}
        }; 

        Log.w("Antidote", "Connect: creating thread");   
        new Thread(handle_sk).start();

        Log.w("Antidote", "Connect end");
	}
	
	@Override
	public void onDestroy()
	{
		Log.w("Antidote", "Destroying activity");
		running = false;
		try {
			sk.close();
		} catch (Exception e) {
		
		}
		super.onDestroy();
	}
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        setContentView(R.layout.main);
        status = (TextView) findViewById(R.id.status);
        msg = (TextView) findViewById(R.id.msg);
        device = (TextView) findViewById(R.id.device);
        data = (TextView) findViewById(R.id.data);
        
        buffer = "";
        map = new HashMap<String, String>();
        
        status.setText("Initializing...");
        tm = new Handler();
        
        Runnable do_connect = new Runnable() {
        	public void run() {
        		connect();
        	}
        };
        
        tm.postDelayed(do_connect, 1000);
        
        Log.w("Antidote", "onCreate");
    }
}