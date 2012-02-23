#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <glib.h>

#include <src/util/bytelib.h>
#include <src/util/log.h>
#include <src/util/dateutil.h>
#include <src/dim/nomenclature.h>
#include <src/dim/mds.h>
#include <communication/parser/encoder_ASN1.h>
#include <communication/parser/struct_cleaner.h>
#include <src/trans/trans.h>
#include <src/specializations/pulse_oximeter.h>
#include "example_oximeter.h"


static TransPlugin *this_plugin = 0;
static char *lladdr = "trans_example_oximeter";
static int server_sk = -1;
static int sk = -1;
static const int tcp_port = 10404;

static const int config_id = 0x5190;

static ConfigReport generate_config()
{
	ConfigReport cfgrep;
	bzero(&cfgrep, sizeof(cfgrep));

	ConfigObjectList object_list;
	bzero(&object_list, sizeof(object_list));

	object_list.count = 2;
	object_list.length = 80;
	object_list.value = malloc(sizeof(ConfigObject)*2);
	object_list.value[0].obj_class = MDC_MOC_VMO_METRIC_NU;

	object_list.value[0].obj_handle = 1;
	AttributeList *attr_list1 = malloc(sizeof(AttributeList));
	attr_list1->count = 4;
	attr_list1->length = 32;
	attr_list1->value = malloc(attr_list1->count * sizeof(AVA_Type));

	attr_list1->value[0].attribute_id = MDC_ATTR_ID_TYPE;
	attr_list1->value[0].attribute_value.length = 4;

	ByteStreamWriter *bsw = byte_stream_writer_instance(4);
	write_intu16(bsw, MDC_PART_SCADA);
	write_intu16(bsw, MDC_PULS_OXIM_SAT_O2);
	attr_list1->value[0].attribute_value.value = bsw->buffer;

	attr_list1->value[1].attribute_id = MDC_ATTR_METRIC_SPEC_SMALL;
	attr_list1->value[1].attribute_value.length = 2;

	free(bsw);
	bsw = byte_stream_writer_instance(2);
	write_intu16(bsw, 0x4040); // 0x40 0x40 avail-stored-data, acc-agent-init, measured
	attr_list1->value[1].attribute_value.value = bsw->buffer;

	attr_list1->value[2].attribute_id = MDC_ATTR_UNIT_CODE;
	attr_list1->value[2].attribute_value.length = 2;

	free(bsw);
	bsw = byte_stream_writer_instance(2);
	write_intu16(bsw, MDC_DIM_PERCENT);
	attr_list1->value[2].attribute_value.value = bsw->buffer;

	attr_list1->value[3].attribute_id = MDC_ATTR_ATTRIBUTE_VAL_MAP;
	attr_list1->value[3].attribute_value.length = 8;
	free(bsw);
	bsw = byte_stream_writer_instance(8);
	write_intu16(bsw, 1); // AttrValMap.count = 1
	write_intu16(bsw, 4); // AttrValMap.length = 4
	write_intu16(bsw, MDC_ATTR_NU_VAL_OBS_BASIC);
	write_intu16(bsw, 2); // value length = 2
	attr_list1->value[3].attribute_value.value = bsw->buffer;

	object_list.value[1].obj_class = MDC_MOC_VMO_METRIC_NU;
	object_list.value[1].obj_handle = 10;
	AttributeList *attr_list2 = malloc(sizeof(AttributeList));
	attr_list2->count = 4;
	attr_list2->length = 32;
	attr_list2->value = malloc(attr_list2->count * sizeof(AVA_Type));

	attr_list2->value[0].attribute_id = MDC_ATTR_ID_TYPE;
	attr_list2->value[0].attribute_value.length = 4;
	free(bsw);
	bsw = byte_stream_writer_instance(4);
	write_intu16(bsw, MDC_PART_SCADA);
	write_intu16(bsw, MDC_PULS_OXIM_PULS_RATE);
	attr_list2->value[0].attribute_value.value = bsw->buffer;

	attr_list2->value[1].attribute_id = MDC_ATTR_METRIC_SPEC_SMALL;
	attr_list2->value[1].attribute_value.length = 2;
	free(bsw);
	bsw = byte_stream_writer_instance(2);
	write_intu16(bsw, 0x4040); // 0x40 0x40 avail-stored-data, acc-agent-init, measured
	attr_list2->value[1].attribute_value.value = bsw->buffer;

	attr_list2->value[2].attribute_id = MDC_ATTR_UNIT_CODE;
	attr_list2->value[2].attribute_value.length = 2;
	free(bsw);
	bsw = byte_stream_writer_instance(2);
	write_intu16(bsw, MDC_DIM_BEAT_PER_MIN);
	attr_list2->value[2].attribute_value.value = bsw->buffer;

	attr_list2->value[3].attribute_id = MDC_ATTR_ATTRIBUTE_VAL_MAP;
	attr_list2->value[3].attribute_value.length = 8;
	free(bsw);
	bsw = byte_stream_writer_instance(8);
	write_intu16(bsw, 1); // AttrValMap.count = 1
	write_intu16(bsw, 4); // AttrValMap.length = 4
	write_intu16(bsw, MDC_ATTR_NU_VAL_OBS_BASIC);
	write_intu16(bsw, 2); // value length = 2
	attr_list2->value[3].attribute_value.value = bsw->buffer;
	free(bsw);

	object_list.value[0].attributes = *attr_list1;
	object_list.value[1].attributes = *attr_list2;

	free(attr_list1);
	free(attr_list2);

	cfgrep.config_obj_list = object_list;
	cfgrep.config_report_id = config_id;

	return cfgrep;
}

