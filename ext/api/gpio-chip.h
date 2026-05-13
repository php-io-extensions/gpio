#ifndef PHP_GPIO_CHIP_API_H
#define PHP_GPIO_CHIP_API_H

#include "php.h"

zend_long gpio_get_line(zval *fd, zval *line, zval *flags, zval *event_buffer_size, zval *consumer);
void      gpio_chip_info(zval *return_value, zval *fd);
void      gpio_line_info(zval *return_value, zval *fd, zval *offset);
zend_long gpio_line_info_watch(zval *fd, zval *offset);
zend_long gpio_line_info_unwatch(zval *fd, zval *offset);
void      gpio_line_info_changed(zval *return_value, zval *fd);

#endif /* PHP_GPIO_CHIP_API_H */
