# -*-makefile-*-
#
# Copyright (C) 2011 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_TRACE_CMD) += trace-cmd

#
# Paths and names
#
TRACE_CMD_VERSION	:= 2.9.7
TRACE_CMD_MD5		:= 003988f6dd7cac3bf1da04af03d50b5b
TRACE_CMD		:= trace-cmd-v$(TRACE_CMD_VERSION)
TRACE_CMD_SUFFIX	:= tar.gz
TRACE_CMD_URL		:= https://git.kernel.org/pub/scm/utils/trace-cmd/trace-cmd.git/snapshot/$(TRACE_CMD).$(TRACE_CMD_SUFFIX)
TRACE_CMD_SOURCE	:= $(SRCDIR)/$(TRACE_CMD).$(TRACE_CMD_SUFFIX)
TRACE_CMD_DIR		:= $(BUILDDIR)/$(TRACE_CMD)
TRACE_CMD_LICENSE	:= GPL-2.0-only

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

TRACE_CMD_CONF_TOOL	:= NO

TRACE_CMD_MAKE_ENV	:= \
	$(CROSS_ENV) \
	CROSS_COMPILE=$(COMPILER_PREFIX) \
	NO_PYTHON=1 \
	prefix=/usr \
	libdir_relative=lib

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

TRACE_CMD_PLUGINS	:= \
	blk \
	cfg80211 \
	futex \
	function \
	hrtimer \
	jbd2 \
	kmem \
	kvm \
	mac80211 \
	sched_switch \
	scsi \
	tlb \
	xen


$(STATEDIR)/trace-cmd.targetinstall:
	@$(call targetinfo)

	@$(call install_init, trace-cmd)
	@$(call install_fixup, trace-cmd,PRIORITY,optional)
	@$(call install_fixup, trace-cmd,SECTION,base)
	@$(call install_fixup, trace-cmd,AUTHOR,"Michael Olbrich <m.olbrich@pengutronix.de>")
	@$(call install_fixup, trace-cmd,DESCRIPTION,missing)

	@$(call install_copy, trace-cmd, 0, 0, 0755, -, /usr/bin/trace-cmd)

	@$(foreach plugin, $(TRACE_CMD_PLUGINS), \
		$(call install_lib, trace-cmd, 0, 0, 0644, \
			traceevent/plugins/plugin_$(plugin))$(ptx/nl))

	@$(call install_finish, trace-cmd)

	@$(call touch)

# vim: syntax=make
