namespace Gpio;

class GPIOLine
{
    public static function getValues(int fd) -> int
    {
        int value;

        let value = get_values(fd);

        return value;
    }

    public static function setValues(int fd, int value) -> bool
    {
        return set_values(fd, value);
    }

    /**
     * Read edge detection events from the line request FD.
     * Uses read() syscall, not ioctl.
     * Line must have GPIO_V2_LINE_FLAG_EDGE_RISING or GPIO_V2_LINE_FLAG_EDGE_FALLING enabled.
     * Returns array with event data or empty array if no event/error.
     */
    public static function readEdgeEvents(int fd) -> array
    {
        var event;

        let event = read_edge_events(fd);

        return event;
    }
}
