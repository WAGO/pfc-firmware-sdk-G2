# -*-makefile-*-
#
# Copyright (C) 2007 by Sascha Hauer
#           (C) 2010 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_DTC) += host-dtc

#
# Paths and names
#
HOST_DTC_VERSION	:= $(call remove_quotes,$(PTXCONF_HOST_DTC_VERSION))
HOST_DTC_MD5		:= $(call remove_quotes,$(PTXCONF_HOST_DTC_MD5))
HOST_DTC		:= dtc-v$(HOST_DTC_VERSION)
HOST_DTC_SUFFIX		:= tar.gz
HOST_DTC_URL		:= https://git.kernel.org/pub/scm/utils/dtc/dtc.git/snapshot/$(HOST_DTC).$(HOST_DTC_SUFFIX)
HOST_DTC_SOURCE		:= $(SRCDIR)/$(HOST_DTC).$(HOST_DTC_SUFFIX)
HOST_DTC_DIR		:= $(HOST_BUILDDIR)/$(HOST_DTC)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

HOST_DTC_CONF_TOOL := NO
HOST_DTC_MAKE_OPT := \
	PREFIX= \
	NO_PYTHON=1 \
	NO_VALGRIND=1 \
	NO_YAML=1 \
	$(HOST_ENV_CC)

HOST_DTC_INSTALL_OPT := \
	$(HOST_DTC_MAKE_OPT) \
	install

# vim: syntax=make
