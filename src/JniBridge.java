package com.signove.health.service;

import com.signove.health.service.HealthService;

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
	public synchronized void channel_connected(int context) {
		Cchannel_connected(context);
	}

	public synchronized void channel_disconnected(int context) {
		Cchannel_disconnected(context);
	}

	public synchronized void data_received(int context, byte [] data)
	{
		Cdata_received(context);
	}

	public native void Cchannel_connected(int context);
	public native void Cchannel_disconnected(int context);
	public native void Cdata_received(int context, byte [] data);

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
	public synchronized void timer_alarm(int handle)
	{
		Ctimer_alarm(handle);
	}

	public synchronized void healthd_init()
	{
		Chealthd_init();
	}

	public synchronized void healthd_finalize()
	{
		Chealthd_finalize();
	}

	public synchronized void releaseassoc(int context)
	{
		Creleaseassoc(context);
	}

	public synchronized void abortassoc(int context)
	{
		Cabortassoc(context);
	}

	public synchronized String getconfig(int context)
	{
		return Cgetconfig(context);
	}

	public synchronized void reqmdsattr(int context)
	{
		Creqmdsattr(context);
	}

	public synchronized void reqactivationscanner(int context, int handle)
	{
		Creqactivationscanner(context, handle);
	}

	public synchronized void reqdeactivationscanner(int context, int handle)
	{
		Creqdeactivationscanner(context, handle);
	}

	public synchronized void reqmeasurement(int context)
	{
		Creqmeasurement(context);
	}

	public native void Ctimer_alarm(int handle);
	public native void Chealthd_init();
	public native void Chealthd_finalize();
	public native void Creleaseassoc(int context);
	public native void Cabortassoc(int context);
	public native String Cgetconfig(int context);
	public native void Creqmdsattr(int context);
	public native void Creqactivationscanner(int context, int handle);
	public native void Creqdeactivationscanner(int context, int handle);
	public native void Creqmeasurement(int context);

	// FIXME implement PM-Store calls

	static {
		System.loadLibrary("healthd");
	}
};
