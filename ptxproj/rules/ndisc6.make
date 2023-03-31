# -*-makefile-*-
#
# Copyright (C) 2022 by WAGO Gmbh & Co. KG
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_NDISC6) += ndisc6

#
# Paths and names
#
NDISC6_VERSION	:= 1.0.5
NDISC6_MD5		:= c3edfc01f597afc095b4fe895b86dfe0
NDISC6			:= ndisc6-$(NDISC6_VERSION)
NDISC6_SUFFIX	:= tar.bz2
NDISC6_URL		:= https://www.remlab.net/files/ndisc6/ndisc6-1.0.5.tar.bz2
NDISC6_SOURCE	:= $(SRCDIR)/$(NDISC6).$(NDISC6_SUFFIX)
NDISC6_DIR		:= $(BUILDDIR)/$(NDISC6)
NDISC6_LICENSE	:= GPLv2

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

NDISC6_CONF_ENV	:= $(CROSS_ENV)

#
# autoconf
#
NDISC6_CONF_TOOL	:= autoconf
NDISC6_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--enable-assert=no

#$(STATEDIR)/ndisc6.prepare:
#	@$(call targetinfo)
#	@$(call world/prepare, NDISC6)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/ndisc6.compile:
	@$(call targetinfo)
	@$(call world/compile, NDISC6)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/ndisc6.install:
#	@$(call targetinfo)
#	@$(call world/install, NDISC6)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/ndisc6.targetinstall:
	@$(call targetinfo)

	@$(call install_init, ndisc6)
	@$(call install_fixup, ndisc6, PRIORITY, optional)
	@$(call install_fixup, ndisc6, SECTION, base)
	@$(call install_fixup, ndisc6, AUTHOR, "WAGO Gmbh \& Co. KG")
	@$(call install_fixup, ndisc6, DESCRIPTION, "ndisc6")

ifdef PTXCONF_NDISC6_NDISC6
	@$(call install_copy, ndisc6, 0, 0, 0755, -, /usr/bin/ndisc6)
endif

ifdef PTXCONF_NDISC6_RDISC6
	@$(call install_copy, ndisc6, 0, 0, 0755, -, /usr/bin/rdisc6)
endif

ifdef PTXCONF_NDISC6_TCPTRACEROUTE6
	@$(call install_copy, ndisc6, 0, 0, 0755, -, /usr/bin/tcptraceroute6)
endif

ifdef PTXCONF_NDISC6_TRACEROUTE6
	@$(call install_copy, ndisc6, 0, 0, 0755, -, /usr/bin/rltraceroute6)
endif

ifdef PTXCONF_NDISC6_RDNSSD
	@$(call install_copy, ndisc6, 0, 0, 0755, -, /usr/sbin/rdnssd)
	
	@$(call install_alternative, ndisc6, 0, 0, 0755, /etc/init.d/rdnssd)
	@$(call install_link, ndisc6, ../init.d/rdnssd, /etc/rc.d/S23_rdnssd)
endif

	@$(call install_finish, ndisc6)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/ndisc6.clean:
	@$(call targetinfo)
	@$(call clean_pkg, NDISC6)

# vim: syntax=make
