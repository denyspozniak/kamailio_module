/* ============================================================================
 *  my_module.c -- A minimal, educational Kamailio module.
 * ----------------------------------------------------------------------------
 *  This module demonstrates the bare minimum needed to:
 *
 *    1. Register a module with Kamailio's core.
 *    2. Export functions callable from `kamailio.cfg` (the routing script).
 *    3. Safely access fields of a parsed SIP message (`struct sip_msg`).
 *    4. Use Kamailio's `str` type and its logging macros correctly.
 *
 *  The two exported functions illustrate two common patterns:
 *
 *    - `get_sip_callid()` reads a single mandatory header (Call-ID).
 *    - `get_sip_to()`     reads a structured header (To) and parses its URI.
 *
 *  Notes on Kamailio specifics that newcomers often miss:
 *
 *    * Kamailio's `str` is { char *s; int len; } and is *not*
 *      NUL-terminated. Always print it with the "%.*s" pattern
 *      (i.e. `(int)s.len, s.s`). Using "%s" is undefined behaviour.
 *
 *    * Most non-trivial headers are parsed lazily. Before touching a
 *      structured header you must call `parse_headers(msg, HDR_xxx_F, 0)`
 *      (or one of its higher-level helpers such as `parse_to_header`).
 *
 *    * Exported functions return `int`: a value > 0 keeps script
 *      execution flowing, <= 0 stops it. The convention used here is
 *      `1` on success and `-1` on error, matching most core modules.
 * ============================================================================ */

#include "../../core/sr_module.h"
#include "../../core/dprint.h"
#include "../../core/trim.h"
#include "../../core/parser/msg_parser.h"
#include "../../core/parser/parse_uri.h"
#include "../../core/parser/parse_to.h"
#include "../../core/parser/parse_fline.h"

MODULE_VERSION

/* ---- Forward declarations -------------------------------------------------- */

static int mod_init(void);
static int w_get_sip_callid(struct sip_msg *msg);
static int w_get_sip_to(struct sip_msg *msg);

/* ---- Exported script commands ---------------------------------------------
 *
 * Fields of cmd_export_t:
 *   { name, function, param_count, fixup, free_fixup, allowed_routes }
 *
 * `ANY_ROUTE` lets the function be called from request_route, reply_route,
 * branch_route, failure_route, etc.
 * --------------------------------------------------------------------------- */
static cmd_export_t cmds[] = {
	{ "get_sip_callid", (cmd_function)w_get_sip_callid, 0, 0, 0, ANY_ROUTE },
	{ "get_sip_to",     (cmd_function)w_get_sip_to,     0, 0, 0, ANY_ROUTE },
	{ 0, 0, 0, 0, 0, 0 }
};

/* ---- Module descriptor ----------------------------------------------------
 *
 * Because we compile with -DKAMAILIO_MOD_INTERFACE (see Makefile), the
 * `struct module_exports` symbol is what Kamailio's loader resolves.
 *
 * Only the slots we actually use are populated; the rest must remain NULL.
 * --------------------------------------------------------------------------- */
struct module_exports exports = {
	"my_module",      /* module name                                         */
	DEFAULT_DLFLAGS,  /* dlopen() flags                                      */
	cmds,             /* exported script commands                            */
	0,                /* exported module parameters                          */
	0,                /* exported RPC commands                               */
	0,                /* exported pseudo-variables                           */
	0,                /* response-handling function                          */
	mod_init,         /* per-process-group initialization                    */
	0,                /* per-child initialization                            */
	0,                /* destroy function                                    */
	0                 /* on cancel                                           */
};

/* ---- Lifecycle ------------------------------------------------------------ */

static int mod_init(void)
{
	LM_INFO("[%s] module initialized\n", exports.name);
	return 0;
}

/* ============================================================================
 *  get_sip_callid()
 *
 *  Logs the Call-ID of the current SIP message.
 *
 *  Call-ID is a mandatory header (RFC 3261 §8.1.1.4), and Kamailio's parser
 *  exposes a fast accessor via `msg->callid`. The slot is populated only
 *  once the Call-ID header has actually been parsed, hence the explicit
 *  `parse_headers()` call below.
 * ============================================================================ */
static int w_get_sip_callid(struct sip_msg *msg)
{
	str callid;

	if (parse_headers(msg, HDR_CALLID_F, 0) < 0 || msg->callid == NULL) {
		LM_ERR("failed to locate Call-ID header\n");
		return -1;
	}

	callid = msg->callid->body;
	trim(&callid);  /* strip surrounding whitespace in-place on the str view */

	LM_INFO("Call-ID: [%.*s]\n", callid.len, callid.s);
	return 1;
}

/* ============================================================================
 *  get_sip_to()
 *
 *  Parses the To header and logs the user@host part of its URI.
 *
 *  Parsing strategy:
 *    1. `parse_headers(msg, HDR_TO_F, 0)` ensures the To header is at
 *       least lexically located in the message.
 *    2. `parse_to_header(msg)` fully parses it into a `struct to_body`.
 *    3. The header's URI itself is then parsed with `parse_uri()` into
 *       a `struct sip_uri` from which `.user` and `.host` can be read.
 *
 *  Each of these is its own step because Kamailio parses incrementally
 *  for performance: scripts that never inspect the To header pay nothing.
 * ============================================================================ */
static int w_get_sip_to(struct sip_msg *msg)
{
	struct to_body *tb;
	struct sip_uri  to_uri;

	if (parse_headers(msg, HDR_TO_F, 0) < 0) {
		LM_ERR("failed to parse SIP headers up to To\n");
		return -1;
	}

	if (parse_to_header(msg) < 0 || msg->to == NULL) {
		LM_ERR("failed to parse To header\n");
		return -1;
	}

	tb = get_to(msg);
	if (tb == NULL || tb->uri.s == NULL || tb->uri.len <= 0) {
		LM_ERR("To header has no URI\n");
		return -1;
	}

	if (parse_uri(tb->uri.s, tb->uri.len, &to_uri) < 0) {
		LM_ERR("failed to parse To URI: [%.*s]\n", tb->uri.len, tb->uri.s);
		return -1;
	}

	LM_INFO("To URI: [%.*s]  user=[%.*s]  host=[%.*s]\n",
	        tb->uri.len, tb->uri.s,
	        to_uri.user.len, to_uri.user.s,
	        to_uri.host.len, to_uri.host.s);

	return 1;
}
