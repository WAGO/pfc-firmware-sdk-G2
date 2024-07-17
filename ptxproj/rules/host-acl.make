# -*-makefile-*-
#
# Copyright (C) 2009 by Carsten Schlote <c.schlote@konzeptpark.de>
#               2010 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_ACL) += host-acl

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
HOST_ACL_CONF_TOOL	:= autoconf
HOST_ACL_CONF_OPT	:= \
	$(HOST_AUTOCONF) \
	--disable-nls \
	--disable-rpath \
	--disable-debug

# vim: syntax=make