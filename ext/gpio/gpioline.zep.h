
extern zend_class_entry *gpio_gpioline_ce;

ZEPHIR_INIT_CLASS(Gpio_GPIOLine);

PHP_METHOD(Gpio_GPIOLine, getValues);
PHP_METHOD(Gpio_GPIOLine, setValues);
PHP_METHOD(Gpio_GPIOLine, readEdgeEvents);

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gpio_gpioline_getvalues, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, fd, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gpio_gpioline_setvalues, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, fd, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gpio_gpioline_readedgeevents, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, fd, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEPHIR_INIT_FUNCS(gpio_gpioline_method_entry) {
	PHP_ME(Gpio_GPIOLine, getValues, arginfo_gpio_gpioline_getvalues, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(Gpio_GPIOLine, setValues, arginfo_gpio_gpioline_setvalues, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(Gpio_GPIOLine, readEdgeEvents, arginfo_gpio_gpioline_readedgeevents, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_FE_END
};
