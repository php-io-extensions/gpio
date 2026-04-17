# GPIO

[![PHP][badge-php]][php]
[![Zephir][badge-zephir]][zephir-install]
[![Platform][badge-platform]][linux]

PHP-controllable Linux GPIO extension built with Zephir.

The GPIO extension is an extension for PHP (running on Linux devices) that allows PHP to utilize the available GPIO in a given Linux device, allowing for control of sensors, interrupts and diodes.

```shell
git clone https://github.com/DeptOfScrapyardRobotics/GPIO
cd GPIO
bash install.sh
```

## Requirements

- PHP 8.3+ with development headers (`php-dev` / `php-devel`)
- [Zephir][zephir-install] 0.19+
- [FD extension][fd-repo] — provides raw integer file descriptors via `Fd\FD::open()`
- Linux kernel with GPIO character device support (`/dev/gpiochipN`)
- GPIO v2 userspace API — kernel 5.10+

## Installation

Install Zephir if you haven't already:

```shell
composer global require phalcon/zephir
```

Install the [FD extension][fd-repo] first — GPIO requires raw integer file descriptors that PHP's stream layer cannot provide:

```shell
git clone https://github.com/DeptOfScrapyardRobotics/FD
cd FD && bash install.sh
```

Then clone and build this extension:

```shell
git clone https://github.com/DeptOfScrapyardRobotics/GPIO
cd GPIO
bash install.sh
```

`install.sh` handles the full workflow: clean → build → copy `.so` → write `30-gpio.ini` into all detected `conf.d` directories → verify `php -m` → reload php-fpm if present.

To use a specific Zephir binary:

```shell
ZEPHIR_BIN=/path/to/zephir bash install.sh
```

## API

All methods are static. File descriptors are plain integers. Use the [FD extension][fd-repo] to open and close them — PHP's stream-based `fopen` does not produce the raw integer FDs the kernel ioctls require.

```php
use Fd\FD;

$fd = FD::open('/dev/gpiochip0', 2); // O_RDWR = 2
// ... use $fd with GPIOChip methods ...
FD::close($fd);
```

### `Gpio\GPIOChip`

Operations on a GPIO chip file descriptor (e.g. an open `/dev/gpiochip0`).

---

#### `getLine(int $fd, int $line, int $flags, int $event_buffer_size = 0, string $consumer = "php-ext-ioctl"): int`

Requests a GPIO line and returns a new **line file descriptor**, or `-1` on failure.

| Parameter | Description |
|-----------|-------------|
| `$fd` | Chip file descriptor |
| `$line` | Line offset on the chip |
| `$flags` | `GPIO_V2_LINE_FLAG_*` bitmask (see [linux/gpio.h][gpio-h]) |
| `$event_buffer_size` | Kernel-side edge event buffer size; `0` for default |
| `$consumer` | Label shown in `/sys/kernel/debug/gpio` |

---

#### `chipInfo(int $fd): array`

Returns chip metadata, or `[]` on failure.

```php
['name' => string, 'label' => string, 'lines' => int]
```

---

#### `lineInfo(int $fd, int $offset): array`

Returns line metadata, or `[]` on failure.

```php
['name' => string, 'consumer' => string, 'offset' => int, 'num_attrs' => int, 'flags' => int]
```

---

#### `lineInfoWatch(int $fd, int $offset): int`

Subscribes to line-info change events for `$offset` on the chip FD. Returns `0` on success, `-1` on failure. Read events with `lineInfoChanged()`.

---

#### `lineInfoUnwatch(int $fd, int $offset): int`

Cancels a watch registered with `lineInfoWatch()`. Returns `0` on success, `-1` on failure.

---

#### `lineInfoChanged(int $fd): array`

Blocking read of one `gpio_v2_line_info_changed` event from the chip FD. Returns `[]` if no event or on error.

```php
[
    'info'         => ['name' => string, 'consumer' => string, 'offset' => int, 'num_attrs' => int, 'flags' => int],
    'timestamp_ns' => int,
    'event_type'   => int,  // 1 = REQUESTED, 2 = RELEASED, 3 = CONFIG_CHANGED
]
```

---

### `Gpio\GPIOLine`

Operations on a line file descriptor returned by `GPIOChip::getLine()`.

---

#### `getValues(int $fd): int`

