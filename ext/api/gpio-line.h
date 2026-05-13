#ifndef PHP_GPIO_LINE_API_H
#define PHP_GPIO_LINE_API_H

#include "php.h"

zend_long gpio_get_values(zval *fd);
zend_long gpio_set_values(zval *fd, zval *value);
void      gpio_read_edge_events(zval *return_value, zval *fd);

#endif /* PHP_GPIO_LINE_API_H */
