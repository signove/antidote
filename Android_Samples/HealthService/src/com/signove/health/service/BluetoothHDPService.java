/*
 * Copyright (C) 2012 Signove Tecnologia
 * Based on Bluetooth HDP example from Android SDK
 * Copyright (C) 2011 The Android Open Source Project
 */

package com.signove.health.service;

import android.app.Service;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothHealth;
import android.bluetooth.BluetoothHealthAppConfiguration;
import android.bluetooth.BluetoothHealthCallback;
import android.bluetooth.BluetoothProfile;
import android.content.Intent;
import java.util.HashMap;
import java.util.ArrayList;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.ParcelFileDescriptor;
import android.os.RemoteException;
import android.util.Log;
import java.util.List;


import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

public class BluetoothHDPService extends Service {
	private static final String TAG = "HSSHDP";

	public static final int RESULT_OK = 0;
	public static final int RESULT_FAIL = -1;

	// Status codes sent back to the UI client.
	// Application registration complete.
	public static final int STATUS_HEALTH_APP_REG = 100;
	// Application unregistration complete.
	public static final int STATUS_HEALTH_APP_UNREG = 101;
	// Channel creation complete.
	public static final int STATUS_CREATE_CHANNEL = 102;
	// Channel destroy complete.
	public static final int STATUS_DESTROY_CHANNEL = 103;
	// Reading data from Bluetooth HDP device.
	public static final int STATUS_READ_DATA = 104;
	// Done with reading data.
	public static final int STATUS_READ_DATA_DONE = 105;

	// Message codes received from the client.
	// Register client with this service.
	public static final int MSG_REG_CLIENT = 200;
	// Unregister client from this service.
	public static final int MSG_UNREG_CLIENT = 201;
	// Register health application.
	public static final int MSG_REG_HEALTH_APP = 300;
	// Unregister health application.
	public static final int MSG_UNREG_HEALTH_APP = 301;
	// Connect channel.
	public static final int MSG_CONNECT_CHANNEL = 400;
	// Disconnect channel.
	public static final int MSG_DISCONNECT_CHANNEL = 401;
	public static final int MSG_SEND_DATA = 501;

	private List<BluetoothHealthAppConfiguration> configs =
						new ArrayList<BluetoothHealthAppConfiguration>();
	
	// TODO this allows just one data type per device
	private HashMap<BluetoothDevice, BluetoothHealthAppConfiguration> deviceconfigs =
						new HashMap<BluetoothDevice, BluetoothHealthAppConfiguration>();
	
	private BluetoothAdapter mBluetoothAdapter;
	private BluetoothHealth mBluetoothHealth;

	private Messenger mClient;
	
	HashMap<BluetoothDevice, Integer> channelIds = new HashMap<BluetoothDevice, Integer>();
	HashMap<BluetoothDevice, FileOutputStream> writers = new HashMap<BluetoothDevice, FileOutputStream>();
	
	private boolean acceptsConfiguration(BluetoothHealthAppConfiguration config)
	{
		return configs.contains(config);
	}
	
	private BluetoothHealthAppConfiguration getDeviceConfiguration(BluetoothDevice dev)
	{
		if (! deviceconfigs.containsKey(dev)) {
			if (configs.size() > 0) {
				return configs.get(0);
			} else {
				return null;
			}
		}
		return deviceconfigs.get(dev);
	}
	
	private synchronized void insertDeviceConfiguration(BluetoothDevice dev,
										BluetoothHealthAppConfiguration config)
	{
		deviceconfigs.put(dev, config);
	}
	
	private int getChannelId(BluetoothDevice dev)
	{
		if (! channelIds.containsKey(dev)) {
			Log.w(TAG, "No channel id for dev " + dev.getAddress());
			return 1;
		}
		return channelIds.get(dev);
	}
	
	private synchronized void insertChannelId(BluetoothDevice dev, int channelId)
	{
		channelIds.put(dev, channelId);
	}

	private FileOutputStream getWriter(BluetoothDevice dev)
	{
		if (! writers.containsKey(dev)) {
			Log.w(TAG, "No writer fd for dev " + dev.getAddress());
			return null;
		}
		return writers.get(dev);
	}
	
	private synchronized void insertWriter(BluetoothDevice dev, FileOutputStream writer)
	{
		writers.put(dev, writer);
	}
	
	private synchronized void removeWriter(BluetoothDevice dev)
	{
		FileOutputStream writer = getWriter(dev);
		if (writer != null) {
			try {
				writer.close();
			} catch (IOException e) {}
			writers.remove(dev);			
		}
	}

