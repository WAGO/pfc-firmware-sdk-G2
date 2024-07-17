# -*-makefile-*-
#
# Copyright (C) 2023 by wago
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_MSTPD) += mstpd

#
# Paths and names
#
MSTPD_VERSION	:= 0.1.0
MSTPD_MD5		:= 114f645aa6c447c04e0e575d9c041d08
MSTPD			:= mstpd-$(MSTPD_VERSION)
MSTPD_SUFFIX    := tar.gz
MSTPD_URL		:= https://github.com/mstpd/mstpd/archive/refs/tags/$(MSTPD_VERSION).$(MSTPD_SUFFIX)
MSTPD_SOURCE	:= $(SRCDIR)/$(MSTPD).$(MSTPD_SUFFIX)
MSTPD_DIR		:= $(BUILDDIR)/$(MSTPD)
MSTPD_LICENSE	:= GPL-2.0-only

#MSTPD_BUILD_OOT	:= YES

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#MSTPD_CONF_ENV	:= $(CROSS_ENV)

#
# autoconf
#
MSTPD_CONF_TOOL	:= autoconf
MSTPD_CONF_OPT	:= $(CROSS_AUTOCONF_USR)

#$(STATEDIR)/mstpd.prepare:
#	@$(call targetinfo)
#	@$(call world/prepare, MSTPD)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/mstpd.compile:
#	@$(call targetinfo)
#	@$(call world/compile, MSTPD)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/mstpd.install:
#	@$(call targetinfo)
#	@$(call world/install, MSTPD)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/mstpd.targetinstall:
	@$(call targetinfo)

	@$(call install_init, mstpd)
	@$(call install_fixup, mstpd, PRIORITY, optional)
	@$(call install_fixup, mstpd, SECTION, base)
	@$(call install_fixup, mstpd, AUTHOR, "wago")
	@$(call install_fixup, mstpd, DESCRIPTION, "TODO mstpd")

	@$(call install_copy, mstpd, 0, 0, 0755, $(MSTPD_DIR)/mstpd, /usr/sbin/mstpd)
	@$(call install_copy, mstpd, 0, 0, 0755, $(MSTPD_DIR)/mstpctl, /usr/sbin/mstpctl)
	@$(call install_copy, mstpd, 0, 0, 0755, $(MSTPD_DIR)/bridge-stp, /usr/sbin/bridge-stp)
	@$(call install_copy, mstpd, 0, 0, 0755, $(MSTPD_DIR)/utils/ifquery, /lib/mstpctl-utils/ifquery)
	@$(call install_copy, mstpd, 0, 0, 0755, $(MSTPD_DIR)/utils/ifupdown.sh, /lib/mstpctl-utils/ifupdown.sh)
	@$(call install_copy, mstpd, 0, 0, 0755, $(MSTPD_DIR)/utils/mstp_config_bridge, /lib/mstpctl-utils/mstp_config_bridge)
	@$(call install_copy, mstpd, 0, 0, 0755, $(MSTPD_DIR)/bridge-stp.conf, /etc/bridge-stp.conf)
	@$(call install_copy, mstpd, 0, 0, 0644, $(MSTPD_DIR)/LICENSE, /usr/share/licenses/oss/license.mstpd_$(MSTPD_VERSION).txt)

	@$(call install_finish, mstpd)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/mstpd.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, MSTPD)

# vim: syntax=make
