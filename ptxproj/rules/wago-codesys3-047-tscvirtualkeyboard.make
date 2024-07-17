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
PACKAGES-$(PTXCONF_CDS3_TSCVIRTUALKEYBOARD) += cds3-tscvirtualkeyboard

CDS3_TSCVIRTUALKEYBOARD_VERSION	 := 1.0.0
CDS3_TSCVIRTUALKEYBOARD              := TscVirtualKeyboard
CDS3_TSCVIRTUALKEYBOARD_DIR          := $(BUILDDIR)/$(CDS3_TSCVIRTUALKEYBOARD)
CDS3_TSCVIRTUALKEYBOARD_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCVIRTUALKEYBOARD)
CDS3_TSCVIRTUALKEYBOARD_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCVIRTUALKEYBOARD)
CDS3_TSCVIRTUALKEYBOARD_SRC          := $(CDS3_TSCVIRTUALKEYBOARD_SRC_DIR)/$(CDS3_TSCVIRTUALKEYBOARD)
CDS3_TSCVIRTUALKEYBOARD_BIN          := lib$(CDS3_TSCVIRTUALKEYBOARD).so.$(CDS3_TSCVIRTUALKEYBOARD_VERSION)

CDS3_TSCVIRTUALKEYBOARD_PACKAGE_NAME := cds3-tscvirtualkeyboard_$(CDS3_TSCVIRTUALKEYBOARD_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCVIRTUALKEYBOARD_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages
CDS3_TSCVIRTUALKEYBOARD_PACKAGE_DIR := $(PTXDIST_TEMPDIR)/package/$(CDS3_TSCVIRTUALKEYBOARD_PACKAGE_NAME)

#CDS3_IODRVDAL_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVDAL_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscvirtualkeyboard.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscvirtualkeyboard.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCVIRTUALKEYBOARD_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCVIRTUALKEYBOARD_SRC_DIR)/ $(CDS3_TSCVIRTUALKEYBOARD_DIR)/

#	$(call patchin, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tscvirtualkeyboard.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(MAKE) -C $(CDS3_TSCVIRTUALKEYBOARD_DIR) SYSROOT=$(PTXCONF_SYSROOT_TARGET) dep
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCVIRTUALKEYBOARD_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCVIRTUALKEYBOARD_MAKE_ENV  := $(CROSS_ENV)
CDS3_TSCVIRTUALKEYBOARD_MAKE_OPT  := CC=$(CROSS_CC)

$(STATEDIR)/cds3-tscvirtualkeyboard.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCVIRTUALKEYBOARD)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

TSCIODRVCTP_SYSROOT_INCLUDES= $(PTXCONF_SYSROOT_TARGET)/usr/include/TscVirtualKeyboard
$(STATEDIR)/cds3-tscvirtualkeyboard.install:
	@$(call targetinfo)

	@mkdir -p $(TSCIODRVCTP_SYSROOT_INCLUDES)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cp $(CDS3_TSCVIRTUALKEYBOARD_DIR)/*.h $(TSCIODRVCTP_SYSROOT_INCLUDES)/
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CDS3_TSCVIRTUALKEYBOARD_DIR) && tar cvzf $(CDS3_TSCVIRTUALKEYBOARD_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCVIRTUALKEYBOARD_PACKAGE_NAME).tgz *.h
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	@tar -xzvf $(CDS3_TSCVIRTUALKEYBOARD_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCVIRTUALKEYBOARD_PACKAGE_NAME).tgz -C $(TSCIODRVCTP_SYSROOT_INCLUDES)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscvirtualkeyboard.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tscvirtualkeyboard)

	@$(call install_fixup,cds3-tscvirtualkeyboard,PRIORITY,optional)
	@$(call install_fixup,cds3-tscvirtualkeyboard,SECTION,base)
	@$(call install_fixup,cds3-tscvirtualkeyboard,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tscvirtualkeyboard,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	mkdir -p $(CDS3_TSCVIRTUALKEYBOARD_PACKAGE_DIR)
	cd $(CDS3_TSCVIRTUALKEYBOARD_PACKAGE_DIR) && \
	ar -xov $(CDS3_TSCVIRTUALKEYBOARD_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCVIRTUALKEYBOARD_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tscvirtualkeyboard, 0, 0, $(CDS3_TSCVIRTUALKEYBOARD_PACKAGE_DIR)/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-tscvirtualkeyboard, 0, 0, 0750, $(CDS3_TSCVIRTUALKEYBOARD_DIR)/$(CDS3_TSCVIRTUALKEYBOARD_BIN), /usr/lib/$(CDS3_TSCVIRTUALKEYBOARD_BIN))
	@$(call install_link, cds3-tscvirtualkeyboard, ./$(CDS3_TSCVIRTUALKEYBOARD_BIN), /usr/lib/lib$(CDS3_TSCVIRTUALKEYBOARD).so);
	@$(call install_link, cds3-tscvirtualkeyboard, ../$(CDS3_TSCVIRTUALKEYBOARD_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCVIRTUALKEYBOARD).so);
#	@$(call install_copy, cds3-tscvirtualkeyboard, 0, 0, 0644, $(CDS3_TSCVIRTUALKEYBOARD_DIR)/$(CDS3_TSCVIRTUALKEYBOARD).cfg, $(PTXCONF_CDS3_PLCCONFIGDIR)/$(CDS3_TSCVIRTUALKEYBOARD).cfg, n)

endif
	@$(call install_finish,cds3-tscvirtualkeyboard)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCVIRTUALKEYBOARD_PACKAGE_NAME).ipk $(CDS3_TSCVIRTUALKEYBOARD_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscvirtualkeyboard.clean:
	rm -rf $(STATEDIR)/cds3-tscvirtualkeyboard.*
	rm -rf $(PKGDIR)/cds3-tscvirtualkeyboard_*
	rm -rf $(CDS3_TSCVIRTUALKEYBOARD_DIR)
