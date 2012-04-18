#!/bin/bash

SCRIPT_DIR="$( cd -P "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
AGENT_EXEC=simulator_agent
MANAGER_EXEC=../apps/ieee_manager
IP_ADDRESS=$1
SENSOR_LIST='PulseOximeter BloodPressure WeightScale GlucoseMeter'

echo "Starting IEEE TCP Manager..."
$SCRIPT_DIR/$MANAGER_EXEC --tcp &

for s in $SENSOR_LIST
do
	echo "Starting sensor $s ..."
	$SCRIPT_DIR/$AGENT_EXEC --host $IP_ADDRESS --sensor $s &
done
