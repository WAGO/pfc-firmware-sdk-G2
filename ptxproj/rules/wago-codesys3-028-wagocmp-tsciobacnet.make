## -*-makefile-*-
# $Id: wago-codesys3-024-tsciobacnet.make 7242 2014-06-24 15:44:38Z joerg.bolte@wago.com $
#
# Copyright (C) 2016 by WAGO GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CDS3_TSCIOBACNET) += cds3-tsciobacnet
#
#--- paths and names --------------------------------------------------------- 
#
CDS3_TSCIOBACNET                 := TscIoBacnet
CDS3_TSCIOBACNET_SO_VERSION      := 2.2.0
CDS3_TSCIOBACNET_FOLDER          := TscIoBacnet_git

ifdef PTXCONF_CDS3_TSCIOBACNET_SOURCE_DEV
CDS3_TSCIOBACNET_GIT_URL         := git@svgithub01001.wago.local:BU-Automation/bacnet-ptxdist-tsciobacnet.git
endif

CDS3_TSCIOBACNET_REL_PATH        := wago_intern/codesys3-Component/$(CDS3_TSCIOBACNET_FOLDER)
CDS3_TSCIOBACNET_SRC_DIR         := $(PTXDIST_WORKSPACE)/$(CDS3_TSCIOBACNET_REL_PATH)
CDS3_TSCIOBACNET_VERSION         := $(CDS3_TSCIOBACNET_SO_VERSION)
CDS3_TSCIOBACNET_ENV_VENDOR      := WAGO

ifdef PTXCONF_CDS3_TSCIOBACNET_SOURCE_RELEASED
CDS3_TSCIOBACNET_URL             := $(call jfrog_template_to_url, CDS3_TSCIOBACNET)
else
ifdef PTXCONF_CDS3_TSCIOBACNET_ARTIFACTORY_DEV
CDS3_TSCIOBACNET_URL             := $(call jfrog_template_to_url, CDS3_TSCIOBACNET_ARTIFACTORY_DEV)
else
CDS3_TSCIOBACNET_URL             := file://$(CDS3_TSCIOBACNET_REL_PATH)
endif
endif
CDS3_TSCIOBACNET_SUFFIX          := $(suffix $(CDS3_TSCIOBACNET_URL))
CDS3_TSCIOBACNET_MD5              = $(shell [ -f $(CDS3_TSCIOBACNET_MD5_FILE) ] && cat $(CDS3_TSCIOBACNET_MD5_FILE))
CDS3_TSCIOBACNET_MD5_FILE        := wago_intern/artifactory_sources/$(CDS3_TSCIOBACNET)$(CDS3_TSCIOBACNET_SUFFIX).md5
CDS3_TSCIOBACNET_ARTIFACT         = $(call jfrog_get_filename,$(CDS3_TSCIOBACNET_URL))
ifdef PTXCONF_CDS3_TSCIOBACNET_SOURCE_RELEASED
CDS3_TSCIOBACNET_ARCHIVE         := $(CDS3_TSCIOBACNET)-$(CDS3_TSCIOBACNET_VERSION)$(CDS3_TSCIOBACNET_SUFFIX)
endif
ifdef PTXCONF_CDS3_TSCIOBACNET_ARTIFACTORY_DEV
CDS3_TSCIOBACNET_ARCHIVE         := $(CDS3_TSCIOBACNET)-$(CDS3_TSCIOBACNET_VERSION)$(CDS3_TSCIOBACNET_SUFFIX)
endif

CDS3_TSCIOBACNET_BUILDROOT_DIR   := $(BUILDDIR)/$(CDS3_TSCIOBACNET)
CDS3_TSCIOBACNET_DIR             := $(CDS3_TSCIOBACNET_BUILDROOT_DIR)/$(CDS3_TSCIOBACNET)
CDS3_TSCIOBACNET_LICENSE         := unknown

