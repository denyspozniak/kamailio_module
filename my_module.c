#include "/usr/src/kamailio/src/core/sr_module.h"

MODULE_VERSION

static int mod_init();

// The real structure is kam_module_exports
// due to we define  KAMAILIO_MOD_INTERFACE
struct module_exports exports = {
  "my_module", /**< module name */
  DEFAULT_DLFLAGS, /**< flags for dlopen  */
  0, /**< null term array of the exported commands */
  0, /**< null term array of the exported module parameters */
  0, /**< null term array of the exported module statistics */
  0, /**< null terminated array of the exported MI functions */
  0, /*!< null terminated array of the
          exported module items (pseudo-variables) */
  0, /**< null terminated array of the
                    additional processes required by the
                    module */
  mod_init, /**< Initialization function */
  0, /**< function used for responses,
          returns yes or no; can be null */
  0, /**< function called when the module should
          be "destroyed", e.g: on ser exit;
          can be null */
  0, /**< function called by all processes
          after the fork */
};

static int mod_init() {
  INFO("%s - initializing\n", exports.name);
  return 0;
}

