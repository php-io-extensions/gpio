
extern zend_class_entry *gpio_gpiochip_ce;

ZEPHIR_INIT_CLASS(Gpio_GPIOChip);

PHP_METHOD(Gpio_GPIOChip, getLine);
PHP_METHOD(Gpio_GPIOChip, chipInfo);
PHP_METHOD(Gpio_GPIOChip, lineInfo);
PHP_METHOD(Gpio_GPIOChip, lineInfoWatch);
PHP_METHOD(Gpio_GPIOChip, lineInfoUnwatch);
PHP_METHOD(Gpio_GPIOChip, lineInfoChanged);

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gpio_gpiochip_getline, 0, 3, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, fd, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, line, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, event_buffer_size, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, consumer, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gpio_gpiochip_chipinfo, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, fd, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gpio_gpiochip_lineinfo, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, fd, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gpio_gpiochip_lineinfowatch, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, fd, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gpio_gpiochip_lineinfounwatch, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, fd, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gpio_gpiochip_lineinfochanged, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, fd, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEPHIR_INIT_FUNCS(gpio_gpiochip_method_entry) {
	PHP_ME(Gpio_GPIOChip, getLine, arginfo_gpio_gpiochip_getline, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(Gpio_GPIOChip, chipInfo, arginfo_gpio_gpiochip_chipinfo, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(Gpio_GPIOChip, lineInfo, arginfo_gpio_gpiochip_lineinfo, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(Gpio_GPIOChip, lineInfoWatch, arginfo_gpio_gpiochip_lineinfowatch, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(Gpio_GPIOChip, lineInfoUnwatch, arginfo_gpio_gpiochip_lineinfounwatch, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(Gpio_GPIOChip, lineInfoChanged, arginfo_gpio_gpiochip_lineinfochanged, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_FE_END
};