CDS3_TSCIOBACNET_BUILDCONFIG     := Release
CDS3_TSCIOBACNET_BUILD_DIR       := $(CDS3_TSCIOBACNET_BUILDROOT_DIR)/bin/$(CDS3_TSCIOBACNET_BUILDCONFIG)
CDS3_TSCIOBACNET_BIN             := lib$(CDS3_TSCIOBACNET).so.$(CDS3_TSCIOBACNET_VERSION)
CDS3_TSCIOBACNET_CONF_TOOL       := NO
CDS3_TSCIOBACNET_MAKE_ENV        := $(CROSS_ENV) \
BUILDCONFIG=$(LIBBACNETSTACK_BUILDCONFIG_BUILDCONFIG) \
              BIN_DIR=$(LIBBACNETSTACK_BUILDCONFIG_BUILD_DIR) \
              TARGET_ARCH=$(PTXCONF_ARCH_STRING) \
              ARM_ARCH_VERSION=7 \
              SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build \
              CDS3_TSCIOBACNET_SO_VERSION=$(CDS3_TSCIOBACNET_SO_VERSION)

CDS3_TSCIOBACNET_PATH            := PATH=$(CROSS_PATH)
CDS3_TSCIOBACNET_PACKAGE_NAME    := $(CDS3_TSCIOBACNET)_$(CDS3_TSCIOBACNET_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCIOBACNET_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages


# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

ifdef PTXCONF_CDS3_TSCIOBACNET_SOURCE_DEV

$(CDS3_TSCIOBACNET_SRC_DIR):
	{ cd $(PTXDIST_WORKSPACE)/wago_intern && git clone $(CDS3_TSCIOBACNET_GIT_URL) $(CDS3_TSCIOBACNET_SRC_DIR); } \
    || rm -fr $(CDS3_TSCIOBACNET_SRC_DIR)
ifdef PTXCONF_CDS3_TSCIOBACNET_SOURCE_DEV_BRANCH
	{ cd $(CDS3_TSCIOBACNET_SRC_DIR) && git checkout $(PTXCONF_CDS3_TSCIOBACNET_SOURCE_DEV_BRANCH); } \
    || rm -fr $(CDS3_TSCIOBACNET_SRC_DIR)
endif

$(STATEDIR)/cds3-tsciobacnet.get: | $(CDS3_TSCIOBACNET_SRC_DIR)

endif

ifdef PTXCONF_CDS3_TSCIOBACNET_SOURCE_RELEASED
$(STATEDIR)/cds3-tsciobacnet.get:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call ptx/in-path, PTXDIST_PATH, scripts/wago/artifactory.sh) fetch \
    '$(CDS3_TSCIOBACNET_URL)' wago_intern/artifactory_sources/$(CDS3_TSCIOBACNET_ARCHIVE) '$(CDS3_TSCIOBACNET_MD5_FILE)'
endif

	@$(call touch)
endif

ifdef PTXCONF_CDS3_TSCIOBACNET_ARTIFACTORY_DEV
$(STATEDIR)/cds3-tsciobacnet.get:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call ptx/in-path, PTXDIST_PATH, scripts/wago/artifactory.sh) fetch \
    '$(CDS3_TSCIOBACNET_URL)' wago_intern/artifactory_sources/$(CDS3_TSCIOBACNET_ARCHIVE) '$(CDS3_TSCIOBACNET_MD5_FILE)'
