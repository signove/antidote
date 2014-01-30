package com.signove.health.service;

import com.signove.health.service.HealthAgentAPI;

interface HealthServiceAPI {
	void ConfigurePassive(HealthAgentAPI agt, in int[] specs);
	String GetConfiguration(String dev);
	void RequestDeviceAttributes(String dev);
	void Unconfigure(HealthAgentAPI agt);
}
