# -*-makefile-*-
# $Id$
#
# Copyright (C) 2008 by Stefanie Meihöfer
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# TODO: install modules according to xml configuration
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CBM) += cbm

#
# Paths and names
#
CBM_VERSION 	:= 2.02.00
CBM		:= cbm
CBM_URL		:= /$(CBM)
CBM_SOURCE	:=
CBM_DIR		:= $(BUILDDIR)/$(CBM)
CBM_SRC_DIR := $(PTXDIST_WORKSPACE)/local_src/cbm


# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

#
# Dummy get target - nothing to fetch here
# $(STATEDIR)/cbm.get is rebuilt implicitely
#

$(CBM_SOURCE):
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

#
# During BSP creation local_src is deleted and the source code directories are
# copied on demand. To handle this condition an order-only dependency on
# the source code directory is created. When it is missing, the target below
# is executed and an error message is generated.
$(CBM_SRC_DIR):
	@echo "Error: $@: directory not found!" >&2; \
	exit 2
#

$(STATEDIR)/cbm.extract: | $(CBM_SRC_DIR)
	@$(call targetinfo)
	@mkdir -p $(CBM_DIR)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#$(STATEDIR)/cbm.prepare: $(STATEDIR)/host-python.install.post
#  @$(call targetinfo)
#  @$(call world/prepare, CBM)
#  @$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/cbm.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cbm.install: $(cbm_install_deps_default)
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cbm.targetinstall:
	@$(call targetinfo)

	@$(call install_init, cbm)
	@$(call install_fixup,cbm,PRIORITY,optional)
	@$(call install_fixup,cbm,SECTION,base)
	@$(call install_fixup,cbm,AUTHOR,"WAGO")
	@$(call install_fixup,cbm,DESCRIPTION,missing)

ifdef PTXCONF_CT_WDIALOG
	@$(call install_copy, cbm, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/wdialog, /etc/config-tools/wdialog)
endif

ifdef PTXCONF_CBM
	@$(call install_copy, cbm, 0, 0, 0755, $(CBM_SRC_DIR)/etc/init.d/cbm, /etc/init.d/cbm, n)
	@$(call install_copy, cbm, 0, 0, 0750, $(CBM_SRC_DIR)/usr/sbin/cbm, /usr/sbin/cbm)

	# Main Script
	@$(call install_copy, cbm, 0, 0, 0750, $(CBM_SRC_DIR)/etc/config-tools/cbm, /etc/config-tools/cbm, n)

	# Additional Modules

	@$(call install_copy, cbm, 0, 0, 0750, /etc/config-tools/cbm-script-modules);
	@for script in $(CBM_SRC_DIR)/etc/config-tools/cbm-script-modules/*.sh ; do \
		$(call install_copy, cbm, 0, 0, 0640, $$script, /etc/config-tools/cbm-script-modules/$$(basename $$script), n); \
	done


	@$(call install_copy, cbm, 0, 0, 0644, $(CBM_SRC_DIR)/etc/config-tools/frontend_config_cbm.xml, /etc/config-tools/frontend_config_cbm.xml, n)

endif # PTXCONF_CBM

	@$(call install_finish,cbm)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

cbm_clean:
	rm -rf $(CBM_DIR)
	rm -rf $(STATEDIR)/cbm.*


# vim: syntax=make
