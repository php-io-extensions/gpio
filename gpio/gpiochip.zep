namespace Gpio;

class GPIOChip
{
    public static function getLine(int fd, int line, int flags, int event_buffer_size = 0, string consumer = "php-ext-ioctl") -> int
    {
        int line_fd;

        let line_fd = get_line(fd, line, flags, event_buffer_size, consumer);

        return line_fd;
    }

    public static function chipInfo(int fd) -> array
    {
        var info;

        let info = chip_info(fd);

        return info;
    }

    public static function lineInfo(int fd, int offset) -> array
    {
        var info;

        let info = line_info(fd, offset);

        return info;
    }

    public static function lineInfoWatch(int fd, int offset) -> int
    {
        int results;

        let results = line_info_watch(fd, offset);

        return results;
    }

    public static function lineInfoUnwatch(int fd, int offset) -> int
    {
        int results;

        let results = line_info_unwatch(fd, offset);

        return results;
    }

    /**
     * Read line info changed events from the chip FD.
     * Uses read() syscall, not ioctl.
     * Returns array with event data or empty array if no event/error.
     */
    public static function lineInfoChanged(int fd) -> array
    {
        var event;

        let event = line_info_changed(fd);

        return event;
    }

}
