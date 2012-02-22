#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>
#include <time.h>
#include <glib.h>
#include <gio/gio.h>

#include <ieee11073.h>
#include <communication/plugin/plugin_bluez.h>
#include <specializations/pulse_oximeter.h>
#include <agent.h>

static const intu8 AGENT_SYSTEM_ID_VALUE[] = { 0x11, 0x33, 0x55, 0x77, 0x99,
		0xbb, 0xdd, 0xff};

static int oximeter_specialization = 0x0190;
guint16 hdp_data_types[] = {0x1004, 0x00};

static CommunicationPlugin comm_plugin = COMMUNICATION_PLUGIN_NULL;
static PluginBluezListener bluez_listener;
static GMainLoop *mainloop = NULL;

static int alarms = 0;
static guint alrm_handle = 0;

static ContextId cid;

static void app_clean_up()
{
	g_main_loop_unref(mainloop);
}

static void timer_reset_timeout(Context *ctx)
{
	if (ctx->timeout_action.id) {
		g_source_remove(ctx->timeout_action.id);
	}
}

static gboolean timer_alarm(gpointer data)
{
	Context *ctx = data;
	void (*f)() = ctx->timeout_action.func;
	f(ctx);
	return FALSE;
}

static int timer_count_timeout(Context *ctx)
{
	ctx->timeout_action.id = g_timeout_add(ctx->timeout_action.timeout
					       * 1000, timer_alarm, ctx);
	return ctx->timeout_action.id;
}

static gboolean sigalrm(gpointer data);

static void schedule_alarm(ContextId id, int to)
{
	if (alrm_handle) {
		g_source_remove(alrm_handle);
	}
	cid = id;
	alrm_handle = g_timeout_add(to * 1000, sigalrm, 0);
}

static gboolean sigalrm(gpointer data)
{
	fprintf(stderr, "==== alarm %d ====\n", alarms);

	alrm_handle = 0;
	
	if (alarms > 2) {
		agent_send_data(cid);
		schedule_alarm(cid, 3);
	} else if (alarms == 2) {
		agent_request_association_release(cid);
		schedule_alarm(cid, 2);
	} else if (alarms == 1) {
		agent_disconnect(cid);
		schedule_alarm(cid, 2);
	} else {
		g_main_loop_quit(mainloop);
	}

	--alarms;

	return FALSE;
}

void device_associated(Context *ctx)
{
	fprintf(stderr, "Associated\n");
	alarms = 5;
	schedule_alarm(ctx->id, 3);
}

void device_unavailable(Context *ctx)
{
	fprintf(stderr, "Disasociated\n");
	if (alarms > 2) {
		// involuntary; go straight to disconnection
		alarms = 1;
	}
}

void device_connected(Context *ctx)
{
	fprintf(stderr, "Connected\n");
	agent_associate(ctx->id);
}

static gboolean bt_connected(ContextId context, const char *btaddr)
{
	fprintf(stderr, "bt_connected %s\n", btaddr);
	return TRUE;
}

static gboolean bt_disconnected(ContextId context, const char *btaddr)
{
	fprintf(stderr, "bt_disconnected %s\n", btaddr);
	return TRUE;
}


static void *event_report_cb()
{
	time_t now;
	struct tm nowtm;
	struct oximeter_event_report_data* data =
		malloc(sizeof(struct oximeter_event_report_data));

	time(&now);
	localtime_r(&now, &nowtm);

	data->beats = 60.5 + random() % 20;
	data->oximetry = 90.5 + random() % 10;
	data->century = nowtm.tm_year / 100 + 19;
	data->year = nowtm.tm_year % 100;
	data->month = nowtm.tm_mon + 1;
	data->day = nowtm.tm_mday;
	data->hour = nowtm.tm_hour;
	data->minute = nowtm.tm_min;
	data->second = nowtm.tm_sec;
	data->sec_fractions = 50;

	return data;
}

static struct mds_system_data *mds_data_cb()
{
	struct mds_system_data* data = malloc(sizeof(struct mds_system_data));
	memcpy(&data->system_id, AGENT_SYSTEM_ID_VALUE, 8);
	return data;
}

int main(int argc, char **argv)
{
	g_type_init();

	comm_plugin = communication_plugin();

	if (argc != 2) {
		fprintf(stderr, "Usage: sample_bt_agent <bdaddr>\n");
		exit(1);
	}

	fprintf(stderr, "\nIEEE 11073 sample agent\n");

	comm_plugin.timer_count_timeout = timer_count_timeout;
	comm_plugin.timer_reset_timeout = timer_reset_timeout;

	CommunicationPlugin *plugins[] = {&comm_plugin, 0};

	plugin_bluez_setup(&comm_plugin);

	bluez_listener.peer_connected = bt_connected;
	bluez_listener.peer_disconnected = bt_disconnected;
	plugin_bluez_set_listener(&bluez_listener);

	agent_init(plugins, oximeter_specialization,
			event_report_cb, mds_data_cb);

	AgentListener listener = AGENT_LISTENER_EMPTY;
	listener.device_connected = &device_connected;
	listener.device_associated = &device_associated;
	listener.device_unavailable = &device_unavailable;

	agent_add_listener(listener);

	agent_start();

	plugin_bluez_update_data_types(FALSE, hdp_data_types); // FALSE=source

	if (!plugin_bluez_connect(argv[1], hdp_data_types[0], HDP_CHANNEL_RELIABLE)) {
		exit(1);
	}

	alarms = 0;

	fprintf(stderr, "Main loop started\n");
	mainloop = g_main_loop_new(NULL, FALSE);
	g_main_loop_ref(mainloop);
	g_main_loop_run(mainloop);
	fprintf(stderr, "Main loop stopped\n");

	agent_finalize();
	app_clean_up();

	return 0;
}
