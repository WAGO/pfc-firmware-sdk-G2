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
PACKAGES-$(PTXCONF_CONFIG_OPCUA) += config_opcua

#
# Paths and names
#
CONFIG_OPCUA_VERSION        := 1.0.0
CONFIG_OPCUA_MD5            :=
CONFIG_OPCUA                := config_opcua
CONFIG_OPCUA_BUILDCONFIG    := Release
CONFIG_OPCUA_SRC_DIR        := $(PTXDIST_WORKSPACE)/local_src/config-tools/$(CONFIG_OPCUA)
CONFIG_OPCUA_BUILDROOT_DIR  := $(BUILDDIR)/$(CONFIG_OPCUA)
CONFIG_OPCUA_DIR            := $(CONFIG_OPCUA_BUILDROOT_DIR)/src
CONFIG_OPCUA_BUILD_DIR      := $(CONFIG_OPCUA_BUILDROOT_DIR)/bin/$(CONFIG_OPCUA_BUILDCONFIG)
CONFIG_OPCUA_LICENSE        := MPL-2.0
CONFIG_OPCUA_CONF_TOOL      := NO
CONFIG_OPCUA_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(CONFIG_OPCUA_BUILDCONFIG) \
BIN_DIR=$(CONFIG_OPCUA_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build


# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# During BSP creation local_src is deleted and the source code directories are
# copied on demand. To handle this condition an order-only dependency on
# the source code directory is created. When it is missing, the target below
# is executed and an error message is generated.
$(CONFIG_OPCUA_SRC_DIR):
	@echo "Error: $@: directory not found!" >&2; exit 2

$(STATEDIR)/config_opcua.extract:  | $(CONFIG_OPCUA_SRC_DIR)
	@$(call targetinfo)
	@mkdir -p $(CONFIG_OPCUA_BUILDROOT_DIR)
	@if [ ! -L $(CONFIG_OPCUA_DIR) ]; then \
	  ln -s $(CONFIG_OPCUA_SRC_DIR) $(CONFIG_OPCUA_DIR); \
	fi
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/config_opcua.prepare:
	@$(call targetinfo)
	@$(call world/prepare, CONFIG_OPCUA)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/config_opcua.compile:
	@$(call targetinfo)
	@$(call world/compile, CONFIG_OPCUA)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/config_opcua.install:
	@$(call targetinfo)
	@$(call world/install, CONFIG_OPCUA)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/config_opcua.targetinstall:
	@$(call targetinfo)

	@$(call install_init, config_opcua)
	@$(call install_fixup, config_opcua,PRIORITY,optional)
	@$(call install_fixup, config_opcua,SECTION,base)
	@$(call install_fixup, config_opcua,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, config_opcua,DESCRIPTION,missing)

	@$(call install_copy, config_opcua, 0, 0, 0755, -, /etc/config-tools/config_opcua)

	@$(call install_finish, config_opcua)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/config_opcua.clean:
	@$(call targetinfo)
	@if [ -d $(CONFIG_OPCUA_DIR) ]; then \
		$(CONFIG_OPCUA_MAKE_ENV) $(CONFIG_OPCUA_PATH) $(MAKE) $(MFLAGS) -C $(CONFIG_OPCUA_DIR) clean; \
	fi
	@$(call clean_pkg, CONFIG_OPCUA)
	@rm -rf $(CONFIG_OPCUA_BUILDROOT_DIR)

# vim: syntax=make
