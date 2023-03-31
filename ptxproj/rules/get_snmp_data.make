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
PACKAGES-$(PTXCONF_GET_SNMP_DATA) += get_snmp_data

#
# Paths and names
#
GET_SNMP_DATA_VERSION        := 1.0.0
GET_SNMP_DATA_MD5            :=
GET_SNMP_DATA                := get_snmp_data
GET_SNMP_DATA_BUILDCONFIG    := Release
GET_SNMP_DATA_SRC_DIR        := $(PTXDIST_WORKSPACE)/local_src/config-tools/$(GET_SNMP_DATA)
GET_SNMP_DATA_BUILDROOT_DIR  := $(BUILDDIR)/$(GET_SNMP_DATA)
GET_SNMP_DATA_DIR            := $(GET_SNMP_DATA_BUILDROOT_DIR)/src
GET_SNMP_DATA_BUILD_DIR      := $(GET_SNMP_DATA_BUILDROOT_DIR)/bin/$(GET_SNMP_DATA_BUILDCONFIG)
GET_SNMP_DATA_LICENSE        := MPL-2.0
GET_SNMP_DATA_CONF_TOOL      := NO
GET_SNMP_DATA_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(GET_SNMP_DATA_BUILDCONFIG) \
BIN_DIR=$(GET_SNMP_DATA_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build


# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# During BSP creation local_src is deleted and the source code directories are
# copied on demand. To handle this condition an order-only dependency on
# the source code directory is created. When it is missing, the target below
# is executed and an error message is generated.
$(GET_SNMP_DATA_SRC_DIR):
	@echo "Error: $@: directory not found!" >&2; exit 2

$(STATEDIR)/get_snmp_data.extract:  | $(GET_SNMP_DATA_SRC_DIR)
	@$(call targetinfo)
	@mkdir -p $(GET_SNMP_DATA_BUILDROOT_DIR)
	@if [ ! -L $(GET_SNMP_DATA_DIR) ]; then \
	  ln -s $(GET_SNMP_DATA_SRC_DIR) $(GET_SNMP_DATA_DIR); \
	fi
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/get_snmp_data.prepare:
	@$(call targetinfo)
	@$(call world/prepare, GET_SNMP_DATA)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/get_snmp_data.compile:
	@$(call targetinfo)
	@$(call world/compile, GET_SNMP_DATA)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/get_snmp_data.install:
	@$(call targetinfo)

	@$(call world/install, GET_SNMP_DATA)

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/get_snmp_data.targetinstall:
	@$(call targetinfo)

	@$(call install_init, get_snmp_data)
	@$(call install_fixup, get_snmp_data,PRIORITY,optional)
	@$(call install_fixup, get_snmp_data,SECTION,base)
	@$(call install_fixup, get_snmp_data,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, get_snmp_data,DESCRIPTION,missing)

	@$(call install_copy, get_snmp_data, 0, 0, 0755, $(GET_SNMP_DATA_BUILD_DIR)/get_snmp_data.elf, /etc/config-tools/get_snmp_data)

	@$(call install_finish, get_snmp_data)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/get_snmp_data.clean:
	@$(call targetinfo)
	@if [ -d $(GET_SNMP_DATA_DIR) ]; then \
		$(GET_SNMP_DATA_MAKE_ENV) $(GET_SNMP_DATA_PATH) $(MAKE) $(MFLAGS) -C $(GET_SNMP_DATA_DIR) clean; \
	fi
	@$(call clean_pkg, GET_SNMP_DATA)
	@rm -rf $(GET_SNMP_DATA_BUILDROOT_DIR)

# vim: syntax=make
