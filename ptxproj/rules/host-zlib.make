# -*-makefile-*-
#
# Copyright (C) 2006 by Pengutronix e.K., Hildesheim, Germany
#               2009 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_ZLIB) += host-zlib

#
# Paths and names
#
HOST_ZLIB_DIR	= $(HOST_BUILDDIR)/$(ZLIB)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
HOST_ZLIB_CONF_TOOL	:= autoconf

# vim: syntax=make
