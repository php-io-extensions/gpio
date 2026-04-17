namespace Gpio;

%{
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/gpio.h>
}%
class GPIOLine
{
    public static function getValues(int fd) -> int
    {
        int value;

        %{
            struct gpio_v2_line_values values;
            int result;

            memset(&values, 0, sizeof(values));
            values.mask = 1;  // Read bit 0

            result = ioctl(fd, GPIO_V2_LINE_GET_VALUES_IOCTL, &values);

            if (result < 0) {
                value = -1;
            } else {
                value = (values.bits & 1) ? 1 : 0;
            }
        }%

        return value;
    }

    public static function setValues(int fd, int value) -> bool
    {
        int result;

        %{
            struct gpio_v2_line_values values;

            memset(&values, 0, sizeof(values));
            values.bits = (value != 0) ? 1 : 0;  // Set to 1 (HIGH) or 0 (LOW)
            values.mask = 1;  // Update bit 0

            result = ioctl(fd, GPIO_V2_LINE_SET_VALUES_IOCTL, &values);
        }%

        return result >= 0;
    }

    /**
     * Read edge detection events from the line request FD
     * Uses read() syscall, not ioctl
     * Line must have GPIO_V2_LINE_FLAG_EDGE_RISING or GPIO_V2_LINE_FLAG_EDGE_FALLING enabled
     * Returns array with event data or empty array if no event/error
     */
    public static function readEdgeEvents(int fd) -> array
    {
        array event = [];

        %{
            struct gpio_v2_line_event event_data;
            ssize_t bytes_read;
            
            memset(&event_data, 0, sizeof(event_data));
            bytes_read = read(fd, &event_data, sizeof(event_data));
            
            if (bytes_read == sizeof(event_data)) {
                array_init(&event);
                
                // Add timestamp (nanoseconds)
                add_assoc_long(&event, "timestamp_ns", event_data.timestamp_ns);
                
                // Add event ID (1=RISING_EDGE, 2=FALLING_EDGE)
                add_assoc_long(&event, "id", event_data.id);
                
                // Add line offset that triggered the event
                add_assoc_long(&event, "offset", event_data.offset);
                
                // Add sequence number (across all lines in request)
                add_assoc_long(&event, "seqno", event_data.seqno);
                
                // Add line-specific sequence number
                add_assoc_long(&event, "line_seqno", event_data.line_seqno);
            } else {
                array_init(&event);
            }
        }%

        return event;
    }
}
