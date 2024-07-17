# -*-makefile-*-
#
# Copyright (C) 2002-2007 by Pengutronix e.K., Hildesheim, Germany
#               2009 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_ZLIB) += zlib

#
# Paths and names
#
ZLIB_VERSION	:= 1.3.1
ZLIB_MD5	:= 5e6af153311327e516690d300527ec9e
ZLIB		:= zlib-$(ZLIB_VERSION)
ZLIB_SUFFIX	:= tar.xz
ZLIB_URL	:= \
	http://zlib.net/$(ZLIB).$(ZLIB_SUFFIX) \
	$(call ptx/mirror, SF, libpng/$(ZLIB).$(ZLIB_SUFFIX))
ZLIB_SOURCE	:= $(SRCDIR)/$(ZLIB).$(ZLIB_SUFFIX)
ZLIB_DIR	:= $(BUILDDIR)/$(ZLIB)
ZLIB_LICENSE	:= Zlib
ZLIB_LICENSE_FILES := \
	file://LICENSE;md5=b51a40671bc46e961c0498897742c0b8

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

ZLIB_CONF_ENV := \
	$(CROSS_ENV) \
	CROSS_PREFIX=$(PTXCONF_COMPILER_PREFIX)

#
# autoconf
#
ZLIB_CONF_TOOL := autoconf
ZLIB_CONF_OPT := \
	--prefix=/usr \
	--uname=Linux \
	--libdir=/usr/$(CROSS_LIB_DIR)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/zlib.targetinstall:
	@$(call targetinfo)

	@$(call install_init, zlib)
	@$(call install_fixup, zlib,PRIORITY,optional)
	@$(call install_fixup, zlib,SECTION,base)
	@$(call install_fixup, zlib,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, zlib,DESCRIPTION,missing)

	@$(call install_lib, zlib, 0, 0, 0644, libz)

	@$(call install_finish, zlib)
	@$(call touch)

# vim: syntax=make
