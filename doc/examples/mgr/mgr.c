#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <glib.h>
#include <gio/gio.h>

#include <ieee11073.h>
#include <communication/plugin/plugin_bluez.h>
#include <communication/service.h>

static PluginBluezListener bluez_listener;

static gboolean display_measurementdata(ContextId, char *);
static gboolean display_disassociated(ContextId);
static gboolean display_associated(ContextId, char *);


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
	if (f)
		f(ctx);
	return FALSE;
}

static int timer_count_timeout(Context *ctx)
{
	ctx->timeout_action.id = g_timeout_add(ctx->timeout_action.timeout
					       * 1000, timer_alarm, ctx);
	return ctx->timeout_action.id;
}

void new_data_received(Context *ctx, DataList *list)
{
	char *data = xml_encode_data_list(list);

	if (data) {
		display_measurementdata(ctx->id, data);
		free(data);
	}
}

void device_associated(Context *ctx, DataList *list)
{
	char *data = xml_encode_data_list(list);

	if (data) {
		display_associated(ctx->id, data);
		free(data);
	}
}

void device_disassociated(Context *ctx)
{
	display_disassociated(ctx->id);
}

void hdp_configure()
{
	// oximeter, scale, blood pressure
	guint16 hdp_data_types[] = {0x1004, 0x1007, 0x1029, 0x100f, 0x0};
	plugin_bluez_update_data_types(TRUE, hdp_data_types); // TRUE=sink
}

static gboolean display_connected(ContextId conn_handle, const char *low_addr)
{
	printf("Device connected context %d:%d addr %s\n",
		conn_handle.plugin, (int) conn_handle.connid,
		low_addr);

	return TRUE;
}

static gboolean display_associated(ContextId conn_handle, char *xml)
{
	printf("Device associated context %d:%d\n",
		conn_handle.plugin, (int) conn_handle.connid);
	printf("\n\nAssociation data:\n%s\n\n", xml);

	return TRUE;
}

static gboolean display_measurementdata(ContextId conn_handle, gchar *xml)
{
	printf("Device context %d:%d\n",
		conn_handle.plugin, (int) conn_handle.connid);
	printf("\n\nMeasurement data:\n%s\n\n", xml);

	return TRUE;
}

static gboolean display_disassociated(ContextId conn_handle)
{
	printf("Device disassociated context %d:%d\n",
		conn_handle.plugin, (int) conn_handle.connid);
	return TRUE;
}

static gboolean display_disconnected(ContextId conn_handle, const char *low_addr)
{
	printf("Device disconnected context %d:%d addr %s\n",
		conn_handle.plugin, (int) conn_handle.connid,
		low_addr);

	return TRUE;
}

static GMainLoop *mainloop = NULL;

static void app_finalize(int sig)
{
	g_main_loop_quit(mainloop);
}

static void app_setup_signals()
{
	signal(SIGINT, app_finalize);
	signal(SIGTERM, app_finalize);
}

int main(int argc, char *argv[])
{
	g_type_init();

	CommunicationPlugin plugin;
	app_setup_signals();

	plugin = communication_plugin();

	plugin_bluez_setup(&plugin);
	bluez_listener.peer_connected = display_connected;
	bluez_listener.peer_disconnected = display_disconnected;
	plugin_bluez_set_listener(&bluez_listener);

	plugin.timer_count_timeout = timer_count_timeout;
	plugin.timer_reset_timeout = timer_reset_timeout;

	CommunicationPlugin *plugins[] = {&plugin, 0};

	manager_init(plugins);

	ManagerListener listener = MANAGER_LISTENER_EMPTY;
	listener.measurement_data_updated = &new_data_received;
	listener.segment_data_received = 0;
	listener.device_available = &device_associated;
	listener.device_unavailable = &device_disassociated;

	manager_add_listener(listener);
	manager_start();

	hdp_configure();

	mainloop = g_main_loop_new(NULL, FALSE);
	g_main_loop_ref(mainloop);
	g_main_loop_run(mainloop);

	manager_finalize();

	return 0;
}
