# -*-makefile-*-
#
# Copyright (C) 2003 by Auerswald GmbH & Co. KG, Schandelah, Germany
#               2002-2008 by Pengutronix e.K., Hildesheim, Germany
#               2009, 2012 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_GDBSERVER) += gdbserver

GDBSERVER_VERSION	 = $(SHARED_GDB_VERSION)
GDBSERVER_MD5		 = $(SHARED_GDB_MD5)
GDBSERVER		:= gdb-$(GDBSERVER_VERSION)
GDBSERVER_SUFFIX	:= tar.xz
GDBSERVER_SOURCE	:= $(SRCDIR)/$(GDBSERVER).$(GDBSERVER_SUFFIX)
GDBSERVER_DIR		:= $(BUILDDIR)/gdbserver-$(GDBSERVER_VERSION)
GDBSERVER_PKGDIR	:= $(PKGDIR)/gdbserver-$(GDBSERVER_VERSION)
GDBSERVER_LICENSE	:= GPL-2.0-only AND GPL-3.0-or-later
GDBSERVER_LICENSE_FILES	:= \
	file://COPYING;md5=59530bdf33659b29e73d4adb9f9f6552 \
	file://COPYING3;md5=d32239bcb673463ab874e80d47fae504 \
	file://COPYING.LIB;md5=9f604d8a4f8e74f4f5140845a21b6674 \
	file://COPYING3.LIB;md5=6a6a8e020838b23406c81b19c1d46df6

GDBSERVER_URL := \
	$(call ptx/mirror, GNU, gdb/$(GDBSERVER).$(GDBSERVER_SUFFIX)) \
	ftp://sourceware.org/pub/gdb/snapshots/current/$(GDBSERVER).$(GDBSERVER_SUFFIX)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

ifdef PTXCONF_ARCH_X86
GDBSERVER_WRAPPER_BLACKLIST := \
	TARGET_HARDEN_PIE
endif

GDBSERVER_ENV := $(GDB_ENV)

ifndef PTXCONF_GDBSERVER_SHARED
GDBSERVER_ENV +=  LDFLAGS=-static
endif

GDBSERVER_CONF_TOOL := autoconf
GDBSERVER_CONF_OPT  := \
	$(CROSS_AUTOCONF_USR) \
	--disable-werror

GDBSERVER_BUILD_OOT := YES

ifeq ($(filter 1%,$(GDBSERVER_VERSION)),)
# version < 10
GDBSERVER_SUBDIR := gdb/gdbserver
else
GDBSERVER_MAKE_OPT := all-gdbserver
GDBSERVER_INSTALL_OPT := install-gdbserver
endif

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/gdbserver.targetinstall:
	@$(call targetinfo)

	@$(call install_init, gdbserver)
	@$(call install_fixup, gdbserver,PRIORITY,optional)
	@$(call install_fixup, gdbserver,SECTION,base)
	@$(call install_fixup, gdbserver,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, gdbserver,DESCRIPTION,missing)

	@$(call install_copy, gdbserver, 0, 0, 0755, -, /usr/bin/gdbserver)

	@$(call install_finish, gdbserver)

	@$(call touch)

# vim: syntax=make