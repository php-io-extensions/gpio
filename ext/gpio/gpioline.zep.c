
#ifdef HAVE_CONFIG_H
#include "../ext_config.h"
#endif

#include <php.h>
#include "../php_ext.h"
#include "../ext.h"

#include <Zend/zend_operators.h>
#include <Zend/zend_exceptions.h>
#include <Zend/zend_interfaces.h>

#include "kernel/main.h"
#include "kernel/operators.h"
#include "kernel/memory.h"
#include "kernel/object.h"

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/gpio.h>



ZEPHIR_INIT_CLASS(Gpio_GPIOLine)
{
	ZEPHIR_REGISTER_CLASS(Gpio, GPIOLine, gpio, gpioline, gpio_gpioline_method_entry, 0);

	return SUCCESS;
}

PHP_METHOD(Gpio_GPIOLine, getValues)
{
	zval *fd_param = NULL;
	zend_long fd, value = 0;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(fd)
	ZEND_PARSE_PARAMETERS_END();
	zephir_fetch_params_without_memory_grow(1, 0, &fd_param);
	
            struct gpio_v2_line_values values;
            int result;

            memset(&values, 0, sizeof(values));
            values.mask = 1;  // Read bit 0

            result = ioctl(fd, GPIO_V2_LINE_GET_VALUES_IOCTL, &values);

            if (result < 0) {
                value = -1;
            } else {
                value = (values.bits & 1) ? 1 : 0;
            }
        
	RETURN_LONG(value);
}

PHP_METHOD(Gpio_GPIOLine, setValues)
{
	zval *fd_param = NULL, *value_param = NULL;
	zend_long fd, value, result = 0;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(fd)
		Z_PARAM_LONG(value)
	ZEND_PARSE_PARAMETERS_END();
	zephir_fetch_params_without_memory_grow(2, 0, &fd_param, &value_param);
	
            struct gpio_v2_line_values values;

            memset(&values, 0, sizeof(values));
            values.bits = (value != 0) ? 1 : 0;  // Set to 1 (HIGH) or 0 (LOW)
            values.mask = 1;  // Update bit 0

            result = ioctl(fd, GPIO_V2_LINE_SET_VALUES_IOCTL, &values);
        
	RETURN_BOOL(result >= 0);
}

/**
 * Read edge detection events from the line request FD
 * Uses read() syscall, not ioctl
 * Line must have GPIO_V2_LINE_FLAG_EDGE_RISING or GPIO_V2_LINE_FLAG_EDGE_FALLING enabled
 * Returns array with event data or empty array if no event/error
 */
PHP_METHOD(Gpio_GPIOLine, readEdgeEvents)
{
	zval event;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zval *fd_param = NULL;
	zend_long fd;

	ZVAL_UNDEF(&event);
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(fd)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &fd_param);
	ZEPHIR_INIT_VAR(&event);
	array_init(&event);
	
            struct gpio_v2_line_event event_data;
            ssize_t bytes_read;
            
            memset(&event_data, 0, sizeof(event_data));
            bytes_read = read(fd, &event_data, sizeof(event_data));
            
            if (bytes_read == sizeof(event_data)) {
                array_init(&event);
                
                // Add timestamp (nanoseconds)
                add_assoc_long(&event, "timestamp_ns", event_data.timestamp_ns);
                
                // Add event ID (1=RISING_EDGE, 2=FALLING_EDGE)
                add_assoc_long(&event, "id", event_data.id);
                
                // Add line offset that triggered the event
                add_assoc_long(&event, "offset", event_data.offset);
                
                // Add sequence number (across all lines in request)
                add_assoc_long(&event, "seqno", event_data.seqno);
                
                // Add line-specific sequence number
                add_assoc_long(&event, "line_seqno", event_data.line_seqno);
            } else {
                array_init(&event);
            }
        
	RETURN_CTOR(&event);
}