endif

	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsciobacnet.extract:
	@$(call targetinfo)
	@mkdir -p $(CDS3_TSCIOBACNET_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@mkdir -p $(CDS3_TSCIOBACNET_DIR)
ifdef PTXCONF_CDS3_TSCIOBACNET_SOURCE_RELEASED
	@tar xvf wago_intern/artifactory_sources/$(CDS3_TSCIOBACNET_ARCHIVE) -C $(CDS3_TSCIOBACNET_DIR) --strip-components=1
	@$(call patchin, CDS3_TSCIOBACNET)
else
ifdef PTXCONF_CDS3_TSCIOBACNET_ARTIFACTORY_DEV
	@tar xvf wago_intern/artifactory_sources/$(CDS3_TSCIOBACNET_ARCHIVE) -C $(CDS3_TSCIOBACNET_DIR) --strip-components=1
	@$(call patchin, CDS3_TSCIOBACNET)
else
	@mv $(CDS3_TSCIOBACNET_SRC_DIR)/* $(CDS3_TSCIOBACNET_DIR)
endif
endif
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract.post
# ----------------------------------------------------------------------------

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/cds3-tsciobacnet.extract.post:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsciobacnet.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(MAKE) -C $(CDS3_TSCIOBACNET_DIR) SYSROOT=$(PTXCONF_SYSROOT_TARGET) itf
	$(MAKE) -C $(CDS3_TSCIOBACNET_DIR) SYSROOT=$(PTXCONF_SYSROOT_TARGET) dep
	
	@$(call world/prepare, CDS3_TSCIOBACNET)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCIOBACNET_MAKE_OPT  := CC=$(CROSS_CC)

CDS3_TSCIOBACNET_MAKE_OPT += "DBGMODE=-g3"
CDS3_TSCIOBACNET_MAKE_OPT += "OPTIMIZE=-O2"

 
$(STATEDIR)/cds3-tsciobacnet.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCIOBACNET)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsciobacnet.install:
	@$(call targetinfo)
#$(error $(CDS3_TSCIOBACNET_PKGDIR))
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
#   BSP mode: install by extracting tgz file
	@mkdir -p $(CDS3_TSCIOBACNET_PKGDIR) && \
  tar xvzf $(CDS3_TSCIOBACNET_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCIOBACNET_PACKAGE_NAME).tgz -C $(CDS3_TSCIOBACNET_PKGDIR)
else	
# 	normal mode, call "make install"	
	
	@$(call world/install, CDS3_TSCIOBACNET)
	
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
#   # save install directory to tgz for BSP mode
	@mkdir -p $(CDS3_TSCIOBACNET_PLATFORMCONFIGPACKAGEDIR)
	@cd $(CDS3_TSCIOBACNET_PKGDIR) && tar cvzf $(CDS3_TSCIOBACNET_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCIOBACNET_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsciobacnet.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init, cds3-tsciobacnet)
	@$(call install_fixup, cds3-tsciobacnet,PRIORITY,optional)
	@$(call install_fixup, cds3-tsciobacnet,SECTION,base)
	@$(call install_fixup, cds3-tsciobacnet,AUTHOR,"WAGO")
	@$(call install_fixup, cds3-tsciobacnet,DESCRIPTION,missing)

#
# TODO: Add here all files that should be copied to the target
# Note: Add everything before(!) call to macro install_finish
#
	@$(call install_lib, cds3-tsciobacnet, 0, 0, 0644, libTscIoBacnet)
	@$(call install_link, cds3-tsciobacnet, $(CDS3_TSCIOBACNET_BIN), /usr/lib/libTscIoBacnet.so);
	@$(call install_link, cds3-tsciobacnet, ../$(CDS3_TSCIOBACNET_BIN), /usr/lib/cds3-custom-components/libTscIoBacnet.so);

	@$(call install_finish, cds3-tsciobacnet)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsciobacnet.clean:
	@$(call targetinfo)

	@if [ -d $(CDS3_TSCIOBACNET_DIR) ]; then \
		$(CDS3_TSCIOBACNET_MAKE_ENV) $(CDS3_TSCIOBACNET_PATH) $(MAKE) $(MFLAGS) -C $(CDS3_TSCIOBACNET_DIR) clean; \
	fi
	@$(call clean_pkg, CDS3_TSCIOBACNET)
	
	rm -rf $(CDS3_TSCIOBACNET_SRC_DIR)
	rm -rf $(CDS3_TSCIOBACNET_BUILDROOT_DIR)
