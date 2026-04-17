<?php

namespace Gpio;

class GPIOChip
{


    /**
     * @param int $fd
     * @param int $line
     * @param int $flags
     * @param int $event_buffer_size
     * @param string $consumer
     * @return int
     */
    public static function getLine(int $fd, int $line, int $flags, int $event_buffer_size = 0, string $consumer = 'php-ext-ioctl'): int
    {
    }

    /**
     * @param int $fd
     * @return array
     */
    public static function chipInfo(int $fd): array
    {
    }

    /**
     * @param int $fd
     * @param int $offset
     * @return array
     */
    public static function lineInfo(int $fd, int $offset): array
    {
    }

    /**
     * @param int $fd
     * @param int $offset
     * @return int
     */
    public static function lineInfoWatch(int $fd, int $offset): int
    {
    }

    /**
     * @param int $fd
     * @param int $offset
     * @return int
     */
    public static function lineInfoUnwatch(int $fd, int $offset): int
    {
    }

    /**
     * Read line info changed events from the chip FD
     * Uses read() syscall, not ioctl
     * Returns array with event data or empty array if no event/error
     *
     * @param int $fd
     * @return array
     */
    public static function lineInfoChanged(int $fd): array
    {
    }
}
