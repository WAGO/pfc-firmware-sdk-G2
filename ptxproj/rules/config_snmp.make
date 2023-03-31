# -*-makefile-*-
#
# Copyright (C) 2022 by WAGO GmbH \& Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CONFIG_SNMP) += config_snmp

#
# Paths and names
#
CONFIG_SNMP_VERSION        := 1.0.0
CONFIG_SNMP_MD5            :=
CONFIG_SNMP                := config_snmp
CONFIG_SNMP_BUILDCONFIG    := Release
CONFIG_SNMP_SRC_DIR        := $(PTXDIST_WORKSPACE)/local_src/config-tools/$(CONFIG_SNMP)
CONFIG_SNMP_BUILDROOT_DIR  := $(BUILDDIR)/$(CONFIG_SNMP)
CONFIG_SNMP_DIR            := $(CONFIG_SNMP_BUILDROOT_DIR)/src
CONFIG_SNMP_BUILD_DIR      := $(CONFIG_SNMP_BUILDROOT_DIR)/bin/$(CONFIG_SNMP_BUILDCONFIG)
CONFIG_SNMP_LICENSE        := MPL-2.0
CONFIG_SNMP_CONF_TOOL      := NO
CONFIG_SNMP_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(CONFIG_SNMP_BUILDCONFIG) \
BIN_DIR=$(CONFIG_SNMP_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build


# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# During BSP creation local_src is deleted and the source code directories are
# copied on demand. To handle this condition an order-only dependency on
# the source code directory is created. When it is missing, the target below
# is executed and an error message is generated.
$(CONFIG_SNMP_SRC_DIR):
	@echo "Error: $@: directory not found!" >&2; exit 2

$(STATEDIR)/config_snmp.extract:  | $(CONFIG_SNMP_SRC_DIR)
	@$(call targetinfo)
	@mkdir -p $(CONFIG_SNMP_BUILDROOT_DIR)
	@if [ ! -L $(CONFIG_SNMP_DIR) ]; then \
	  ln -s $(CONFIG_SNMP_SRC_DIR) $(CONFIG_SNMP_DIR); \
	fi
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/config_snmp.prepare:
	@$(call targetinfo)
	@$(call world/prepare, CONFIG_SNMP)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/config_snmp.compile:
	@$(call targetinfo)
	@$(call world/compile, CONFIG_SNMP)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/config_snmp.install:
	@$(call targetinfo)

	@$(call world/install, CONFIG_SNMP)

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/config_snmp.targetinstall:
	@$(call targetinfo)

	@$(call install_init, config_snmp)
	@$(call install_fixup, config_snmp,PRIORITY,optional)
	@$(call install_fixup, config_snmp,SECTION,base)
	@$(call install_fixup, config_snmp,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, config_snmp,DESCRIPTION,missing)

	@$(call install_copy, config_snmp, 0, 0, 0755, $(CONFIG_SNMP_BUILD_DIR)/config_snmp.elf, /etc/config-tools/config_snmp)

	@$(call install_finish, config_snmp)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/config_snmp.clean:
	@$(call targetinfo)
	@if [ -d $(CONFIG_SNMP_DIR) ]; then \
		$(CONFIG_SNMP_MAKE_ENV) $(CONFIG_SNMP_PATH) $(MAKE) $(MFLAGS) -C $(CONFIG_SNMP_DIR) clean; \
	fi
	@$(call clean_pkg, CONFIG_SNMP)
	@rm -rf $(CONFIG_SNMP_BUILDROOT_DIR)

# vim: syntax=make
