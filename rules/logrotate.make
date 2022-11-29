# -*-makefile-*-
#
# Copyright (C) 2006, 2015 by Marc Kleine-Budde <mkl@pengutronix.de>
#           (C) 2010 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LOGROTATE) += logrotate

#
# Paths and names
#
LOGROTATE_BASE_VERSION	:= 3.20.1
LOGROTATE_VERSION		:= $(LOGROTATE_BASE_VERSION)+wago1
LOGROTATE_MD5		:= 2af40d2d2b218f91c6d7a14eda078131
LOGROTATE		:= logrotate-$(LOGROTATE_VERSION)
LOGROTATE_ARCHIVE_NAME  := logrotate-$(LOGROTATE_BASE_VERSION)
LOGROTATE_SUFFIX	:= tar.gz
LOGROTATE_URL		:= https://github.com/logrotate/logrotate/releases/download/$(LOGROTATE_BASE_VERSION)/$(LOGROTATE_ARCHIVE_NAME).$(LOGROTATE_SUFFIX)
LOGROTATE_SOURCE	:= $(SRCDIR)/$(LOGROTATE_ARCHIVE_NAME).$(LOGROTATE_SUFFIX)
LOGROTATE_DIR		:= $(BUILDDIR)/$(LOGROTATE)
LOGROTATE_LICENSE	:= GPL-2.0-only

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

LOGROTATE_CONF_TOOL := autoconf
LOGROTATE_CONF_OPT := \
	$(CROSS_AUTOCONF_USR) \
	$(GLOBAL_LARGE_FILE_OPTION) \
	--$(call ptx/wwo, PTXCONF_LOGROTATE_ACL)-acl \
	--$(call ptx/wwo, PTXCONF_GLOBAL_SELINUX)-selinux

LOGROTATE_MAKE_OPT := AM_CFLAGS="-Wall"

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/logrotate.targetinstall:
	@$(call targetinfo)

	@$(call install_init, logrotate)
	@$(call install_fixup, logrotate,PRIORITY,optional)
	@$(call install_fixup, logrotate,SECTION,base)
	@$(call install_fixup, logrotate,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, logrotate,DESCRIPTION,missing)

	@$(call install_copy, logrotate, 0, 0, 0755, -, /usr/sbin/logrotate)
	@$(call install_alternative, logrotate, 0, 0, 0644, /etc/logrotate.conf)
	@$(call install_copy, logrotate, 0, 0, 0755, /var/lib/logrotate)

# WAGO custom configuration
	@$(call install_copy, logrotate, 0, 0, 0644, $(PTXDIST_WORKSPACE)/projectroot/etc/logrotate.d/wagolog.conf, /etc/logrotate.d/wagolog.conf)

ifdef PTXCONF_LOGROTATE_SYSTEMD_UNIT
	@$(call install_alternative, logrotate, 0, 0, 0644, \
		/usr/lib/systemd/system/logrotate.timer)
	@$(call install_alternative, logrotate, 0, 0, 0644, \
		/usr/lib/systemd/system/logrotate.service)
	@$(call install_link, logrotate, ../logrotate.timer, \
		/usr/lib/systemd/system/multi-user.target.wants/logrotate.timer)
endif

	@$(call install_finish, logrotate)

	@$(call touch)

# vim: syntax=make
