## -*-makefile-*-
# $Id$
#
# Copyright (C) 2022 by Wago GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CDS3_TSCIODRVKBUS) += cds3-tsciodrvkbus

CDS3_TSCIODRVKBUS_VERSION      := 0.0.1
CDS3_TSCIODRVKBUS              := TscIoDrvKbus
CDS3_TSCIODRVKBUS_DIR          := $(BUILDDIR)/$(CDS3_TSCIODRVKBUS)
CDS3_TSCIODRVKBUS_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCIODRVKBUS)
CDS3_TSCIODRVKBUS_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCIODRVKBUS)
CDS3_TSCIODRVKBUS_SRC          := $(CDS3_TSCIODRVKBUS_SRC_DIR)/$(CDS3_TSCIODRVKBUS)
CDS3_TSCIODRVKBUS_BIN          := lib$(CDS3_TSCIODRVKBUS).so.$(CDS3_TSCIODRVKBUS_VERSION)

CDS3_TSCIODRVKBUS_PACKAGE_NAME := cds3-tsciodrvkbus_$(CDS3_TSCIODRVKBUS_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCIODRVKBUS_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages
CDS3_TSCIODRVKBUS_PACKAGE_DIR := $(PTXDIST_TEMPDIR)/package/$(CDS3_TSCIODRVKBUS_PACKAGE_NAME)

#CDS3_IODRVDAL_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVDAL_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsciodrvkbus.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsciodrvkbus.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCIODRVKBUS_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCIODRVKBUS_SRC_DIR)/ $(CDS3_TSCIODRVKBUS_DIR)/

#	$(call patchin, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsciodrvkbus.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(MAKE) -C $(CDS3_TSCIODRVKBUS_DIR) SYSROOT=$(PTXCONF_SYSROOT_TARGET) itf
	$(MAKE) -C $(CDS3_TSCIODRVKBUS_DIR) SYSROOT=$(PTXCONF_SYSROOT_TARGET) dep
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCIODRVKBUS_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCIODRVKBUS_MAKE_ENV  := $(CROSS_ENV) ARCH=$(PTXCONF_ARCH_STRING)
CDS3_TSCIODRVKBUS_MAKE_OPT  := CC=$(CROSS_CC)


$(STATEDIR)/cds3-tsciodrvkbus.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCIODRVKBUS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

TSCIODRVKBUS_SYSROOT_INCLUDES= $(PTXCONF_SYSROOT_TARGET)/usr/include/TscIoDrvKbus
$(STATEDIR)/cds3-tsciodrvkbus.install:
	@$(call targetinfo)

	mkdir -p $(CDS3_TSCIODRVKBUS_PKGDIR)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@mkdir -p $(TSCIODRVKBUS_SYSROOT_INCLUDES)
	@cp $(CDS3_TSCIODRVKBUS_DIR)/*.h $(TSCIODRVKBUS_SYSROOT_INCLUDES)/

	@cd $(CDS3_TSCIODRVKBUS_DIR) && \
	for header in *.h; do \
		install -D $$header $(CDS3_TSCIODRVKBUS_PKGDIR)/usr/include/TscIoDrvKbus/$$(basename $$header); \
	done;

	@cp $(CDS3_TSCIODRVKBUS_DIR)/*Itf.h $(PTXCONF_SYSROOT_TARGET)/usr/include/codesys3/

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CDS3_TSCIODRVKBUS_PKGDIR) && tar -czvf $(CDS3_TSCIODRVKBUS_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCIODRVKBUS_PACKAGE_NAME).tgz *

endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	tar -xzvf $(CDS3_TSCIODRVKBUS_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCIODRVKBUS_PACKAGE_NAME).tgz -C $(PTXCONF_SYSROOT_TARGET)
	tar -xzvf $(CDS3_TSCIODRVKBUS_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCIODRVKBUS_PACKAGE_NAME).tgz -C $(PKGDIR)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsciodrvkbus.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tsciodrvkbus)

	@$(call install_fixup,cds3-tsciodrvkbus,PRIORITY,optional)
	@$(call install_fixup,cds3-tsciodrvkbus,SECTION,base)
	@$(call install_fixup,cds3-tsciodrvkbus,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tsciodrvkbus,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	mkdir -p $(CDS3_TSCIODRVKBUS_PACKAGE_DIR)
	cd $(CDS3_TSCIODRVKBUS_PACKAGE_DIR) && \
	ar -xov $(CDS3_TSCIODRVKBUS_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCIODRVKBUS_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tsciodrvkbus, 0, 0, $(CDS3_TSCIODRVKBUS_PACKAGE_DIR)/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-tsciodrvkbus, 0, 0, 0750, $(CDS3_TSCIODRVKBUS_DIR)/$(CDS3_TSCIODRVKBUS_BIN), /usr/lib/$(CDS3_TSCIODRVKBUS_BIN))
	@$(call install_link, cds3-tsciodrvkbus, ./$(CDS3_TSCIODRVKBUS_BIN), /usr/lib/lib$(CDS3_TSCIODRVKBUS).so);
	@$(call install_link, cds3-tsciodrvkbus, ../$(CDS3_TSCIODRVKBUS_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCIODRVKBUS).so);

endif
	@$(call install_finish,cds3-tsciodrvkbus)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCIODRVKBUS_PACKAGE_NAME).ipk $(CDS3_TSCIODRVKBUS_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsciodrvkbus.clean:
	rm -rf $(STATEDIR)/cds3-tsciodrvkbus.*
	rm -rf $(PKGDIR)/cds3-tsciodrvkbus_*
	rm -rf $(CDS3_TSCIODRVKBUS_DIR)
