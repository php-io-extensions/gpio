#!/bin/bash

# Installer for the GPIO extension on NVIDIA JetPack 6 (Ubuntu 22.04 / aarch64).
# - Ensures linux-libc-dev is present for <linux/gpio.h>
# - Resolves PHP binary and extension dir correctly (handles ondrej/php PPA paths)
# - Builds via Zephir; Zephir is NOT required for pie installs
# - Enables extension across all detected SAPIs (CLI, FPM, Apache)

set -Eeuo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
EXTENSION_NAME="gpio"
BUILD_SO="${SCRIPT_DIR}/ext/modules/${EXTENSION_NAME}.so"
LOG_FILE="${SCRIPT_DIR}/build.log"

if [ "${EUID:-$(id -u)}" -ne 0 ]; then
    SUDO="sudo"
else
    SUDO=""
fi

die() {
    echo ""
    echo "❌ $*"
    exit 1
}

require_cmd() {
    local cmd="$1"
    command -v "$cmd" >/dev/null 2>&1 || die "Required command not found: $cmd"
}

header() {
    echo "=========================================="
    echo " GPIO Extension Installer (JetPack 6)"
    echo "=========================================="
    echo ""
}

step() { echo "$*"; }
ok()   { echo "   ✓ $*"; }

show_failure_logs() {
    if [ -f "$LOG_FILE" ]; then
        echo ""
        echo "---- Last 80 lines of ${LOG_FILE} ----"
        tail -80 "$LOG_FILE" || true
    fi
}

header

# ── Preflight ────────────────────────────────────────────────────────────────
step "🔎 Preflight checks..."
require_cmd php
require_cmd php-config

# Ensure <linux/gpio.h> is available. On JetPack 6 the Jetson kernel headers
# ship via linux-libc-dev; the standard Ubuntu package provides the uAPI.
if ! [ -f /usr/include/linux/gpio.h ]; then
    step "   Installing linux-libc-dev for <linux/gpio.h>..."
    $SUDO apt-get install -y --no-install-recommends linux-libc-dev
fi
ok "linux/gpio.h present"

# Resolve Zephir
if [ -n "${ZEPHIR_BIN:-}" ]; then
    ZEPHIR="$ZEPHIR_BIN"
elif command -v zephir >/dev/null 2>&1; then
    ZEPHIR="$(command -v zephir)"
elif [ -x "$HOME/.config/composer/vendor/bin/zephir" ]; then
    ZEPHIR="$HOME/.config/composer/vendor/bin/zephir"
else
    die "Zephir not found. Install via: composer global require phalcon/zephir  (or set ZEPHIR_BIN)"
fi
ok "Found zephir: $ZEPHIR"

PHP_VER_MM="$(php -r 'echo PHP_MAJOR_VERSION.".".PHP_MINOR_VERSION;')"
PHP_VER_NN="$(php -r 'echo PHP_MAJOR_VERSION.PHP_MINOR_VERSION;')"

# Use PHP_BINARY to find the real binary (ondrej/php PPA installs to versioned
# paths like /usr/bin/php8.3; using PHP_BINARY resolves whatever `php` points to).
PHP_BIN_REAL="$(php -r 'echo PHP_BINARY;' 2>/dev/null)"
PHP_BIN_DIR="$(dirname "$PHP_BIN_REAL")"
RESOLVED_PHP_CONFIG="${PHP_BIN_DIR}/php-config"

if [ -x "$RESOLVED_PHP_CONFIG" ]; then
    PHP_EXT_DIR="$("$RESOLVED_PHP_CONFIG" --extension-dir)"
elif command -v php-config >/dev/null 2>&1; then
    PHP_EXT_DIR="$(php-config --extension-dir)"
else
    die "Could not locate php-config. Install php${PHP_VER_MM}-dev or set PHP_EXT_DIR manually."
fi
[ -n "$PHP_EXT_DIR" ] || die "Could not determine PHP extension dir."

