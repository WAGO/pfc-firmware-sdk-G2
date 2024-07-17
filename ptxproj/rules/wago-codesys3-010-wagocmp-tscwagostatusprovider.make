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
PACKAGES-$(PTXCONF_CDS3_TSCWAGOSTATUSPROVIDER) += cds3-tscwagostatusprovider

CDS3_TSCWAGOSTATUSPROVIDER_VERSION	 := 0.0.2
CDS3_TSCWAGOSTATUSPROVIDER              := TscWagoStatusProvider
CDS3_TSCWAGOSTATUSPROVIDER_DIR          := $(BUILDDIR)/$(CDS3_TSCWAGOSTATUSPROVIDER)
CDS3_TSCWAGOSTATUSPROVIDER_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCWAGOSTATUSPROVIDER)
CDS3_TSCWAGOSTATUSPROVIDER_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCWAGOSTATUSPROVIDER)
CDS3_TSCWAGOSTATUSPROVIDER_SRC          := $(CDS3_TSCWAGOSTATUSPROVIDER_SRC_DIR)/$(CDS3_TSCWAGOSTATUSPROVIDER)
CDS3_TSCWAGOSTATUSPROVIDER_BIN          := lib$(CDS3_TSCWAGOSTATUSPROVIDER).so.$(CDS3_TSCWAGOSTATUSPROVIDER_VERSION)

CDS3_TSCWAGOSTATUSPROVIDER_PACKAGE_NAME := cds3-tscwagostatusprovider_$(CDS3_TSCWAGOSTATUSPROVIDER_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCWAGOSTATUSPROVIDER_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages
CDS3_TSCWAGOSTATUSPROVIDER_PACKAGE_DIR := $(PTXDIST_TEMPDIR)/package/$(CDS3_TSCWAGOSTATUSPROVIDER_PACKAGE_NAME)

#CDS3_IODRVDAL_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVDAL_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwagostatusprovider.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwagostatusprovider.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCWAGOSTATUSPROVIDER_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCWAGOSTATUSPROVIDER_SRC_DIR)/ $(CDS3_TSCWAGOSTATUSPROVIDER_DIR)/

#	$(call patchin, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tscwagostatusprovider.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(MAKE) -C $(CDS3_TSCWAGOSTATUSPROVIDER_DIR) SYSROOT=$(PTXCONF_SYSROOT_TARGET) itf
	$(MAKE) -C $(CDS3_TSCWAGOSTATUSPROVIDER_DIR) SYSROOT=$(PTXCONF_SYSROOT_TARGET) dep
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCWAGOSTATUSPROVIDER_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCWAGOSTATUSPROVIDER_MAKE_ENV  := $(CROSS_ENV)
CDS3_TSCWAGOSTATUSPROVIDER_MAKE_OPT  := CC=$(CROSS_CC)

ifdef PTXCONF_CDS3_TSCWAGOSTATUSPROVIDER_TARGET_PFCXXX
	CDS3_TSCWAGOSTATUSPROVIDER_MAKE_ENV += TRG_PLATFORM=pfc
else
ifdef PTXCONF_CDS3_TSCWAGOSTATUSPROVIDER_TARGET_PAC
	CDS3_TSCWAGOSTATUSPROVIDER_MAKE_ENV += TRG_PLATFORM=pac
endif
endif

$(STATEDIR)/cds3-tscwagostatusprovider.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
#	@$(call xslt_compile, $(CDS3_TSCWAGOSTATUSPROVIDER_DIR)/xml/codesys3.xml)
	$(call world/compile, CDS3_TSCWAGOSTATUSPROVIDER)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwagostatusprovider.install:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@cp $(CDS3_TSCWAGOSTATUSPROVIDER_DIR)/*Itf.h $(PTXCONF_SYSROOT_TARGET)/usr/include/codesys3/
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwagostatusprovider.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tscwagostatusprovider)

	@$(call install_fixup,cds3-tscwagostatusprovider,PRIORITY,optional)
	@$(call install_fixup,cds3-tscwagostatusprovider,SECTION,base)
	@$(call install_fixup,cds3-tscwagostatusprovider,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tscwagostatusprovider,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	mkdir -p $(CDS3_TSCWAGOSTATUSPROVIDER_PACKAGE_DIR)
	cd $(CDS3_TSCWAGOSTATUSPROVIDER_PACKAGE_DIR) && \
	ar -xov $(CDS3_TSCWAGOSTATUSPROVIDER_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCWAGOSTATUSPROVIDER_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tscwagostatusprovider, 0, 0, $(CDS3_TSCWAGOSTATUSPROVIDER_PACKAGE_DIR)/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-tscwagostatusprovider, 0, 0, 0750, $(CDS3_TSCWAGOSTATUSPROVIDER_DIR)/$(CDS3_TSCWAGOSTATUSPROVIDER_BIN), /usr/lib/$(CDS3_TSCWAGOSTATUSPROVIDER_BIN))
	@$(call install_link, cds3-tscwagostatusprovider, ./$(CDS3_TSCWAGOSTATUSPROVIDER_BIN), /usr/lib/lib$(CDS3_TSCWAGOSTATUSPROVIDER).so);
	@$(call install_link, cds3-tscwagostatusprovider, ../$(CDS3_TSCWAGOSTATUSPROVIDER_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCWAGOSTATUSPROVIDER).so);
	

endif
	@$(call install_finish,cds3-tscwagostatusprovider)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCWAGOSTATUSPROVIDER_PACKAGE_NAME).ipk $(CDS3_TSCWAGOSTATUSPROVIDER_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwagostatusprovider.clean:
#	@$(call xslt_clean, $(CDS3_TSCWAGOSTATUSPROVIDER_DIR)/xml/codesys3.xml)
	rm -rf $(STATEDIR)/cds3-tscwagostatusprovider.*
	rm -rf $(PKGDIR)/cds3-tscwagostatusprovider_*
	rm -rf $(CDS3_TSCWAGOSTATUSPROVIDER_DIR)
