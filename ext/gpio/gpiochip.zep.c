
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



ZEPHIR_INIT_CLASS(Gpio_GPIOChip)
{
	ZEPHIR_REGISTER_CLASS(Gpio, GPIOChip, gpio, gpiochip, gpio_gpiochip_method_entry, 0);

	return SUCCESS;
}

PHP_METHOD(Gpio_GPIOChip, getLine)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zval consumer;
	zval *fd_param = NULL, *line_param = NULL, *flags_param = NULL, *event_buffer_size_param = NULL, *consumer_param = NULL;
	zend_long fd, line, flags, event_buffer_size, line_fd = 0;

	ZVAL_UNDEF(&consumer);
	ZEND_PARSE_PARAMETERS_START(3, 5)
		Z_PARAM_LONG(fd)
		Z_PARAM_LONG(line)
		Z_PARAM_LONG(flags)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(event_buffer_size)
		Z_PARAM_STR(consumer)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 3, 2, &fd_param, &line_param, &flags_param, &event_buffer_size_param, &consumer_param);
	if (!event_buffer_size_param) {
		event_buffer_size = 0;
	} else {
		}
	if (!consumer_param) {
		ZEPHIR_INIT_VAR(&consumer);
		ZVAL_STRING(&consumer, "php-ext-ioctl");
	} else {
		zephir_get_strval(&consumer, consumer_param);
	}
	
            struct gpio_v2_line_config config = {
                .flags = flags
            };
            struct gpio_v2_line_request request = {
                .offsets = {line},
                .num_lines = 1,
                .config = config,
                .event_buffer_size = event_buffer_size
            };
            memcpy(request.consumer, Z_STRVAL(consumer), strnlen(Z_STRVAL(consumer), sizeof(request.consumer)));

            int result;
            result = ioctl(fd, GPIO_V2_GET_LINE_IOCTL, &request);

            if (result < 0) {
                line_fd = -1;
            } else {
                line_fd = request.fd;
            }
        
	RETURN_MM_LONG(line_fd);
}

PHP_METHOD(Gpio_GPIOChip, chipInfo)
{
	zval info;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zval *fd_param = NULL;
	zend_long fd;

	ZVAL_UNDEF(&info);
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(fd)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &fd_param);
	ZEPHIR_INIT_VAR(&info);
	array_init(&info);
	
            struct gpiochip_info chip_info;
            int results;
            
            memset(&chip_info, 0, sizeof(chip_info));
            results = ioctl(fd, GPIO_GET_CHIPINFO_IOCTL, &chip_info);
            
            if (results < 0) {
                array_init(&info);
            } else {
                array_init(&info);
                add_assoc_string(&info, "name", chip_info.name);
                add_assoc_string(&info, "label", chip_info.label);
                add_assoc_long(&info, "lines", chip_info.lines);
            }
        
	RETURN_CTOR(&info);
}

PHP_METHOD(Gpio_GPIOChip, lineInfo)
{
	zval info;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zval *fd_param = NULL, *offset_param = NULL;
	zend_long fd, offset;

	ZVAL_UNDEF(&info);
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(fd)
		Z_PARAM_LONG(offset)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 2, 0, &fd_param, &offset_param);
	ZEPHIR_INIT_VAR(&info);
	array_init(&info);
	
            struct gpio_v2_line_info line_info;
            int results;
            
            memset(&line_info, 0, sizeof(line_info));
            line_info.offset = offset;
            results = ioctl(fd, GPIO_V2_GET_LINEINFO_IOCTL, &line_info);
            
            if (results < 0) {
                array_init(&info);
            } else {
                array_init(&info);
                add_assoc_string(&info, "name", line_info.name);
                add_assoc_string(&info, "consumer", line_info.consumer);
                add_assoc_long(&info, "offset", line_info.offset);
                add_assoc_long(&info, "num_attrs", line_info.num_attrs);
                add_assoc_long(&info, "flags", line_info.flags);
            }
        
	RETURN_CTOR(&info);
}

PHP_METHOD(Gpio_GPIOChip, lineInfoWatch)
{
	zval *fd_param = NULL, *offset_param = NULL;
	zend_long fd, offset, results = 0;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(fd)
		Z_PARAM_LONG(offset)
	ZEND_PARSE_PARAMETERS_END();
	zephir_fetch_params_without_memory_grow(2, 0, &fd_param, &offset_param);
	
            struct gpio_v2_line_info line_info;
            memset(&line_info, 0, sizeof(line_info));
            line_info.offset = offset;
            
            results = ioctl(fd, GPIO_V2_GET_LINEINFO_WATCH_IOCTL, &line_info);
        
	if (results < 0) {
		RETURN_LONG(-1);
	}
	RETURN_LONG(0);
}

PHP_METHOD(Gpio_GPIOChip, lineInfoUnwatch)
{
	zval *fd_param = NULL, *offset_param = NULL;
	zend_long fd, offset, results = 0;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(fd)
		Z_PARAM_LONG(offset)
	ZEND_PARSE_PARAMETERS_END();
	zephir_fetch_params_without_memory_grow(2, 0, &fd_param, &offset_param);
	
            __u32 line_offset = offset;
            results = ioctl(fd, GPIO_GET_LINEINFO_UNWATCH_IOCTL, &line_offset);
        
	if (results < 0) {
		RETURN_LONG(-1);
	}
	RETURN_LONG(0);
}

/**
 * Read line info changed events from the chip FD
 * Uses read() syscall, not ioctl
 * Returns array with event data or empty array if no event/error
 */
PHP_METHOD(Gpio_GPIOChip, lineInfoChanged)
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
	
            struct gpio_v2_line_info_changed event_data;
            ssize_t bytes_read;
            
            memset(&event_data, 0, sizeof(event_data));
            bytes_read = read(fd, &event_data, sizeof(event_data));
            
            if (bytes_read == sizeof(event_data)) {
                array_init(&event);
                
                // Add line info
                zval info_array;
                array_init(&info_array);
                add_assoc_string(&info_array, "name", event_data.info.name);
                add_assoc_string(&info_array, "consumer", event_data.info.consumer);
                add_assoc_long(&info_array, "offset", event_data.info.offset);
                add_assoc_long(&info_array, "num_attrs", event_data.info.num_attrs);
                add_assoc_long(&info_array, "flags", event_data.info.flags);
                add_assoc_zval(&event, "info", &info_array);
                
                // Add timestamp (nanoseconds)
                add_assoc_long(&event, "timestamp_ns", event_data.timestamp_ns);
                
                // Add event type (1=REQUESTED, 2=RELEASED, 3=CONFIG)
                add_assoc_long(&event, "event_type", event_data.event_type);
            } else {
                array_init(&event);
            }
        
	RETURN_CTOR(&event);
}

