# -*-makefile-*-
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
PACKAGES-$(PTXCONF_CDS3_TSCIODRVCC100WDX) += cds3-tsciodrvcc100wdx

CDS3_TSCIODRVCC100WDX_VERSION   := 1.0.5
CDS3_TSCIODRVCC100WDX           := TscIoDrvCC100WDx
CDS3_TSCIODRVCC100WDX_DIR       := $(BUILDDIR)/$(CDS3_TSCIODRVCC100WDX)
CDS3_TSCIODRVCC100WDX_URL       := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCIODRVCC100WDX)
CDS3_TSCIODRVCC100WDX_SRC_DIR   := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCIODRVCC100WDX)
CDS3_TSCIODRVCC100WDX_SRC       := $(CDS3_TSCIODRVCC100WDX_SRC_DIR)/$(CDS3_TSCIODRVCC100WDX)
CDS3_TSCIODRVCC100WDX_BIN       := lib$(CDS3_TSCIODRVCC100WDX).so.$(CDS3_TSCIODRVCC100WDX_VERSION)

CDS3_TSCIODRVCC100WDX_PACKAGE_NAME := cds3-tsciodrvcc100wdx_$(CDS3_TSCIODRVCC100WDX_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCIODRVCC100WDX_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages
CDS3_TSCIODRVCC100WDX_PACKAGE_DIR := $(PTXDIST_TEMPDIR)/package/$(CDS3_TSCIODRVCC100WDX_PACKAGE_NAME)

#CDS3_IODRVDAL_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVDAL_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsciodrvcc100wdx.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsciodrvcc100wdx.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCIODRVCC100WDX_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCIODRVCC100WDX_SRC_DIR)/ $(CDS3_TSCIODRVCC100WDX_DIR)/

#	$(call patchin, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tsciodrvcc100wdx.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(MAKE) -C $(CDS3_TSCIODRVCC100WDX_DIR) SYSROOT=$(PTXCONF_SYSROOT_TARGET) itf
	$(MAKE) -C $(CDS3_TSCIODRVCC100WDX_DIR) SYSROOT=$(PTXCONF_SYSROOT_TARGET) dep
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCIODRVCC100WDX_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCIODRVCC100WDX_MAKE_ENV  := $(CROSS_ENV)
CDS3_TSCIODRVCC100WDX_MAKE_OPT  := CC=$(CROSS_CC)

$(STATEDIR)/cds3-tsciodrvcc100wdx.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCIODRVCC100WDX)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

TSCIODRVCC100WDX_SYSROOT_INCLUDES= $(PTXCONF_SYSROOT_TARGET)/usr/include/TscIoDrvCC100WDx
$(STATEDIR)/cds3-tsciodrvcc100wdx.install:
	@$(call targetinfo)

	@mkdir -p $(TSCIODRVCC100WDX_SYSROOT_INCLUDES)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cp $(CDS3_TSCIODRVCC100WDX_DIR)/*.h $(TSCIODRVCC100WDX_SYSROOT_INCLUDES)/
	@cp $(CDS3_TSCIODRVCC100WDX_DIR)/*Itf.h $(PTXCONF_SYSROOT_TARGET)/usr/include/codesys3/
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CDS3_TSCIODRVCC100WDX_DIR) && tar cvzf $(CDS3_TSCIODRVCC100WDX_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCIODRVCC100WDX_PACKAGE_NAME).tgz *.h
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	@tar -xzvf $(CDS3_TSCIODRVCC100WDX_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCIODRVCC100WDX_PACKAGE_NAME).tgz -C $(TSCIODRVCC100WDX_SYSROOT_INCLUDES)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsciodrvcc100wdx.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tsciodrvcc100wdx)

	@$(call install_fixup,cds3-tsciodrvcc100wdx,PRIORITY,optional)
	@$(call install_fixup,cds3-tsciodrvcc100wdx,SECTION,base)
	@$(call install_fixup,cds3-tsciodrvcc100wdx,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tsciodrvcc100wdx,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# Extract precompiled binaries from archive
	mkdir -p $(CDS3_TSCIODRVCC100WDX_PACKAGE_DIR)
	cd $(CDS3_TSCIODRVCC100WDX_PACKAGE_DIR) && \
	ar -xov $(CDS3_TSCIODRVCC100WDX_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCIODRVCC100WDX_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tsciodrvcc100wdx, 0, 0, $(CDS3_TSCIODRVCC100WDX_PACKAGE_DIR)/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call install_copy, cds3-tsciodrvcc100wdx, 0, 0, 0750, $(CDS3_TSCIODRVCC100WDX_DIR)/$(CDS3_TSCIODRVCC100WDX_BIN), /usr/lib/$(CDS3_TSCIODRVCC100WDX_BIN))
	@$(call install_link, cds3-tsciodrvcc100wdx, ./$(CDS3_TSCIODRVCC100WDX_BIN), /usr/lib/lib$(CDS3_TSCIODRVCC100WDX).so);
	@$(call install_link, cds3-tsciodrvcc100wdx, ../$(CDS3_TSCIODRVCC100WDX_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCIODRVCC100WDX).so);
#	@$(call install_copy, cds3-tsciodrvcc100wdx, 0, 0, 0644, $(CDS3_TSCIODRVCC100WDX_DIR)/$(CDS3_TSCIODRVCC100WDX).cfg, $(PTXCONF_CDS3_PLCCONFIGDIR)/$(CDS3_TSCIODRVCC100WDX).cfg, n)
endif
	@$(call install_finish,cds3-tsciodrvcc100wdx)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCIODRVCC100WDX_PACKAGE_NAME).ipk $(CDS3_TSCIODRVCC100WDX_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsciodrvcc100wdx.clean:
	rm -rf $(STATEDIR)/cds3-tsciodrvcc100wdx.*
	rm -rf $(PKGDIR)/cds3-tsciodrvcc100wdx_*
	rm -rf $(CDS3_TSCIODRVCC100WDX_DIR)	
