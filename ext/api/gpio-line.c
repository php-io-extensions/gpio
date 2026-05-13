#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ext.h"
#include "gpio-line.h"
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/gpio.h>

zend_long gpio_get_values(zval *fd)
{
    struct gpio_v2_line_values values;

    memset(&values, 0, sizeof(values));
    values.mask = 1;

    int result = ioctl((int) Z_LVAL_P(fd), GPIO_V2_LINE_GET_VALUES_IOCTL, &values);

    if (result < 0) {
        return -1;
    }

    return (values.bits & 1) ? 1 : 0;
}

zend_long gpio_set_values(zval *fd, zval *value)
{
    struct gpio_v2_line_values values;

    memset(&values, 0, sizeof(values));
    values.bits = Z_LVAL_P(value) != 0 ? 1 : 0;
    values.mask = 1;

    int result = ioctl((int) Z_LVAL_P(fd), GPIO_V2_LINE_SET_VALUES_IOCTL, &values);

    return result >= 0 ? 1 : 0;
}

void gpio_read_edge_events(zval *return_value, zval *fd)
{
    struct gpio_v2_line_event event_data;
    ssize_t bytes_read;

    memset(&event_data, 0, sizeof(event_data));
    bytes_read = read((int) Z_LVAL_P(fd), &event_data, sizeof(event_data));

    array_init(return_value);

    if (bytes_read == (ssize_t) sizeof(event_data)) {
        add_assoc_long(return_value, "timestamp_ns", event_data.timestamp_ns);
        add_assoc_long(return_value, "id",           event_data.id);
        add_assoc_long(return_value, "offset",       event_data.offset);
        add_assoc_long(return_value, "seqno",        event_data.seqno);
        add_assoc_long(return_value, "line_seqno",   event_data.line_seqno);
    }
}