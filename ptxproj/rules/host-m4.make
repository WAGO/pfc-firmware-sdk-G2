# -*-makefile-*-
#
# Copyright (C) 2014 by Juergen Beisert <jbe@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_M4) += host-m4

#
# Paths and names
#
HOST_M4_VERSION	:= 1.4.19
HOST_M4_MD5	:= 0d90823e1426f1da2fd872df0311298d
HOST_M4		:= m4-$(HOST_M4_VERSION)
HOST_M4_SUFFIX	:= tar.xz
HOST_M4_URL	:= http://ftp.gnu.org/gnu/m4/$(HOST_M4).$(HOST_M4_SUFFIX)
HOST_M4_SOURCE	:= $(SRCDIR)/$(HOST_M4).$(HOST_M4_SUFFIX)
HOST_M4_DIR	:= $(HOST_BUILDDIR)/$(HOST_M4)
HOST_M4_LICENSE	:= GPL-3.0-only

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
HOST_M4_CONF_TOOL	:= autoconf
HOST_M4_CONF_OPT	:= \
	$(HOST_AUTOCONF) \
	--enable-threads=posix \
	--disable-gcc-warnings \
	--enable-assert \
	--disable-rpath \
	--disable-c++ \
	--disable-changeword \
	--without-included-regex \
	--with-syscmd-shell=/bin/sh \
	--without-dmalloc

# vim: syntax=make