	// Handles events sent by {@link HealthHDPActivity}.
	private class IncomingHandler extends Handler {
		@Override
		public void handleMessage(Message msg) {
			switch (msg.what) {
			// Register UI client to this service so the client can receive messages.
			case MSG_REG_CLIENT:
				Log.w(TAG, "Activity client registered");
				mClient = msg.replyTo;
				break;
				// Unregister UI client from this service.
			case MSG_UNREG_CLIENT:
				mClient = null;
				break;
				// Register health application.
			case MSG_REG_HEALTH_APP:
				registerApp(msg.arg1);
				break;
				// Unregister health application.
			case MSG_UNREG_HEALTH_APP:
				unregisterApp();
				break;
				// Connect channel.
			case MSG_CONNECT_CHANNEL:
				connectChannel((BluetoothDevice) msg.obj);
				break;
				// Send data
			case MSG_SEND_DATA:
				Object[] pair = (Object []) msg.obj;
				sendData((BluetoothDevice) pair[0], (byte []) pair[1]);
				break;
				// Disconnect channel.
			case MSG_DISCONNECT_CHANNEL:
				disconnectChannel((BluetoothDevice) msg.obj);
				break;
			default:
				super.handleMessage(msg);
			}
		}
	}

	final Messenger mMessenger = new Messenger(new IncomingHandler());

	/**
	 * Make sure Bluetooth and health profile are available on the Android device.  Stop service
	 * if they are not available.
	 */
	@Override
	public void onCreate() {
		super.onCreate();
		mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
		if (mBluetoothAdapter == null || !mBluetoothAdapter.isEnabled()) {
			// Bluetooth adapter isn't available.  The client of the service is supposed to
			// verify that it is available and activate before invoking this service.
			stopSelf();
			return;
		}
		if (!mBluetoothAdapter.getProfileProxy(this, mBluetoothServiceListener,
				BluetoothProfile.HEALTH)) {
			Log.w(TAG, "Bluetooth profile not available");
			stopSelf();
			return;
		}
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		Log.w(TAG, "BluetoothHDPService is running.");
		return START_STICKY;
	}

	@Override
	public IBinder onBind(Intent intent) {
		return mMessenger.getBinder();
	};

	// Register health application through the Bluetooth Health API.
	private void registerApp(int dataType) {
		mBluetoothHealth.registerSinkAppConfiguration(TAG, dataType, mHealthCallback);
	}

	// Unregister health application through the Bluetooth Health API.
	private void unregisterApp() {
		for (BluetoothHealthAppConfiguration app: configs) {
		    mBluetoothHealth.unregisterAppConfiguration(app);
		}
	}

	// Connect channel through the Bluetooth Health API.
	private void connectChannel(BluetoothDevice dev) {
		Log.w(TAG, "connectChannel()");
		BluetoothHealthAppConfiguration app = getDeviceConfiguration(dev);
		if (app != null)
			mBluetoothHealth.connectChannelToSource(dev, app);
	}

	// Send data
	private void sendData (BluetoothDevice dev, byte [] data) {
		Log.w(TAG, "HDP sending data");
		FileOutputStream writer = getWriter(dev);
		if (writer == null) {
			Log.w(TAG, "Could not send data to HDP (no stream)");
			return;
		}
		try {
			writer.write(data);
			Log.w(TAG, "HDP sent data " + data.length);
		} catch (IOException e) {
			removeWriter(dev);
			Log.w(TAG, "Could not send data to HDP");
		}
	}

	// Disconnect channel through the Bluetooth Health API.
	private void disconnectChannel(BluetoothDevice dev) {
		Log.w(TAG, "disconnectChannel()");
		BluetoothHealthAppConfiguration app = getDeviceConfiguration(dev);
		if (app != null)
			mBluetoothHealth.disconnectChannel(dev, app, getChannelId(dev));
	}

	// Callbacks to handle connection set up and disconnection clean up.
	private final BluetoothProfile.ServiceListener mBluetoothServiceListener =
		new BluetoothProfile.ServiceListener() {
		public void onServiceConnected(int profile, BluetoothProfile proxy) {
			if (profile == BluetoothProfile.HEALTH) {
				mBluetoothHealth = (BluetoothHealth) proxy;
				Log.w(TAG, "onServiceConnected to profile: " + profile);
			}
		}

		public void onServiceDisconnected(int profile) {
			if (profile == BluetoothProfile.HEALTH) {
				mBluetoothHealth = null;
			}
		}
	};