# On setups where the CLI scan dir already contains .so files, prefer it.
CLI_SCAN_DIR="$(php --ini 2>/dev/null | awk -F': ' '/Scan for additional \.ini files in:/{print $2}' || true)"
if [ -n "$CLI_SCAN_DIR" ] && [ -d "$CLI_SCAN_DIR" ] && ls "$CLI_SCAN_DIR"/*.so >/dev/null 2>&1; then
    PHP_EXT_DIR="$CLI_SCAN_DIR"
fi

ok "PHP version: ${PHP_VER_MM}"
ok "PHP binary:  ${PHP_BIN_REAL}"
ok "Ext dir:     ${PHP_EXT_DIR}"

# GCC on Ubuntu 22.04 ships gcc-11/12; these don't have the GCC-14 hard-error
# behaviour, but setting the flags is harmless and future-proofs for upgrades.
export CFLAGS="${CFLAGS:-} -Wno-error -Wno-error=incompatible-pointer-types -Wno-error=int-conversion -Wno-pointer-compare"
export CPPFLAGS="${CPPFLAGS:-} -Wno-error -Wno-error=incompatible-pointer-types"
echo ""

# ── Clean ────────────────────────────────────────────────────────────────────
step "🧹 Cleaning previous build..."
cd "${SCRIPT_DIR}"
if ! "$ZEPHIR" fullclean >"$LOG_FILE" 2>&1; then
    show_failure_logs
    die "Clean failed. See ${LOG_FILE}."
fi
ok "Clean complete"
echo ""

# ── Build ────────────────────────────────────────────────────────────────────
step "🔨 Building extension..."
if ! "$ZEPHIR" build >>"$LOG_FILE" 2>&1 || [ ! -f "$BUILD_SO" ]; then
    show_failure_logs
    die "Build failed or output not found at ${BUILD_SO}."
fi
ok "Build complete"
echo ""

# ── Install .so ──────────────────────────────────────────────────────────────
step "📦 Installing binary..."
$SUDO mkdir -p "$PHP_EXT_DIR"
$SUDO cp -f "$BUILD_SO" "${PHP_EXT_DIR}/${EXTENSION_NAME}.so"
$SUDO chmod 755 "${PHP_EXT_DIR}/${EXTENSION_NAME}.so"
ok "Copied to: ${PHP_EXT_DIR}/${EXTENSION_NAME}.so"
echo ""

# ── Enable across SAPIs ──────────────────────────────────────────────────────
step "⚙️  Enabling extension..."
declare -a CONF_DIR_CANDIDATES=()

if [ -n "$CLI_SCAN_DIR" ] && [ "$CLI_SCAN_DIR" != "(none)" ] && [ -d "$CLI_SCAN_DIR" ]; then
    CONF_DIR_CANDIDATES+=("$CLI_SCAN_DIR")
fi

for d in "/etc/php/${PHP_VER_MM}/cli/conf.d" "/etc/php/${PHP_VER_MM}/fpm/conf.d" "/etc/php/${PHP_VER_MM}/apache2/conf.d"; do
    [ -d "$d" ] && CONF_DIR_CANDIDATES+=("$d")
done

ALPINE_CONF="/etc/php${PHP_VER_NN}/conf.d"
[ -d "$ALPINE_CONF" ] && CONF_DIR_CANDIDATES+=("$ALPINE_CONF")

for d in "/etc/php-fpm.d" "/etc/php-fpm/conf.d"; do
    [ -d "$d" ] && CONF_DIR_CANDIDATES+=("$d")
done

CONF_DIRS=()
while IFS= read -r _line; do
    CONF_DIRS+=("$_line")
done < <(printf "%s\n" "${CONF_DIR_CANDIDATES[@]}" | awk '!seen[$0]++')

if [ "${#CONF_DIRS[@]}" -eq 0 ]; then
    echo "   ⚠️  No conf.d directories found; enabling only for current CLI context."
fi

INI_NAME="30-${EXTENSION_NAME}.ini"
INI_CONTENT="extension=${PHP_EXT_DIR}/${EXTENSION_NAME}.so"

for confd in "${CONF_DIRS[@]:-}"; do
    INI_PATH="${confd}/${INI_NAME}"
    echo "$INI_CONTENT" | $SUDO tee "$INI_PATH" >/dev/null
    ok "Written: $INI_PATH"
done
echo ""

# ── Verify ───────────────────────────────────────────────────────────────────
step "🔍 Verifying installation (CLI)..."
VERIFY_ERRORS="$("$PHP_BIN_REAL" -m 2>&1 >/dev/null || true)"
if "$PHP_BIN_REAL" -m 2>/dev/null | grep -q "^${EXTENSION_NAME}$"; then
    ok "Extension loaded successfully in CLI"
    [ -n "$VERIFY_ERRORS" ] && echo "   ⚠️  PHP startup warnings: $VERIFY_ERRORS"
else
    echo "   PHP binary:  $PHP_BIN_REAL"
    echo "   Ext dir:     $PHP_EXT_DIR"
    [ -n "$VERIFY_ERRORS" ] && echo "   PHP stderr:  $VERIFY_ERRORS"
    die "Extension not detected in CLI. Check ${INI_NAME} placement and php --ini."
fi
echo ""

# ── FPM reload ───────────────────────────────────────────────────────────────
if command -v systemctl >/dev/null 2>&1; then
    for svc in "php${PHP_VER_MM}-fpm" "php-fpm"; do
        if systemctl is-active --quiet "${svc}.service" 2>/dev/null; then
            step "🔁 Reloading ${svc}..."
            $SUDO systemctl reload "${svc}" || true
            ok "${svc} reloaded"
            break
        fi
    done
fi

step "=========================================="
step " Extension Information (CLI)"
step "=========================================="
"$PHP_BIN_REAL" --ri "${EXTENSION_NAME}" || true
echo ""

echo "✅  Installation complete!"
echo ""
echo "File locations:"
echo "  • Binary: ${PHP_EXT_DIR}/${EXTENSION_NAME}.so"
if [ "${#CONF_DIRS[@]}" -gt 0 ]; then
    for d in "${CONF_DIRS[@]}"; do
        echo "  • Config: ${d}/${INI_NAME}"
    done
else
    echo "  • Config: (check php --ini)"
fi
echo ""
