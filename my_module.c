#include "/usr/src/kamailio/src/core/sr_module.h"

MODULE_VERSION

static int mod_init();

struct module_exports exports = {
  "my_module",
  DEFAULT_DLFLAGS,
  0,
  0,
  0,
  0,
  0,
  0,
  mod_init, /**< Initialization function */
  0,
  0,
  0,
};

static int mod_init() {
  INFO("%s - initializing\n", exports.name);
  return 0;
}

