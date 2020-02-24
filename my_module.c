#include "/usr/src/kamailio/src/core/sr_module.h"
#include "/usr/src/kamailio/src/core/trim.h"

#include "/usr/src/kamailio/src/core/parser/msg_parser.h"
#include "/usr/src/kamailio/src/core/parser/parse_uri.h"
#include "/usr/src/kamailio/src/core/parser/parse_to.h"
#include "/usr/src/kamailio/src/core/parser/parse_fline.h"

MODULE_VERSION

static int mod_init();
static int get_sip_callid(struct sip_msg* msg);
static int get_sip_to(struct sip_msg *msg);


static cmd_export_t cmds[] = {
	{"get_sip_callid", (cmd_function)get_sip_callid, 0, 0, 0, ANY_ROUTE},
	{"get_sip_to", (cmd_function)get_sip_to, 0, 0, 0, ANY_ROUTE},
	{0,0,0,0,0}
};

// The real structure is kam_module_exports
// due to we define  KAMAILIO_MOD_INTERFACE
struct module_exports exports = {
	"my_module",
	DEFAULT_DLFLAGS,
	cmds,       /* Exported functions */
	0,
	0,
	0,
	0,
	0,
	mod_init, /* Initialization function */
	0,
	0,
	0,
};

static int mod_init() {
	INFO("%s - initializing\n", exports.name);
	return 0;
}

static int get_sip_callid(struct sip_msg* msg)
{
	str scallid;

	if (msg->callid == NULL) {
		LM_ERR("SIP Call-ID is null \n");
		return -1;
	} else {
		scallid = msg->callid->body;
		trim(&scallid);
		LM_ERR("SIP Call-ID is [%s]\n", scallid);
	}
	return 0;
}

static int get_sip_to(struct sip_msg *msg) {

	LM_ERR("get_sip_to\n");

	struct to_body* to_b;

	if ( !msg->to && ( parse_headers(msg,HDR_TO_F,0)==-1 || !msg->to)) {
		LM_ERR("bad msg or missing To header\n");
		goto error;
	}

	if (msg->to->parsed) {
		struct to_body *tb = get_to(msg);

		if (parse_uri(tb->uri.s, tb->uri.len , &tb->parsed_uri)<0) {
			LM_ERR("failed to parse To uri\n");
			return -1;
		}

		LM_ERR("To URI user is %s \n", tb->parsed_uri.user.s);
	}

	return 0;
error:
	return -1;
}


