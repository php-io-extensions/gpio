#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ext.h"
#include "gpio-chip.h"
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/gpio.h>

zend_long gpio_get_line(zval *fd, zval *line, zval *flags, zval *event_buffer_size, zval *consumer)
{
    struct gpio_v2_line_config config = {
        .flags = (uint64_t) Z_LVAL_P(flags)
    };
    struct gpio_v2_line_request request = {
        .offsets          = { (uint32_t) Z_LVAL_P(line) },
        .num_lines        = 1,
        .config           = config,
        .event_buffer_size = (uint32_t) Z_LVAL_P(event_buffer_size)
    };
    memcpy(request.consumer, Z_STRVAL_P(consumer),
           strnlen(Z_STRVAL_P(consumer), sizeof(request.consumer)));

    int result = ioctl((int) Z_LVAL_P(fd), GPIO_V2_GET_LINE_IOCTL, &request);

    if (result < 0) {
        return -1;
    }

    return (zend_long) request.fd;
}

void gpio_line_info(zval *return_value, zval *fd, zval *offset)
{
    struct gpio_v2_line_info line_info;
    int result;

    memset(&line_info, 0, sizeof(line_info));
    line_info.offset = (uint32_t) Z_LVAL_P(offset);
    result = ioctl((int) Z_LVAL_P(fd), GPIO_V2_GET_LINEINFO_IOCTL, &line_info);

    array_init(return_value);

    if (result >= 0) {
        add_assoc_string(return_value, "name",      line_info.name);
        add_assoc_string(return_value, "consumer",  line_info.consumer);
        add_assoc_long(return_value,   "offset",    line_info.offset);
        add_assoc_long(return_value,   "num_attrs", line_info.num_attrs);
        add_assoc_long(return_value,   "flags",     line_info.flags);
    }
}

void gpio_chip_info(zval *return_value, zval *fd)
{
    struct gpiochip_info chip_info;
    int result;

    memset(&chip_info, 0, sizeof(chip_info));
    result = ioctl((int) Z_LVAL_P(fd), GPIO_GET_CHIPINFO_IOCTL, &chip_info);

    array_init(return_value);

    if (result >= 0) {
        add_assoc_string(return_value, "name",  chip_info.name);
        add_assoc_string(return_value, "label", chip_info.label);
        add_assoc_long(return_value,   "lines", chip_info.lines);
    }
}

zend_long gpio_line_info_watch(zval *fd, zval *offset)
{
    struct gpio_v2_line_info line_info;

    memset(&line_info, 0, sizeof(line_info));
    line_info.offset = (uint32_t) Z_LVAL_P(offset);

    int result = ioctl((int) Z_LVAL_P(fd), GPIO_V2_GET_LINEINFO_WATCH_IOCTL, &line_info);

    return result < 0 ? -1 : 0;
}

zend_long gpio_line_info_unwatch(zval *fd, zval *offset)
{
    __u32 line_offset = (__u32) Z_LVAL_P(offset);

    int result = ioctl((int) Z_LVAL_P(fd), GPIO_GET_LINEINFO_UNWATCH_IOCTL, &line_offset);

    return result < 0 ? -1 : 0;
}

void gpio_line_info_changed(zval *return_value, zval *fd)
{
    struct gpio_v2_line_info_changed event_data;
    ssize_t bytes_read;

    memset(&event_data, 0, sizeof(event_data));
    bytes_read = read((int) Z_LVAL_P(fd), &event_data, sizeof(event_data));

    array_init(return_value);

    if (bytes_read == (ssize_t) sizeof(event_data)) {
        zval info_array;
        array_init(&info_array);
        add_assoc_string(&info_array, "name",      event_data.info.name);
        add_assoc_string(&info_array, "consumer",  event_data.info.consumer);
        add_assoc_long(&info_array,   "offset",    event_data.info.offset);
        add_assoc_long(&info_array,   "num_attrs", event_data.info.num_attrs);
        add_assoc_long(&info_array,   "flags",     event_data.info.flags);
        add_assoc_zval(return_value, "info", &info_array);

        add_assoc_long(return_value, "timestamp_ns", event_data.timestamp_ns);
        add_assoc_long(return_value, "event_type",   event_data.event_type);
    }
}