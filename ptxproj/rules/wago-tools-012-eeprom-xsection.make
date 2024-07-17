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
PACKAGES-$(PTXCONF_EEPROM_XSECTION) += eeprom-xsection

#
# Paths and names
#
EEPROM_XSECTION_VERSION        := 0.5.0
EEPROM_XSECTION_MD5            :=
EEPROM_XSECTION                := eeprom-xsection
EEPROM_XSECTION_BUILDCONFIG    := Release
EEPROM_XSECTION_SRC_DIR        := $(PTXDIST_WORKSPACE)/local_src/$(EEPROM_XSECTION)
EEPROM_XSECTION_BUILDROOT_DIR  := $(BUILDDIR)/$(EEPROM_XSECTION)
EEPROM_XSECTION_DIR            := $(EEPROM_XSECTION_BUILDROOT_DIR)/src
EEPROM_XSECTION_BUILD_DIR      := $(EEPROM_XSECTION_BUILDROOT_DIR)/bin/$(EEPROM_XSECTION_BUILDCONFIG)
EEPROM_XSECTION_LICENSE        := WAGO
EEPROM_XSECTION_CONF_TOOL      := NO
EEPROM_XSECTION_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(EEPROM_XSECTION_BUILDCONFIG) \
BIN_DIR=$(EEPROM_XSECTION_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build \
PTXDIST_PACKAGE_MK_FILE=$(call ptx/in-path, PTXDIST_PATH, rules/wago-tools-012-eeprom-xsection.make)


# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# During BSP creation local_src is deleted and the source code directories are
# copied on demand. To handle this condition an order-only dependency on
# the source code directory is created. When it is missing, the target below
# is executed and an error message is generated.
$(EEPROM_XSECTION_SRC_DIR):
	@echo "Error: $@: directory not found!" >&2; exit 2

$(STATEDIR)/eeprom-xsection.extract:  | $(EEPROM_XSECTION_SRC_DIR)
	@$(call targetinfo)
	@mkdir -p $(EEPROM_XSECTION_BUILDROOT_DIR)
	@if [ ! -L $(EEPROM_XSECTION_DIR) ]; then \
	  ln -s $(EEPROM_XSECTION_SRC_DIR) $(EEPROM_XSECTION_DIR); \
	fi
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/eeprom-xsection.prepare:
	@$(call targetinfo)
	@$(call world/prepare, EEPROM_XSECTION)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/eeprom-xsection.compile:
	@$(call targetinfo)
	@$(call world/compile, EEPROM_XSECTION)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/eeprom-xsection.install:
#	@$(call targetinfo)
#	@$(call world/install, EEPROM_XSECTION)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/eeprom-xsection.targetinstall:
	@$(call targetinfo)

	@$(call install_init, eeprom-xsection)
	@$(call install_fixup, eeprom-xsection,PRIORITY,optional)
	@$(call install_fixup, eeprom-xsection,SECTION,base)
	@$(call install_fixup, eeprom-xsection,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, eeprom-xsection,DESCRIPTION,missing)

	@$(call install_copy, eeprom-xsection, 0, 0, 0755, $(EEPROM_XSECTION_BUILD_DIR)/eeprom-xsection.elf, /usr/sbin/eeprom-xsection)

	@$(call install_finish, eeprom-xsection)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/eeprom-xsection.clean:
	@$(call targetinfo)
	@if [ -d $(EEPROM_XSECTION_DIR) ]; then \
		$(EEPROM_XSECTION_MAKE_ENV) $(EEPROM_XSECTION_PATH) $(MAKE) $(MFLAGS) -C $(EEPROM_XSECTION_DIR) clean; \
	fi
	@$(call clean_pkg, EEPROM_XSECTION)
	@rm -rf $(EEPROM_XSECTION_BUILDROOT_DIR)

# vim: syntax=make
