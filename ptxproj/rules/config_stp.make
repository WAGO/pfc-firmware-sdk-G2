# -*-makefile-*-
#
# Copyright (C) 2023 by WAGO GmbH \& Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CONFIG_STP) += config_stp

#
# Paths and names
#
CONFIG_STP_VERSION        := 1.0.1
CONFIG_STP_MD5            :=
CONFIG_STP                := config_stp
CONFIG_STP_BUILDCONFIG    := Release
#CONFIG_STP_SRC_DIR        := $(call ptx/in-path, PTXDIST_PATH, local_src/config-tools/$(CONFIG_STP))
CONFIG_STP_SRC_DIR        := $(PTXDIST_WORKSPACE)/local_src/$(CONFIG_STP)
CONFIG_STP_BUILDROOT_DIR  := $(BUILDDIR)/$(CONFIG_STP)
CONFIG_STP_DIR            := $(CONFIG_STP_BUILDROOT_DIR)/src
CONFIG_STP_BUILD_DIR      := $(CONFIG_STP_BUILDROOT_DIR)/bin/$(CONFIG_STP_BUILDCONFIG)
CONFIG_STP_LICENSE        := MPL-2.0
CONFIG_STP_CONF_TOOL      := NO
CONFIG_STP_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(CONFIG_STP_BUILDCONFIG) \
BIN_DIR=$(CONFIG_STP_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build \
#PTXDIST_PACKAGE_MK_FILE=$(call ptx/in-path, PTXDIST_PATH, rules/config_stp.make)


# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# During BSP creation local_src is deleted and the source code directories are
# copied on demand. To handle this condition an order-only dependency on
# the source code directory is created. When it is missing, the target below
# is executed and an error message is generated.
$(CONFIG_STP_SRC_DIR):
	@echo "Error: $@: directory not found!" >&2; exit 2

$(STATEDIR)/config_stp.extract:  | $(CONFIG_STP_SRC_DIR)
	@$(call targetinfo)
	@mkdir -p $(CONFIG_STP_BUILDROOT_DIR)
	@if [ ! -L $(CONFIG_STP_DIR) ]; then \
	  ln -s $(CONFIG_STP_SRC_DIR) $(CONFIG_STP_DIR); \
	fi
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/config_stp.prepare:
	@$(call targetinfo)
	@$(call world/prepare, CONFIG_STP)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/config_stp.compile:
	@$(call targetinfo)
	@$(call world/compile, CONFIG_STP)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/config_stp.install:
	@$(call targetinfo)

	@$(call world/install, CONFIG_STP)


	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/config_stp.targetinstall:
	@$(call targetinfo)

	@$(call install_init, config_stp)
	@$(call install_fixup, config_stp,PRIORITY,optional)
	@$(call install_fixup, config_stp,SECTION,base)
	@$(call install_fixup, config_stp,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, config_stp,DESCRIPTION,missing)

	@$(call install_copy, config_stp, 0, 0, 0755, $(CONFIG_STP_BUILD_DIR)/config_stp.elf, /etc/config-tools/config_stp)

	@$(call install_finish, config_stp)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/config_stp.clean:
	@$(call targetinfo)
	@if [ -d $(CONFIG_STP_DIR) ]; then \
		$(CONFIG_STP_MAKE_ENV) $(CONFIG_STP_PATH) $(MAKE) $(MFLAGS) -C $(CONFIG_STP_DIR) clean; \
	fi
	@$(call clean_pkg, CONFIG_STP)
	@rm -rf $(CONFIG_STP_BUILDROOT_DIR)

# vim: syntax=make
