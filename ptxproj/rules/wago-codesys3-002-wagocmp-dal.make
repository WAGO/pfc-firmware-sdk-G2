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
PACKAGES-$(PTXCONF_CDS3_IODRVDAL) += cds3-iodrvdal

#
# Paths and names
#
CDS3_IODRVDAL_VERSION       := 1.0.0
CDS3_IODRVDAL_MD5           :=
CDS3_IODRVDAL               := IoDrvDal
CDS3_IODRVDAL_BUILDCONFIG   := Release
CDS3_IODRVDAL_SRC_DIR       :=$(call ptx/in-path, PTXDIST_PATH, wago_intern/codesys3-Component/$(CDS3_IODRVDAL))
CDS3_IODRVDAL_BUILDROOT_DIR := $(BUILDDIR)/$(CDS3_IODRVDAL)
CDS3_IODRVDAL_DIR           := $(CDS3_IODRVDAL_BUILDROOT_DIR)/src
CDS3_IODRVDAL_BUILD_DIR     := $(CDS3_IODRVDAL_BUILDROOT_DIR)/bin/$(CDS3_IODRVDAL_BUILDCONFIG)
CDS3_IODRVDAL_LICENSE       := WAGO
CDS3_IODRVDAL_CONF_TOOL     := NO
CDS3_IODRVDAL_SO_NAME       := lib$(CDS3_IODRVDAL).so
CDS3_IODRVDAL_MAKE_ENV  := $(CROSS_ENV) \
BUILDCONFIG=$(CDS3_IODRVDAL_BUILDCONFIG) \
BIN_DIR=$(CDS3_IODRVDAL_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build \
PKG_CONFIG_PATH=$(PTXCONF_SYSROOT_TARGET)/usr/lib/pkgconfig

CDS3_IODRVDAL_PATH          := PATH=$(CROSS_PATH)
CDS3_IODRVDAL_MAKE_OPT      := CC=$(CROSS_CC)

CDS3_IODRVDAL_PACKAGE_NAME := cds3-iodrvdal_$(CDS3_IODRVDAL_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_IODRVDAL_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages
CDS3_IODRVDAL_PACKAGE_DIR := $(PTXDIST_TEMPDIR)/package/$(CDS3_IODRVDAL_PACKAGE_NAME)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvdal.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_IODRVDAL_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(CDS3_IODRVDAL_DIR) ]; then \
	  ln -s $(CDS3_IODRVDAL_SRC_DIR) $(CDS3_IODRVDAL_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvdal.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvdal.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvdal.install:
	@$(call targetinfo)
	
	@mkdir -p $(CDS3_IODRVDAL_PKGDIR)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/install, CDS3_IODRVDAL)
	
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CDS3_IODRVDAL_PKGDIR) && tar cvzf $(CDS3_IODRVDAL_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_IODRVDAL_PACKAGE_NAME).tgz *
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install headers and/or libraries from archive
	@tar xzvf $(CDS3_IODRVDAL_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_IODRVDAL_PACKAGE_NAME).tgz -C $(CDS3_IODRVDAL_PKGDIR)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvdal.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-iodrvdal)

	@$(call install_fixup,cds3-iodrvdal,PRIORITY,optional)
	@$(call install_fixup,cds3-iodrvdal,SECTION,base)
	@$(call install_fixup,cds3-iodrvdal,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-iodrvdal,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	mkdir -p $(CDS3_IODRVDAL_PACKAGE_DIR)
	cd $(CDS3_IODRVDAL_PACKAGE_DIR) && \
	ar -xov $(CDS3_IODRVDAL_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_IODRVDAL_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-iodrvdal, 0, 0, $(CDS3_IODRVDAL_PACKAGE_DIR)/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_lib, cds3-iodrvdal, 0, 0, 0640, libIoDrvDal)
	@$(call install_link, cds3-iodrvdal, ../$(CDS3_IODRVDAL_SO_NAME), /usr/lib/cds3-custom-components/$(CDS3_IODRVDAL_SO_NAME))
	@$(call install_copy, cds3-iodrvdal, 0, 0, 0640, $(CDS3_IODRVDAL_DIR)/$(CDS3_IODRVDAL).cfg, $(PTXCONF_CDS3_PLCCONFIGDIR)/$(CDS3_IODRVDAL).cfg, n)

endif
	@$(call install_finish,cds3-iodrvdal)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_IODRVDAL_PACKAGE_NAME).ipk $(CDS3_IODRVDAL_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------
$(STATEDIR)/cds3-iodrvdal.clean:
	@$(call targetinfo)
	@if [ -d $(CDS3_IODRVDAL_DIR) ]; then \
		$(CDS3_IODRVDAL_MAKE_ENV) $(CDS3_IODRVDAL_PATH) $(MAKE) $(MFLAGS) -C $(CDS3_IODRVDAL_DIR) clean; \
	fi
	@$(call clean_pkg, CDS3_IODRVDAL)
	@rm -rf $(CDS3_IODRVDAL_BUILDROOT_DIR)

# vim: syntax=make

