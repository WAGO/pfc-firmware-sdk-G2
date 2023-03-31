# -*-makefile-*-
#
# Copyright (C) 2021 by WAGO GmbH \& Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_TSCPPCODESYS3APP) += tscppcodesys3app

#
# Paths and names
#
TSCPPCODESYS3APP_VERSION        := 0.1.1
TSCPPCODESYS3APP_MD5            :=
TSCPPCODESYS3APP                := TscPPCodesys3App
TSCPPCODESYS3APP_BUILDCONFIG    := Release
TSCPPCODESYS3APP_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(TSCPPCODESYS3APP)
TSCPPCODESYS3APP_BUILDROOT_DIR  := $(BUILDDIR)/$(TSCPPCODESYS3APP)
TSCPPCODESYS3APP_DIR            := $(TSCPPCODESYS3APP_BUILDROOT_DIR)/src
TSCPPCODESYS3APP_BUILD_DIR      := $(TSCPPCODESYS3APP_BUILDROOT_DIR)/bin/$(TSCPPCODESYS3APP_BUILDCONFIG)
TSCPPCODESYS3APP_LICENSE        := WAGO
TSCPPCODESYS3APP_CONF_TOOL      := NO
TSCPPCODESYS3APP_SO_NAME        := lib$(TSCPPCODESYS3APP).so
TSCPPCODESYS3APP_BIN            := $(TSCPPCODESYS3APP_SO_NAME).$(TSCPPCODESYS3APP_VERSION)
TSCPPCODESYS3APP_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(TSCPPCODESYS3APP_BUILDCONFIG) \
BIN_DIR=$(TSCPPCODESYS3APP_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

TSCPPCODESYS3APP_PACKAGE_NAME             := $(TSCPPCODESYS3APP)_$(TSCPPCODESYS3APP_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
TSCPPCODESYS3APP_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/tscppcodesys3app.extract: 
	@$(call targetinfo)
	@mkdir -p $(TSCPPCODESYS3APP_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(TSCPPCODESYS3APP_DIR) ]; then \
	  ln -s $(TSCPPCODESYS3APP_SRC_DIR) $(TSCPPCODESYS3APP_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/tscppcodesys3app.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, TSCPPCODESYS3APP)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/tscppcodesys3app.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, TSCPPCODESYS3APP)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/tscppcodesys3app.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(TSCPPCODESYS3APP_PKGDIR) && \
	  tar xvzf $(TSCPPCODESYS3APP_PLATFORMCONFIGPACKAGEDIR)/$(TSCPPCODESYS3APP_PACKAGE_NAME).tgz -C $(TSCPPCODESYS3APP_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, TSCPPCODESYS3APP)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(TSCPPCODESYS3APP_PLATFORMCONFIGPACKAGEDIR)
	@cd $(TSCPPCODESYS3APP_PKGDIR) && tar cvzf $(TSCPPCODESYS3APP_PLATFORMCONFIGPACKAGEDIR)/$(TSCPPCODESYS3APP_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/tscppcodesys3app.targetinstall:
	@$(call targetinfo)

	@$(call install_init, tscppcodesys3app)
	@$(call install_fixup, tscppcodesys3app,PRIORITY,optional)
	@$(call install_fixup, tscppcodesys3app,SECTION,base)
	@$(call install_fixup, tscppcodesys3app,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, tscppcodesys3app,DESCRIPTION,missing)

	@$(call install_lib, tscppcodesys3app, 0, 0, 0644, libTscPPCodesys3App)
	@$(call install_link, tscppcodesys3app, ../$(TSCPPCODESYS3APP_BIN), /usr/lib/cds3-custom-components/$(TSCPPCODESYS3APP_SO_NAME))

	@$(call install_finish, tscppcodesys3app)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/tscppcodesys3app.clean:
	@$(call targetinfo)
	@if [ -d $(TSCPPCODESYS3APP_DIR) ]; then \
		$(TSCPPCODESYS3APP_MAKE_ENV) $(TSCPPCODESYS3APP_PATH) $(MAKE) $(MFLAGS) -C $(TSCPPCODESYS3APP_DIR) clean; \
	fi
	@$(call clean_pkg, TSCPPCODESYS3APP)
	@rm -rf $(TSCPPCODESYS3APP_BUILDROOT_DIR)

# vim: syntax=make
