<?php

namespace Gpio;

class GPIOLine
{


    /**
     * @param int $fd
     * @return int
     */
    public static function getValues(int $fd): int
    {
    }

    /**
     * @param int $fd
     * @param int $value
     * @return bool
     */
    public static function setValues(int $fd, int $value): bool
    {
    }

    /**
     * Read edge detection events from the line request FD
     * Uses read() syscall, not ioctl
     * Line must have GPIO_V2_LINE_FLAG_EDGE_RISING or GPIO_V2_LINE_FLAG_EDGE_FALLING enabled
     * Returns array with event data or empty array if no event/error
     *
     * @param int $fd
     * @return array
     */
    public static function readEdgeEvents(int $fd): array
    {
    }
}
