package com.signove.health.healthservice;

import com.signove.health.healthservice.HealthService;

public class JniBridge {
	HealthService cb;

	JniBridge(HealthService pcb) {
		cb = pcb;
		healthd_init();
	}

	~JniBridge() {
		healthd_finalize();
	}

	// communication part: called from C
	public void disconnect_channel(int context)
	{
		cb.disconnect_channel(context);
	}

	public void send_data(int context, byte [] data)
	{
		cb.send_data(context, data);
	}

	// communication part: called from service
	public native channel_connected(int context);
	public native channel_disconnected(int context);
	public native data_received(int context, byte [] data);

	// manager part: called from C
	public void cancel_timer(int handle)
	{
		cb.cancel_timer(handle);
	}

	public int create_timer(int milisseconds, int handle)
	{
		return cb.create_timer(milisseconds, handle);
	}

	public void associated(int context, String xml)
	{
		cb.associated(context, xml);
	}

	public void disassociated(int context)
	{
		cb.disassociated(context);
	}

	public void deviceattributes(int context, String xml)
	{
		cb.deviceattributes(context, xml);
	}

	public void measurementdata(int context, String xml)
	{
		cb.measurementdata(context, xml);
	}

	// FIXME implement PM-Store calls

	// manager part: called from service
	public native void timer_alarm(int handle);
	public native void healthd_init();
	public native void healthd_finalize();
	public native void releaseassoc(int context);
	public native void abortassoc(int context);
	public native String getconfig(int context);
	public native void reqmdsattr(int context);
	public native void reqactivationscanner(int context, int handle);
	public native void reqdeactivationscanner(int context, int handle);
	public native void reqmeasurement(int context);

	// FIXME implement PM-Store calls

	static {
		System.loadLibrary("healthd");
	}
};
