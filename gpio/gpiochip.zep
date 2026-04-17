namespace Gpio;

%{
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/gpio.h>
}%
class GPIOChip
{
    public static function getLine(int fd, int line, int flags, int event_buffer_size = 0, string consumer = "php-ext-ioctl") -> int
    {
        int line_fd;

        %{
            struct gpio_v2_line_config config = {
                .flags = flags
            };
            struct gpio_v2_line_request request = {
                .offsets = {line},
                .num_lines = 1,
                .config = config,
                .event_buffer_size = event_buffer_size
            };
            memcpy(request.consumer, Z_STRVAL(consumer), strnlen(Z_STRVAL(consumer), sizeof(request.consumer)));

            int result;
            result = ioctl(fd, GPIO_V2_GET_LINE_IOCTL, &request);

            if (result < 0) {
                line_fd = -1;
            } else {
                line_fd = request.fd;
            }
        }%

        return line_fd;
    }

    public static function chipInfo(int fd) -> array
    {
        array info = [];

        %{
            struct gpiochip_info chip_info;
            int results;
            
            memset(&chip_info, 0, sizeof(chip_info));
            results = ioctl(fd, GPIO_GET_CHIPINFO_IOCTL, &chip_info);
            
            if (results < 0) {
                array_init(&info);
            } else {
                array_init(&info);
                add_assoc_string(&info, "name", chip_info.name);
                add_assoc_string(&info, "label", chip_info.label);
                add_assoc_long(&info, "lines", chip_info.lines);
            }
        }%

        return info;
    }

    public static function lineInfo(int fd, int offset) -> array
    {
        array info = [];

        %{
            struct gpio_v2_line_info line_info;
            int results;
            
            memset(&line_info, 0, sizeof(line_info));
            line_info.offset = offset;
            results = ioctl(fd, GPIO_V2_GET_LINEINFO_IOCTL, &line_info);
            
            if (results < 0) {
                array_init(&info);
            } else {
                array_init(&info);
                add_assoc_string(&info, "name", line_info.name);
                add_assoc_string(&info, "consumer", line_info.consumer);
                add_assoc_long(&info, "offset", line_info.offset);
                add_assoc_long(&info, "num_attrs", line_info.num_attrs);
                add_assoc_long(&info, "flags", line_info.flags);
            }
        }%

        return info;
    }

    public static function lineInfoWatch(int fd, int offset) -> int
    {
        int results;

        %{
            struct gpio_v2_line_info line_info;
            memset(&line_info, 0, sizeof(line_info));
            line_info.offset = offset;
            
            results = ioctl(fd, GPIO_V2_GET_LINEINFO_WATCH_IOCTL, &line_info);
        }%

        if (results < 0) {
            return -1;
        }

        return 0;
    }

    public static function lineInfoUnwatch(int fd, int offset) -> int
    {
        int results;

        %{
            __u32 line_offset = offset;
            results = ioctl(fd, GPIO_GET_LINEINFO_UNWATCH_IOCTL, &line_offset);
        }%

        if (results < 0) {
            return -1;
        }

        return 0;
    }

    /**
     * Read line info changed events from the chip FD
     * Uses read() syscall, not ioctl
     * Returns array with event data or empty array if no event/error
     */
    public static function lineInfoChanged(int fd) -> array
    {
        array event = [];

        %{
            struct gpio_v2_line_info_changed event_data;
            ssize_t bytes_read;
            
            memset(&event_data, 0, sizeof(event_data));
            bytes_read = read(fd, &event_data, sizeof(event_data));
            
            if (bytes_read == sizeof(event_data)) {
                array_init(&event);
                
                // Add line info
                zval info_array;
                array_init(&info_array);
                add_assoc_string(&info_array, "name", event_data.info.name);
                add_assoc_string(&info_array, "consumer", event_data.info.consumer);
                add_assoc_long(&info_array, "offset", event_data.info.offset);
                add_assoc_long(&info_array, "num_attrs", event_data.info.num_attrs);
                add_assoc_long(&info_array, "flags", event_data.info.flags);
                add_assoc_zval(&event, "info", &info_array);
                
                // Add timestamp (nanoseconds)
                add_assoc_long(&event, "timestamp_ns", event_data.timestamp_ns);
                
                // Add event type (1=REQUESTED, 2=RELEASED, 3=CONFIG)
                add_assoc_long(&event, "event_type", event_data.event_type);
            } else {
                array_init(&event);
            }
        }%

        return event;
    }

}
