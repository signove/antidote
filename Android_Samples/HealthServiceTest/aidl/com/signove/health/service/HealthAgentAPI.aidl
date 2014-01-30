package com.signove.health.service;

interface HealthAgentAPI {
	void Connected(String dev, String addr);
	void Associated(String dev, String xmldata);
	void MeasurementData(String dev, String xmldata);
	void DeviceAttributes(String dev, String xmldata);
	void Disassociated(String dev);
	void Disconnected(String dev);
}