static PhdAssociationInformation generate_assoc_data()
{
	PhdAssociationInformation config_info;
	bzero(&config_info, sizeof(config_info));

	config_info.protocolVersion = ASSOC_VERSION1;
	config_info.encodingRules = MDER;
	config_info.nomenclatureVersion = NOM_VERSION1;

	config_info.functionalUnits = 0x00000000;
	config_info.systemType = SYS_TYPE_AGENT;

	config_info.system_id.length = strlen(lladdr);
	config_info.system_id.value = malloc(config_info.system_id.length);
	memcpy(config_info.system_id.value,
		lladdr,
		config_info.system_id.length);

	config_info.dev_config_id = config_id;

	config_info.data_req_mode_capab.data_req_mode_flags = DATA_REQ_SUPP_INIT_AGENT;
	// max number of simultaneous sessions
	config_info.data_req_mode_capab.data_req_init_agent_count = 0x01;
	config_info.data_req_mode_capab.data_req_init_manager_count = 0x00;

	config_info.optionList.count = 0;
	config_info.optionList.length = 0;

	return config_info;
}

static ScanReportInfoFixed populate_event_report(int oximetry, int pulse)
{
	ObservationScanFixed *measure;
	AbsoluteTime nu_time;
	BasicNuObsValue nu_oximetry;
	BasicNuObsValue nu_beats;
	time_t now;
	struct tm nowtm;

	measure = calloc(2, sizeof(ObservationScanFixed));

	time(&now);
	localtime_r(&now, &nowtm);

	nu_time = date_util_create_absolute_time(nowtm.tm_year + 1900,
						nowtm.tm_mon + 1,
						nowtm.tm_mday,
						nowtm.tm_hour,
						nowtm.tm_min,
						nowtm.tm_sec,
						0);

	// SFLOAT is just a float in internal representation
	nu_oximetry = oximetry;
	nu_beats = pulse;

	measure[0].obj_handle = 1;
	measure[0].obs_val_data.length = 10;
	ByteStreamWriter *writer0 = byte_stream_writer_instance(measure[0].obs_val_data.length);

	encode_basicnuobsvalue(writer0, &nu_oximetry);
	encode_absolutetime(writer0, &nu_time);

	measure[1].obj_handle = 10;
	measure[1].obs_val_data.length = 10;
	ByteStreamWriter *writer1 = byte_stream_writer_instance(measure[1].obs_val_data.length);

	encode_basicnuobsvalue(writer1, &nu_beats);
	encode_absolutetime(writer1, &nu_time);
	
	measure[0].obs_val_data.value = writer0->buffer;
	measure[1].obs_val_data.value = writer1->buffer;

	ObservationScanFixedList scan_fixed;
	bzero(&scan_fixed, sizeof(scan_fixed));

	scan_fixed.count = 2;
	scan_fixed.length = 28;
	scan_fixed.value = measure;

	ScanReportInfoFixed scan;
	bzero(&scan, sizeof(scan));

	scan.data_req_id = 0xF000;
	scan.scan_report_no = 0;
	scan.obs_scan_fixed = scan_fixed;

	del_byte_stream_writer(writer0, 0);
	del_byte_stream_writer(writer1, 0);

	return scan;
}

static AttributeList generate_spec()
{
	AttributeList spec;
	TypeVerList vlist;

	spec.value = calloc(1, sizeof(AVA_Type));
	AVA_Type *attr = &spec.value[0];

	vlist.count = 1;
	vlist.length = vlist.count * 4;
	vlist.value = calloc(vlist.count, 4);
	// specialization, not config id
	vlist.value[0].type = MDC_DEV_SPEC_PROFILE_PULS_OXIM;
	vlist.value[0].version = 1;

	ByteStreamWriter *encoded_vlist = byte_stream_writer_instance(4 + vlist.length);
	encode_typeverlist(encoded_vlist, &vlist);

	attr->attribute_id = MDC_ATTR_SYS_TYPE_SPEC_LIST;
	attr->attribute_value.length = encoded_vlist->size;
	attr->attribute_value.value = encoded_vlist->buffer;

	del_typeverlist(&vlist);
	free(encoded_vlist);

	spec.count = 1;
	spec.length = 4 + attr->attribute_value.length;

	return spec;
}

