#include "/usr/src/kamailio/src/core/sr_module.h"

MODULE_VERSION

static int mod_init();
static int get_sip_callid(struct sip_msg* msg);

static cmd_export_t cmds[] = {
	{"get_sip_callid", (cmd_function)get_sip_callid, 0, 0, 0, ANY_ROUTE},
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
	if (msg->callid == NULL) {
		LM_ERR("SIP Call-ID is null \n");
		return -1;
	} else {
		LM_ERR("SIP Call-ID is %s\n", msg->callid->body.s);
		return 0;
	}

	return 0;
}



