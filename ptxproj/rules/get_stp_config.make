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
PACKAGES-$(PTXCONF_GET_STP_CONFIG) += get_stp_config

#
# Paths and names
#
GET_STP_CONFIG_VERSION        := 1.0.0
GET_STP_CONFIG_MD5            :=
GET_STP_CONFIG                := get_stp_config
GET_STP_CONFIG_BUILDCONFIG    := Release
#GET_STP_CONFIG_SRC_DIR        := $(call ptx/in-path, PTXDIST_PATH, local_src/config-tools/$(GET_STP_CONFIG))
GET_STP_CONFIG_SRC_DIR        := $(PTXDIST_WORKSPACE)/local_src/$(GET_STP_CONFIG)
GET_STP_CONFIG_BUILDROOT_DIR  := $(BUILDDIR)/$(GET_STP_CONFIG)
GET_STP_CONFIG_DIR            := $(GET_STP_CONFIG_BUILDROOT_DIR)/src
GET_STP_CONFIG_BUILD_DIR      := $(GET_STP_CONFIG_BUILDROOT_DIR)/bin/$(GET_STP_CONFIG_BUILDCONFIG)
GET_STP_CONFIG_LICENSE        := MPL-2.0
GET_STP_CONFIG_CONF_TOOL      := NO
GET_STP_CONFIG_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(GET_STP_CONFIG_BUILDCONFIG) \
BIN_DIR=$(GET_STP_CONFIG_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build \
#PTXDIST_PACKAGE_MK_FILE=$(call ptx/in-path, PTXDIST_PATH, rules/get_stp_config.make)


# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# During BSP creation local_src is deleted and the source code directories are
# copied on demand. To handle this condition an order-only dependency on
# the source code directory is created. When it is missing, the target below
# is executed and an error message is generated.
$(GET_STP_CONFIG_SRC_DIR):
	@echo "Error: $@: directory not found!" >&2; exit 2

$(STATEDIR)/get_stp_config.extract:  | $(GET_STP_CONFIG_SRC_DIR)
	@$(call targetinfo)
	@mkdir -p $(GET_STP_CONFIG_BUILDROOT_DIR)
	@if [ ! -L $(GET_STP_CONFIG_DIR) ]; then \
	  ln -s $(GET_STP_CONFIG_SRC_DIR) $(GET_STP_CONFIG_DIR); \
	fi
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/get_stp_config.prepare:
	@$(call targetinfo)
	@$(call world/prepare, GET_STP_CONFIG)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/get_stp_config.compile:
	@$(call targetinfo)
	@$(call world/compile, GET_STP_CONFIG)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/get_stp_config.install:
	@$(call targetinfo)

	@$(call world/install, GET_STP_CONFIG)


	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/get_stp_config.targetinstall:
	@$(call targetinfo)

	@$(call install_init, get_stp_config)
	@$(call install_fixup, get_stp_config,PRIORITY,optional)
	@$(call install_fixup, get_stp_config,SECTION,base)
	@$(call install_fixup, get_stp_config,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, get_stp_config,DESCRIPTION,missing)

	@$(call install_copy, get_stp_config, 0, 0, 0755, $(GET_STP_CONFIG_BUILD_DIR)/get_stp_config.elf, /etc/config-tools/get_stp_config)

	@$(call install_finish, get_stp_config)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/get_stp_config.clean:
	@$(call targetinfo)
	@if [ -d $(GET_STP_CONFIG_DIR) ]; then \
		$(GET_STP_CONFIG_MAKE_ENV) $(GET_STP_CONFIG_PATH) $(MAKE) $(MFLAGS) -C $(GET_STP_CONFIG_DIR) clean; \
	fi
	@$(call clean_pkg, GET_STP_CONFIG)
	@rm -rf $(GET_STP_CONFIG_BUILDROOT_DIR)

# vim: syntax=make
