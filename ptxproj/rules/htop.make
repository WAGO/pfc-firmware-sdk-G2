# -*-makefile-*-
#
# Copyright (C) 2007 by Robert Schwebel
#               2010 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_HTOP) += htop

#
# Paths and names
#
HTOP_VERSION		:= 3.0.1
HTOP_MD5		:= c09908bacb5e22454715547aed88c3af
HTOP			:= htop-$(HTOP_VERSION)
HTOP_SUFFIX		:= tar.gz
HTOP_URL		:= https://github.com/htop-dev/htop/archive/$(HTOP_VERSION).$(HTOP_SUFFIX)
HTOP_SOURCE		:= $(SRCDIR)/$(HTOP).$(HTOP_SUFFIX)
HTOP_DIR		:= $(BUILDDIR)/$(HTOP)
HTOP_LICENSE		:= GPL-2.0-only
HTOP_LICENSE_FILES	:= \
	file://COPYING;md5=4099d367cd5e59b6d4fc1ee33accb891

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

HTOP_CONF_ENV	:= \
	$(CROSS_ENV) \
	HTOP_NCURSES_CONFIG_SCRIPT=ncurses6-config
#
# autoconf
#
HTOP_CONF_TOOL	:= autoconf
HTOP_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--disable-openvz \
	--enable-cgroup \
	--disable-vserver \
	--disable-ancient-vserver \
	--enable-taskstats \
	--disable-unicode \
	--enable-linux-affinity \
	--disable-hwloc \
	--disable-setuid \
	--disable-delayacct

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/htop.targetinstall:
	@$(call targetinfo)

	@$(call install_init, htop)
	@$(call install_fixup, htop,PRIORITY,optional)
	@$(call install_fixup, htop,SECTION,base)
	@$(call install_fixup, htop,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, htop,DESCRIPTION,missing)

	@$(call install_copy, htop, 0, 0, 0755, -, /usr/bin/htop)

	@$(call install_finish, htop)

	@$(call touch)

# vim: syntax=make
