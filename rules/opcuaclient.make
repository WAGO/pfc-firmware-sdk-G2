# -*-makefile-*-
#
# Copyright (C) 2016 by Jobst Wellensiek
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_OPCUACLIENT) += opcuaclient

#
# Paths and names
#
OPCUACLIENT_VERSION        := 1.1.0
OPCUACLIENT_MD5            :=
OPCUACLIENT                := opcuaclient
OPCUACLIENT_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/opcuaclient
OPCUACLIENT_URL            := file://$(OPCUACLIENT_SRC_DIR)
OPCUACLIENT_BUILDCONFIG    := Release
OPCUACLIENT_BUILDROOT_DIR  := $(BUILDDIR)/$(OPCUACLIENT)-$(OPCUACLIENT_VERSION)
OPCUACLIENT_DIR            := $(OPCUACLIENT_BUILDROOT_DIR)/src
OPCUACLIENT_BUILD_DIR      := $(OPCUACLIENT_BUILDROOT_DIR)/bin/$(OPCUACLIENT_BUILDCONFIG)
OPCUACLIENT_LICENSE        := unknown
OPCUACLIENT_BIN            := libOpcuaClient.so.$(OPCUACLIENT_VERSION)
OPCUACLIENT_PATH           := PATH=$(CROSS_PATH)
OPCUACLIENT_CONF_TOOL      := NO
OPCUACLIENT_MAKE_ENV       := $(CROSS_ENV) \
PTXCONF_CDS3_RTS_TESTS=$(PTXCONF_CDS3_RTS_TESTS) \
BUILDCONFIG=$(OPCUACLIENT_BUILDCONFIG) \
BIN_DIR=$(OPCUACLIENT_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

OPCUACLIENT_PACKAGE_NAME := $(OPCUACLIENT)_$(OPCUACLIENT_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
OPCUACLIENT_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/opcuaclient.extract:
	@$(call targetinfo)
	@mkdir -p $(OPCUACLIENT_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(OPCUACLIENT_DIR) ]; then \
	ln -s $(OPCUACLIENT_SRC_DIR) $(OPCUACLIENT_DIR); \
	fi
endif
	@$(call touch)
# ----------------------------------------------------------------------------
# Extract.post
# ----------------------------------------------------------------------------

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/opcuaclient.extract.post:
	@$(call targetinfo)
	@$(call touch)
endif
# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/opcuaclient.prepare:
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call targetinfo)
	@$(call world/prepare, OPCUACLIENT)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/opcuaclient.compile:
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call targetinfo)
	@$(call world/compile, OPCUACLIENT)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/opcuaclient.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
#   BSP mode: install by extracting tgz file
	@mkdir -p $(OPCUACLIENT_PKGDIR) && \
	tar xvzf $(OPCUACLIENT_PLATFORMCONFIGPACKAGEDIR)/$(OPCUACLIENT_PACKAGE_NAME).tgz -C $(OPCUACLIENT_PKGDIR)
else
	@$(call world/install, OPCUACLIENT)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(OPCUACLIENT_PLATFORMCONFIGPACKAGEDIR)
	@cd $(OPCUACLIENT_PKGDIR) && tar cvzf $(OPCUACLIENT_PLATFORMCONFIGPACKAGEDIR)/$(OPCUACLIENT_PACKAGE_NAME).tgz *
endif
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/opcuaclient.targetinstall:
	@$(call targetinfo)

	@$(call install_init, opcuaclient)
	@$(call install_fixup, opcuaclient,PRIORITY,optional)
	@$(call install_fixup, opcuaclient,SECTION,base)
	@$(call install_fixup, opcuaclient,AUTHOR,"Jobst Wellensiek")
	@$(call install_fixup, opcuaclient,DESCRIPTION,missing)

#
# TODO: Add here all files that should be copied to the target
# Note: Add everything before(!) call to macro install_finish
#
	@$(call install_lib, opcuaclient, 0, 0, 0644, libOpcuaClient)
	@$(call install_link, opcuaclient, ../$(OPCUACLIENT_BIN), /usr/lib/cds3-custom-components/libOpcuaClient.so)
	@$(call install_copy, opcuaclient, 0, 0, 0755, -, /etc/config-tools/config-opcua-client)
	@$(call install_copy, opcuaclient, 0, 0, 0750, /etc/specific/features/)
	@$(call install_copy, opcuaclient, 0, 0, 0644, -, /etc/specific/features/opcua-client)
	@$(call install_copy, opcuaclient, 0, 0, 0444, -, /etc/sudoers.d/config-opcua-client)

	@$(call install_finish, opcuaclient)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/opcuaclient.clean:
	@$(call targetinfo)
	@if [ -d $(OPCUACLIENT_DIR) ]; then \
	  $(OPCUACLIENT_MAKE_ENV) $(OPCUACLIENT_PATH) $(MAKE) $(MFLAGS) -C $(OPCUACLIENT_DIR) clean; \
	fi
	@$(call clean_pkg, OPCUACLIENT)
	@rm -rf $(OPCUACLIENT_BUILDROOT_DIR)

# vim: syntax=make
