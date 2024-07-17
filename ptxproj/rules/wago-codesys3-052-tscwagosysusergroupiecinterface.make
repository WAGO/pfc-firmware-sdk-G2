## -*-makefile-*-
# $Id$
#
# Copyright (C) 2013 by WAGO GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE) += cds3-tscwagosysusergroupiecinterface

CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_VERSION	     := 1.0.0
CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_MD5          :=
CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE              := TscWagoSysUserGroupIecInterface
CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_BUILDCONFIG := Release
CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_DIR          := $(BUILDDIR)/$(CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE)/src
CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_BUILDROOT_DIR  := $(BUILDDIR)/$(CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE)
CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE)
CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE)
CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_SRC          := $(CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_SRC_DIR)/$(CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE)
CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_BIN          := lib$(CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE).so.$(CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_VERSION)

CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_BUILD_DIR      := $(CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_BUILDROOT_DIR)/bin/$(CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_BUILDCONFIG)
CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_LICENSE        := WAGO
CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_CONF_TOOL      := NO
CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_BUILDCONFIG) \
BIN_DIR=$(CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_PACKAGE_NAME := cds3-tscwagosysusergroupiecinterface_$(CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

# During BSP creation local_src is deleted and the source code directories are
# copied on demand. To handle this condition an order-only dependency on
# the source code directory is created. When it is missing, the target below
# is executed and an error message is generated.
$(CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_SRC_DIR):
	@echo "Error: $@: directory not found!" >&2; \
	exit 2

$(STATEDIR)/cds3-tscwagosysusergroupiecinterface.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwagosysusergroupiecinterface.extract:
	@$(call targetinfo)
	@mkdir -p $(CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_DIR) ]; then \
	  ln -s $(CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_SRC_DIR) $(CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_DIR); \
	fi
endif
	@$(call touch)
	
$(STATEDIR)/cds3-tscwagosysusergroupiecinterface.extract.post:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tscwagosysusergroupiecinterface.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwagosysusergroupiecinterface.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwagosysusergroupiecinterface.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_PKGDIR) && \
	  tar xvzf $(CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_PACKAGE_NAME).tgz -C $(CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_PLATFORMCONFIGPACKAGEDIR)
	@cd $(CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_PKGDIR) && tar cvzf $(CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_PACKAGE_NAME).tgz *
endif
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwagosysusergroupiecinterface.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tscwagosysusergroupiecinterface)

	@$(call install_fixup,cds3-tscwagosysusergroupiecinterface,PRIORITY,optional)
	@$(call install_fixup,cds3-tscwagosysusergroupiecinterface,SECTION,base)
	@$(call install_fixup,cds3-tscwagosysusergroupiecinterface,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tscwagosysusergroupiecinterface,DESCRIPTION,missing)

	@$(call install_lib, cds3-tscwagosysusergroupiecinterface, 0, 0, 0644, libTscWagoSysUserGroupIecInterface)
	@$(call install_link, cds3-tscwagosysusergroupiecinterface, ../$(CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE).so);


	@$(call install_finish,cds3-tscwagosysusergroupiecinterface)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwagosysusergroupiecinterface.clean:
	@$(call targetinfo)
	@if [ -d $(CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_DIR) ]; then \
		$(CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_MAKE_ENV) $(CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_PATH) $(MAKE) $(MFLAGS) -C $(CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_DIR) clean; \
	fi
	@$(call clean_pkg, CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE)
	@rm -rf $(CDS3_TSCWAGOSYSUSERGROUPIECINTERFACE_BUILDROOT_DIR)
	