static gboolean data_received_cb(GIOChannel *src, GIOCondition cond, gpointer data);

static gboolean connected_cb(GIOChannel *src, GIOCondition cond, gpointer data)
{
	int new_sk;
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	bzero(&addr, sizeof(addr));

	DEBUG("TCP: accepting condition");

	new_sk = accept(g_io_channel_unix_get_fd(src),
			(struct sockaddr *) &addr, &addrlen);

	if (new_sk < 0) {
		DEBUG("TCP: Failed accept");
		return TRUE;
	}

	if (sk >= 0) {
		send(new_sk, "Busy\n", 5, 0);
		close(new_sk);
		return TRUE;
	}

	sk = new_sk;

	DEBUG("Transcoding oximeter: connected TCP");

	GIOChannel *channel = g_io_channel_unix_new(sk);
	g_io_add_watch(channel, G_IO_IN | G_IO_ERR | G_IO_HUP | G_IO_NVAL,
			data_received_cb, 0);

	trans_connected(this_plugin, lladdr,
			generate_spec(),
			generate_assoc_data(),
			generate_config());

	return TRUE;
}

static void disconnected_cb()
{
	DEBUG("Transcoding example oximeter: disconnected");
	close(sk);
	sk = -1;
	trans_disconnected(this_plugin, lladdr);
}

static void force_disconnect(const char *lladdr)
{
	disconnected_cb();
}

static gboolean data_received_cb(GIOChannel *src, GIOCondition cond, gpointer data)
{
	char buf[256];
	gsize count;

	if (cond != G_IO_IN) {
		disconnected_cb();
		g_io_channel_unref(src);
		return FALSE;
	}

	int fd = g_io_channel_unix_get_fd(src);
	count = recv(fd, buf, 256, 0);

	DEBUG("Transcoding example oximeter: recv %" G_GSIZE_FORMAT "", count);

	if (count == 0) {
		disconnected_cb();
		g_io_channel_unref(src);
		return FALSE;
	}

	DEBUG("Transcoding example oximeter: recv");
	
	// we expect a number like 1098070
	// meaning: 98% oximetry, 70 bpm
	int n = atoi(buf);
	int oximetry = (n % 1000000) / 1000;
	int pulse = n % 1000;

	if (n < 1000000 || oximetry < 0 || oximetry > 100 || pulse < 0) {
		const char *err = "Bad info; supply (1000000 + 1000 * oximetry + pulse)\n";
		send(fd, err, strlen(err), 0);
		return TRUE;
	}
	
	send(fd, "Ok\n", 3, 0);

	trans_event_report_fixed(this_plugin, lladdr, populate_event_report(oximetry, pulse));

	return TRUE;
}

static int init()
{
	DEBUG("trans oximeter: starting socket  %d", tcp_port);

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(tcp_port);

	server_sk = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (server_sk < 0) {
		DEBUG("trans oximeter: Error creating tcp socket");
		close(server_sk);
		server_sk = -1;
		return 0;
	}

	// Set the socket options
	int opt = 1; /* option is to be on/TRUE or off/FALSE */

	setsockopt(server_sk, SOL_SOCKET, SO_REUSEADDR, (char *) &opt,
		   sizeof(opt));

	int error = bind(server_sk, (struct sockaddr *) &server,
		     sizeof(struct sockaddr));

	if (error < 0) {
		DEBUG("trans oximeter: tcp Error in bind %d socket: %d", server_sk, errno);
		close(server_sk);
		server_sk = -1;
		return 0;
	}

	error = listen(server_sk, 1);

	if (error < 0) {
		DEBUG("trans oximeter: tcp Error in listen %d", server_sk);
		close(server_sk);
		server_sk = -1;
		return 0;
	}

	GIOChannel *channel = g_io_channel_unix_new(server_sk);
	g_io_add_watch(channel, G_IO_IN, connected_cb, 0);

	return 1;
}

static int finalize()
{
	close(server_sk);
	return 1;
}

void trans_plugin_oximeter_register(agent_connected_cb conn_cb, agent_disconnected_cb disconn_cb)
{
	this_plugin = malloc(sizeof(TransPlugin));
	this_plugin->init = &init;
	this_plugin->finalize = &finalize;
	this_plugin->force_disconnect = &force_disconnect;
	this_plugin->conn_cb = conn_cb;
	this_plugin->disconn_cb = disconn_cb;
	trans_register_plugin(this_plugin);
}


/** @} */