Returns `1` (HIGH), `0` (LOW), or `-1` on failure.

---

#### `setValues(int $fd, int $value): bool`

Drives the line HIGH (`$value != 0`) or LOW (`$value == 0`). Returns `true` on success.

---

#### `readEdgeEvents(int $fd): array`

Blocking read of one edge event from a line configured with `EDGE_RISING` and/or `EDGE_FALLING`. Returns `[]` if no event or on error.

```php
[
    'timestamp_ns' => int,
    'id'           => int,  // 1 = RISING_EDGE, 2 = FALLING_EDGE
    'offset'       => int,
    'seqno'        => int,
    'line_seqno'   => int,
]
```

---

## Usage

```php
use Fd\FD;
use Gpio\GPIOChip;
use Gpio\GPIOLine;
```

GPIO v2 flag values for `getLine`. These map directly to `linux/gpio.h` — PHP does not define them, so pass the values directly or define your own constants:

| Constant | Value | Description |
|----------|-------|-------------|
| `GPIO_V2_LINE_FLAG_INPUT` | `0x04` | Configure line as input |
| `GPIO_V2_LINE_FLAG_OUTPUT` | `0x08` | Configure line as output |
| `GPIO_V2_LINE_FLAG_EDGE_RISING` | `0x10` | Detect rising edges |
| `GPIO_V2_LINE_FLAG_EDGE_FALLING` | `0x20` | Detect falling edges |
| `GPIO_V2_LINE_FLAG_BIAS_PULL_UP` | `0x100` | Enable pull-up resistor |
| `GPIO_V2_LINE_FLAG_BIAS_PULL_DOWN` | `0x200` | Enable pull-down resistor |

Combine with `|` as needed.

---

### Digital output (LED, relay, fan)

```php
$fd = FD::open('/dev/gpiochip4', 2); // O_RDWR

$lineFd = GPIOChip::getLine($fd, 6, 0x08, 0, 'my-app'); // OUTPUT
if ($lineFd < 0) {
    throw new RuntimeException('Failed to request line');
}

GPIOLine::setValues($lineFd, 1); // HIGH — device on
sleep(1);
GPIOLine::setValues($lineFd, 0); // LOW  — device off

FD::close($fd);
```

### Digital input (button, sensor)

```php
$fd = FD::open('/dev/gpiochip0', 2);

$lineFd = GPIOChip::getLine($fd, 27, 0x04, 0, 'my-app'); // INPUT

$state = GPIOLine::getValues($lineFd);
echo $state === 1 ? "HIGH\n" : "LOW\n";

FD::close($fd);
```

### Edge detection (interrupt)

```php
// INPUT | EDGE_RISING | EDGE_FALLING
$fd = FD::open('/dev/gpiochip0', 2);

$lineFd = GPIOChip::getLine($fd, 27, 0x04 | 0x10 | 0x20, 64, 'my-app');

while (true) {
    $event = GPIOLine::readEdgeEvents($lineFd);
    if ($event !== []) {
        $edge = $event['id'] === 1 ? 'RISING' : 'FALLING';
        echo "{$edge} on offset {$event['offset']} at {$event['timestamp_ns']} ns\n";
    }
}
```

### Query chip and line metadata

```php
$fd = FD::open('/dev/gpiochip0', 2);

$chip = GPIOChip::chipInfo($fd);
echo "Chip: {$chip['name']} — {$chip['label']} ({$chip['lines']} lines)\n";

$line = GPIOChip::lineInfo($fd, 6);
echo "Line 6: consumer={$line['consumer']} flags={$line['flags']}\n";

FD::close($fd);
```

## License

Copyright © Project Saturn Studios, LLC. All rights reserved.

[badge-php]: https://img.shields.io/badge/php-%3E%3D8.3-blue
[badge-zephir]: https://img.shields.io/badge/zephir-0.19%2B-orange
[badge-platform]: https://img.shields.io/badge/platform-linux%20-lightgrey
[php]: https://www.php.net
[zephir-install]: https://docs.zephir-lang.com/latest/en/installation
[linux]: https://www.kernel.org
[gpio-h]: https://elixir.bootlin.com/linux/latest/source/include/uapi/linux/gpio.h
[fd-repo]: https://github.com/DeptOfScrapyardRobotics/FD