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
PACKAGES-$(PTXCONF_CDS3_SYSUTIL) += cds3-sysutil

CDS3_SYSUTIL_VERSION   := 2.0.16
CDS3_SYSUTIL           := SysUtil
CDS3_SYSUTIL_DIR       := $(BUILDDIR)/$(CDS3_SYSUTIL)
CDS3_SYSUTIL_URL       := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_SYSUTIL)
CDS3_SYSUTIL_SRC_DIR   := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_SYSUTIL)
CDS3_SYSUTIL_SRC       := $(CDS3_SYSUTIL_SRC_DIR)/$(CDS3_SYSUTIL)
CDS3_SYSUTIL_BIN       := lib$(CDS3_SYSUTIL).so.$(CDS3_SYSUTIL_VERSION)

CDS3_SYSUTIL_PACKAGE_NAME := cds3-sysutil_$(CDS3_SYSUTIL_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_SYSUTIL_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages
CDS3_SYSUTIL_PACKAGE_DIR := $(PTXDIST_TEMPDIR)/package/$(CDS3_SYSUTIL_PACKAGE_NAME)

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-sysutil.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-sysutil.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_SYSUTIL_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_SYSUTIL_SRC_DIR)/ $(CDS3_SYSUTIL_DIR)/

endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-sysutil.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(MAKE) -C $(CDS3_SYSUTIL_DIR) SYSROOT=$(PTXCONF_SYSROOT_TARGET) itf
	$(MAKE) -C $(CDS3_SYSUTIL_DIR) SYSROOT=$(PTXCONF_SYSROOT_TARGET) dep
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_SYSUTIL_PATH      := PATH=$(CROSS_PATH)
CDS3_SYSUTIL_MAKE_ENV  := $(CROSS_ENV)
CDS3_SYSUTIL_MAKE_OPT  := CC=$(CROSS_CC)

$(STATEDIR)/cds3-sysutil.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_SYSUTIL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

SYSUTIL_SYSROOT_INCLUDES= $(PTXCONF_SYSROOT_TARGET)/usr/include/SysUtil
$(STATEDIR)/cds3-sysutil.install:
	@$(call targetinfo)

	@mkdir -p $(SYSUTIL_SYSROOT_INCLUDES)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cp $(CDS3_SYSUTIL_DIR)/*.h $(SYSUTIL_SYSROOT_INCLUDES)/
	@cp $(CDS3_SYSUTIL_DIR)/*Itf.h $(PTXCONF_SYSROOT_TARGET)/usr/include/codesys3/
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CDS3_SYSUTIL_DIR) && tar cvzf $(CDS3_SYSUTIL_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_SYSUTIL_PACKAGE_NAME).tgz *.h
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	@tar -xzvf $(CDS3_SYSUTIL_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_SYSUTIL_PACKAGE_NAME).tgz -C $(SYSUTIL_SYSROOT_INCLUDES)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-sysutil.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-sysutil)

	@$(call install_fixup,cds3-sysutil,PRIORITY,optional)
	@$(call install_fixup,cds3-sysutil,SECTION,base)
	@$(call install_fixup,cds3-sysutil,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-sysutil,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	mkdir -p $(CDS3_SYSUTIL_PACKAGE_DIR)
	cd $(CDS3_SYSUTIL_PACKAGE_DIR) && \
	ar -xov $(CDS3_SYSUTIL_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_SYSUTIL_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-sysutil, 0, 0, $(CDS3_SYSUTIL_PACKAGE_DIR)/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-sysutil, 0, 0, 0750, $(CDS3_SYSUTIL_DIR)/$(CDS3_SYSUTIL_BIN), /usr/lib/$(CDS3_SYSUTIL_BIN))
	@$(call install_link, cds3-sysutil, ./$(CDS3_SYSUTIL_BIN), /usr/lib/lib$(CDS3_SYSUTIL).so);
	@$(call install_link, cds3-sysutil, ../$(CDS3_SYSUTIL_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_SYSUTIL).so);
#	@$(call install_copy, cds3-sysutil, 0, 0, 0644, $(CDS3_SYSUTIL_DIR)/$(CDS3_SYSUTIL).cfg, $(PTXCONF_CDS3_PLCCONFIGDIR)/$(CDS3_SYSUTIL).cfg, n)

endif
	@$(call install_finish,cds3-sysutil)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_SYSUTIL_PACKAGE_NAME).ipk $(CDS3_SYSUTIL_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-sysutil.clean:
	rm -rf $(STATEDIR)/cds3-sysutil.*
	rm -rf $(PKGDIR)/cds3-sysutil_*
	rm -rf $(CDS3_SYSUTIL_DIR)
