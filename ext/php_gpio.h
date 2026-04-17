
/* This file was generated automatically by Zephir do not modify it! */

#ifndef PHP_GPIO_H
#define PHP_GPIO_H 1

#ifdef PHP_WIN32
#define ZEPHIR_RELEASE 1
#endif

#include "kernel/globals.h"

#define PHP_GPIO_NAME        "gpio"
#define PHP_GPIO_VERSION     "0.1.0"
#define PHP_GPIO_EXTNAME     "gpio"
#define PHP_GPIO_AUTHOR      "Project Saturn Studios, LLC"
#define PHP_GPIO_ZEPVERSION  "0.19.0-$Id$"
#define PHP_GPIO_DESCRIPTION "PHP-Controllable GPIO Extension"



ZEND_BEGIN_MODULE_GLOBALS(gpio)

	int initialized;

	/** Function cache */
	HashTable *fcache;

	zephir_fcall_cache_entry *scache[ZEPHIR_MAX_CACHE_SLOTS];

	/* Cache enabled */
	unsigned int cache_enabled;

	/* Max recursion control */
	unsigned int recursive_lock;

	
ZEND_END_MODULE_GLOBALS(gpio)

#ifdef ZTS
#include "TSRM.h"
#endif

ZEND_EXTERN_MODULE_GLOBALS(gpio)

#ifdef ZTS
	#define ZEPHIR_GLOBAL(v) ZEND_MODULE_GLOBALS_ACCESSOR(gpio, v)
#else
	#define ZEPHIR_GLOBAL(v) (gpio_globals.v)
#endif

#ifdef ZTS
	ZEND_TSRMLS_CACHE_EXTERN()
	#define ZEPHIR_VGLOBAL ((zend_gpio_globals *) (*((void ***) tsrm_get_ls_cache()))[TSRM_UNSHUFFLE_RSRC_ID(gpio_globals_id)])
#else
	#define ZEPHIR_VGLOBAL &(gpio_globals)
#endif

#define ZEPHIR_API ZEND_API

#define zephir_globals_def gpio_globals
#define zend_zephir_globals_def zend_gpio_globals

extern zend_module_entry gpio_module_entry;
#define phpext_gpio_ptr &gpio_module_entry

#endif