	private final BluetoothHealthCallback mHealthCallback = new BluetoothHealthCallback() {
		// Callback to handle application registration and unregistration events.  The service
		// passes the status back to the UI client.
		public void onHealthAppConfigurationStatusChange(BluetoothHealthAppConfiguration config,
				int status) {
			if (status == BluetoothHealth.APP_CONFIG_REGISTRATION_FAILURE) {
				sendMessage(STATUS_HEALTH_APP_REG, RESULT_FAIL, null);
			} else if (status == BluetoothHealth.APP_CONFIG_REGISTRATION_SUCCESS) {
				configs.add(config);
				sendMessage(STATUS_HEALTH_APP_REG, RESULT_OK, null);
			} else if (status == BluetoothHealth.APP_CONFIG_UNREGISTRATION_FAILURE ||
					status == BluetoothHealth.APP_CONFIG_UNREGISTRATION_SUCCESS) {
				sendMessage(STATUS_HEALTH_APP_UNREG,
						status == BluetoothHealth.APP_CONFIG_UNREGISTRATION_SUCCESS ?
								RESULT_OK : RESULT_FAIL,
								null);
			}
		}

		// Callback to handle channel connection state changes.
		// Note that the logic of the state machine may need to be modified based on the HDP device.
		// When the HDP device is connected, the received file descriptor is passed to the
		// ReadThread to read the content.
		public void onHealthChannelStateChange(BluetoothHealthAppConfiguration config,
				BluetoothDevice device, int prevState, int newState, ParcelFileDescriptor fd,
				int channelId) {
			Log.w(TAG, String.format("prevState\t%d ----------> newState\t%d",
					prevState, newState));
			if (prevState == BluetoothHealth.STATE_CHANNEL_DISCONNECTED &&
					newState == BluetoothHealth.STATE_CHANNEL_CONNECTED) {
				if (acceptsConfiguration(config)) {
					insertDeviceConfiguration(device, config);
					insertChannelId(device, channelId);
					sendMessage(STATUS_CREATE_CHANNEL, RESULT_OK, device);
					FileOutputStream wr = new FileOutputStream(fd.getFileDescriptor());
					insertWriter(device, wr);
					(new ReadThread(device, fd)).start();
				} else {
					sendMessage(STATUS_CREATE_CHANNEL, RESULT_FAIL, device);
				}
			} else if (prevState == BluetoothHealth.STATE_CHANNEL_CONNECTING &&
					newState == BluetoothHealth.STATE_CHANNEL_DISCONNECTED) {
				sendMessage(STATUS_CREATE_CHANNEL, RESULT_FAIL, device);
				removeWriter(device);
			} else if (newState == BluetoothHealth.STATE_CHANNEL_DISCONNECTED) {
				if (acceptsConfiguration(config)) {
					sendMessage(STATUS_DESTROY_CHANNEL, RESULT_OK, device);
					removeWriter(device);
				} else {
					sendMessage(STATUS_DESTROY_CHANNEL, RESULT_FAIL, device);
					removeWriter(device);
				}
			}
		}
	};

	// Sends an update message to registered client.
	private void sendMessage(int what, int value, Object obj) {
		if (mClient == null) {
			Log.w(TAG, "No clients registered.");
			return;
		}

		try {
			Message msg = Message.obtain(null, what, value);
			msg.obj = obj;
			mClient.send(msg);
		} catch (RemoteException e) {
			// Unable to reach client.
			e.printStackTrace();
		}
	}

	private class ReadThread extends Thread {
		private ParcelFileDescriptor mFd;
		private BluetoothDevice mDev;

		public ReadThread(BluetoothDevice dev, ParcelFileDescriptor fd) {
			super();
			mFd = fd;
			mDev = dev;
		}

		@Override
		public void run() {
			FileInputStream fis = new FileInputStream(mFd.getFileDescriptor());
			final byte data[] = new byte[8192];
			try {
				int len;
				while ((len = fis.read(data)) > -1) {
					if (len > 0) {
						byte [] buf = new byte[len];
						System.arraycopy(data, 0, buf, 0, len);
						Log.w(TAG, "Reader thread");
						Object[] pair = new Object[2];
						pair[0] = mDev;
						pair[1] = buf;
						sendMessage(STATUS_READ_DATA, 0, pair);
					}
				}
			} catch (IOException ioe) {}
			if (mFd != null) {
				try {
					mFd.close();
				} catch (IOException e) {}
			}
			removeWriter(mDev);
			sendMessage(STATUS_READ_DATA_DONE, 0, mDev);
		}
	}
}
