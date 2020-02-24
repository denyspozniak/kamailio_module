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

	str my_uri;
	my_uri = msg->first_line.u.request.uri;

	LM_ERR("[MY] To URI user is [%.*s] \n", my_uri.len, my_uri.s);
	LM_ERR(" ---------------------------- \n");


/*
	if (msg->to->parsed) {
		struct to_body *tb = get_to(msg);

		if (parse_uri(tb->uri.s, tb->uri.len , &tb->parsed_uri)<0) {
			LM_ERR("failed to parse To uri\n");
			return -1;
		}

		LM_ERR("To URI user is [%s] \n", tb->parsed_uri.user.s);
		LM_ERR(" ---------------------------- \n");
	}

*/

	return 0;
}


