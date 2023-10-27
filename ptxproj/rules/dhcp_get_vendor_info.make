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
PACKAGES-$(PTXCONF_DHCP_GET_VENDOR_INFO) += dhcp_get_vendor_info

#
# Paths and names
#
DHCP_GET_VENDOR_INFO_VERSION        := 0.1.0
DHCP_GET_VENDOR_INFO_MD5            :=
DHCP_GET_VENDOR_INFO                := dhcp_get_vendor_info
DHCP_GET_VENDOR_INFO_BUILDCONFIG    := Release
DHCP_GET_VENDOR_INFO_SRC_DIR        := $(call ptx/in-path, PTXDIST_PATH, wago_intern/$(DHCP_GET_VENDOR_INFO))
DHCP_GET_VENDOR_INFO_BUILDROOT_DIR  := $(BUILDDIR)/$(DHCP_GET_VENDOR_INFO)
DHCP_GET_VENDOR_INFO_DIR            := $(DHCP_GET_VENDOR_INFO_BUILDROOT_DIR)/src
DHCP_GET_VENDOR_INFO_BUILD_DIR      := $(DHCP_GET_VENDOR_INFO_BUILDROOT_DIR)/bin/$(DHCP_GET_VENDOR_INFO_BUILDCONFIG)
DHCP_GET_VENDOR_INFO_LICENSE        := WAGO
DHCP_GET_VENDOR_INFO_CONF_TOOL      := NO
DHCP_GET_VENDOR_INFO_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(DHCP_GET_VENDOR_INFO_BUILDCONFIG) \
BIN_DIR=$(DHCP_GET_VENDOR_INFO_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build \
PTXDIST_PACKAGE_MK_FILE=$(call ptx/in-path, PTXDIST_PATH, rules/dhcp_get_vendor_info.make)

DHCP_GET_VENDOR_INFO_PACKAGE_NAME             := $(DHCP_GET_VENDOR_INFO)_$(DHCP_GET_VENDOR_INFO_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
DHCP_GET_VENDOR_INFO_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/dhcp_get_vendor_info.extract: 
	@$(call targetinfo)
	@mkdir -p $(DHCP_GET_VENDOR_INFO_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(DHCP_GET_VENDOR_INFO_DIR) ]; then \
	  ln -s $(DHCP_GET_VENDOR_INFO_SRC_DIR) $(DHCP_GET_VENDOR_INFO_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/dhcp_get_vendor_info.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, DHCP_GET_VENDOR_INFO)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/dhcp_get_vendor_info.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, DHCP_GET_VENDOR_INFO)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/dhcp_get_vendor_info.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(DHCP_GET_VENDOR_INFO_PKGDIR) && \
	  tar xvzf $(DHCP_GET_VENDOR_INFO_PLATFORMCONFIGPACKAGEDIR)/$(DHCP_GET_VENDOR_INFO_PACKAGE_NAME).tgz -C $(DHCP_GET_VENDOR_INFO_PKGDIR)
else
# normal mode, call "make install"

	@mkdir -p $(DHCP_GET_VENDOR_INFO_PKGDIR)/etc/config-tools/
		@cp $(DHCP_GET_VENDOR_INFO_BUILD_DIR)/$(DHCP_GET_VENDOR_INFO).elf $(DHCP_GET_VENDOR_INFO_PKGDIR)/etc/config-tools/$(DHCP_GET_VENDOR_INFO)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(DHCP_GET_VENDOR_INFO_PLATFORMCONFIGPACKAGEDIR)
	@cd $(DHCP_GET_VENDOR_INFO_PKGDIR) && tar cvzf $(DHCP_GET_VENDOR_INFO_PLATFORMCONFIGPACKAGEDIR)/$(DHCP_GET_VENDOR_INFO_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/dhcp_get_vendor_info.targetinstall:
	@$(call targetinfo)

	@$(call install_init, dhcp_get_vendor_info)
	@$(call install_fixup, dhcp_get_vendor_info,PRIORITY,optional)
	@$(call install_fixup, dhcp_get_vendor_info,SECTION,base)
	@$(call install_fixup, dhcp_get_vendor_info,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, dhcp_get_vendor_info,DESCRIPTION,missing)

	@$(call install_copy, dhcp_get_vendor_info, 0, 0, 0700, -, /etc/config-tools/dhcp_get_vendor_info)

	@$(call install_finish, dhcp_get_vendor_info)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/dhcp_get_vendor_info.clean:
	@$(call targetinfo)
	@if [ -d $(DHCP_GET_VENDOR_INFO_DIR) ]; then \
		$(DHCP_GET_VENDOR_INFO_MAKE_ENV) $(DHCP_GET_VENDOR_INFO_PATH) $(MAKE) $(MFLAGS) -C $(DHCP_GET_VENDOR_INFO_DIR) clean; \
	fi
	@$(call clean_pkg, DHCP_GET_VENDOR_INFO)
	@rm -rf $(DHCP_GET_VENDOR_INFO_BUILDROOT_DIR)

# vim: syntax=make
