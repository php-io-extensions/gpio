
/* This file was generated automatically by Zephir do not modify it! */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>

#include "php_ext.h"
#include "gpio.h"

#include <ext/standard/info.h>

#include <Zend/zend_operators.h>
#include <Zend/zend_exceptions.h>
#include <Zend/zend_interfaces.h>

#include "kernel/globals.h"
#include "kernel/main.h"
#include "kernel/fcall.h"
#include "kernel/memory.h"



zend_class_entry *gpio_gpiochip_ce;
zend_class_entry *gpio_gpioline_ce;

ZEND_DECLARE_MODULE_GLOBALS(gpio)

PHP_INI_BEGIN()
	
PHP_INI_END()

static PHP_MINIT_FUNCTION(gpio)
{
	REGISTER_INI_ENTRIES();
	zephir_module_init();
	ZEPHIR_INIT(Gpio_GPIOChip);
	ZEPHIR_INIT(Gpio_GPIOLine);
	
	return SUCCESS;
}

#ifndef ZEPHIR_RELEASE
static PHP_MSHUTDOWN_FUNCTION(gpio)
{
	
	zephir_deinitialize_memory();
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}
#endif

/**
 * Initialize globals on each request or each thread started
 */
static void php_zephir_init_globals(zend_gpio_globals *gpio_globals)
{
	gpio_globals->initialized = 0;

	/* Cache Enabled */
	gpio_globals->cache_enabled = 1;

	/* Recursive Lock */
	gpio_globals->recursive_lock = 0;

	/* Static cache */
	memset(gpio_globals->scache, '\0', sizeof(zephir_fcall_cache_entry*) * ZEPHIR_MAX_CACHE_SLOTS);

	
	
}

/**
 * Initialize globals only on each thread started
 */
static void php_zephir_init_module_globals(zend_gpio_globals *gpio_globals)
{
	
}

static PHP_RINIT_FUNCTION(gpio)
{
	zend_gpio_globals *gpio_globals_ptr;
	gpio_globals_ptr = ZEPHIR_VGLOBAL;

	php_zephir_init_globals(gpio_globals_ptr);
	zephir_initialize_memory(gpio_globals_ptr);

	
	return SUCCESS;
}

static PHP_RSHUTDOWN_FUNCTION(gpio)
{
	
	zephir_deinitialize_memory();
	return SUCCESS;
}



static PHP_MINFO_FUNCTION(gpio)
{
	php_info_print_box_start(0);
	php_printf("%s", PHP_GPIO_DESCRIPTION);
	php_info_print_box_end();

	php_info_print_table_start();
	php_info_print_table_header(2, PHP_GPIO_NAME, "enabled");
	php_info_print_table_row(2, "Author", PHP_GPIO_AUTHOR);
	php_info_print_table_row(2, "Version", PHP_GPIO_VERSION);
	php_info_print_table_row(2, "Build Date", __DATE__ " " __TIME__ );
	php_info_print_table_row(2, "Powered by Zephir", "Version " PHP_GPIO_ZEPVERSION);
	php_info_print_table_end();
	
	DISPLAY_INI_ENTRIES();
}

static PHP_GINIT_FUNCTION(gpio)
{
#if defined(COMPILE_DL_GPIO) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	php_zephir_init_globals(gpio_globals);
	php_zephir_init_module_globals(gpio_globals);
}

static PHP_GSHUTDOWN_FUNCTION(gpio)
{
	
}


zend_function_entry php_gpio_functions[] = {
	ZEND_FE_END

};

static const zend_module_dep php_gpio_deps[] = {
	
	ZEND_MOD_END
};

zend_module_entry gpio_module_entry = {
	STANDARD_MODULE_HEADER_EX,
	NULL,
	php_gpio_deps,
	PHP_GPIO_EXTNAME,
	php_gpio_functions,
	PHP_MINIT(gpio),
#ifndef ZEPHIR_RELEASE
	PHP_MSHUTDOWN(gpio),
#else
	NULL,
#endif
	PHP_RINIT(gpio),
	PHP_RSHUTDOWN(gpio),
	PHP_MINFO(gpio),
	PHP_GPIO_VERSION,
	ZEND_MODULE_GLOBALS(gpio),
	PHP_GINIT(gpio),
	PHP_GSHUTDOWN(gpio),
#ifdef ZEPHIR_POST_REQUEST
	PHP_PRSHUTDOWN(gpio),
#else
	NULL,
#endif
	STANDARD_MODULE_PROPERTIES_EX
};

/* implement standard "stub" routine to introduce ourselves to Zend */
#ifdef COMPILE_DL_GPIO
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(gpio)
#endif
