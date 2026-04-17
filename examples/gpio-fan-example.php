<?php

use Fd\FD;
use Gpio\GPIOChip;
use Gpio\GPIOLine;

$fd = FD::open('/dev/gpiochip4', 2);
$gpio = GPIOChip::chipInfo($fd);
$line = GPIOChip::lineInfo($fd, 6);
$line_fd = GPIOChip::getLine($fd, 6, 8, 4, 'testing');
GPIOLine::setValues($line_fd, 1);
