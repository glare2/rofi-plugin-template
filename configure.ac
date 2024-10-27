AC_INIT([rofi-world], [0.0.1], [https://my-neat-plugin.org/], [], [https://support.my-neat-plugin.org/])

AC_CONFIG_HEADERS([config.h])

AC_CONFIG_MACRO_DIRS([m4])

# Setup automake to be silent and in foreign mode.
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects dist-xz])
AM_SILENT_RULES([yes])

# Check for compiler
AC_PROG_CC

# Set CFLAGS
AM_CFLAGS="-Wall -Wextra -Wparentheses -Winline -pedantic -Wunreachable-code"

# System extensions and other programs
AC_USE_SYSTEM_EXTENSIONS
AM_PROG_AR
AM_PROG_CC_C_O

# Dependency checks
PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([glib], [glib-2.0 >= 2.40 gio-unix-2.0 gmodule-2.0])
PKG_CHECK_MODULES([rofi], [rofi])

LT_INIT([disable-static])

AC_SUBST([AM_CFLAGS])

# Configure Makefile output
AC_CONFIG_FILES([Makefile])

AC_OUTPUT
