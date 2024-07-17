# -*-makefile-*-
# $Id$
#
# Copyright (C) 2008 by 
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_BACNETNATIVE) += bacnetnative

#--- paths and names --------------------------------------------------------- 
#
BACNETNATIVE                   := bacnetnative
BACNETNATIVE_FOLDER            := bacnetnative_git

BACNETNATIVE_VERSION           := 1.0.2
BACNETNATIVE_ELF_VERSION       := $(BACNETNATIVE_VERSION)
BACNETNATIVE_REL_PATH          := wago_intern/device/bacnet/$(BACNETNATIVE_FOLDER)
BACNETNATIVE_SRC_DIR           := $(PTXDIST_WORKSPACE)/$(BACNETNATIVE_REL_PATH)
BACNETNATIVE_URL               := file://$(BACNETNATIVE_SRC_DIR)

ifdef PTXCONF_BACNETNATIVE_SOURCE_DEV
BACNETNATIVE_GIT_URL           := git@svgithub01001.wago.local:BU-Automation/bacnet-ptxdist-bacnet_native.git
endif

ifdef PTXCONF_BACNETNATIVE_SOURCE_RELEASED
BACNETNATIVE_URL               := $(call jfrog_template_to_url, BACNETNATIVE)
endif


BACNETNATIVE_SUFFIX            := $(suffix $(BACNETNATIVE_URL))
BACNETNATIVE_MD5                = $(shell [ -f $(BACNETNATIVE_MD5_FILE) ] && cat $(BACNETNATIVE_MD5_FILE))
BACNETNATIVE_MD5_FILE          := wago_intern/artifactory_sources/$(BACNETNATIVE)$(BACNETNATIVE_SUFFIX).md5
BACNETNATIVE_ARTIFACT           = $(call jfrog_get_filename,$(BACNETNATIVE_URL))
ifdef PTXCONF_BACNETNATIVE_SOURCE_RELEASED
BACNETNATIVE_ARCHIVE           := $(BACNETNATIVE)-$(BACNETNATIVE_VERSION)$(BACNETNATIVE_SUFFIX)
endif

BACNETNATIVE_BUILDROOT_DIR     := $(BUILDDIR)/$(BACNETNATIVE)
BACNETNATIVE_DIR               := $(BACNETNATIVE_BUILDROOT_DIR)/$(BACNETNATIVE)
BACNETNATIVE_LICENSE           := unknown

BACNETNATIVE_BUILDCONFIG       := Release
BACNETNATIVE_BUILD_DIR         := $(BACNETNATIVE_BUILDROOT_DIR)/bin/$(BACNETNATIVE_BUILDCONFIG)
BACNETNATIVE_BIN               := $(BACNETNATIVE).so.$(BACNETNATIVE_VERSION)
BACNETNATIVE_MAKE_ENV          := $(CROSS_ENV) \
                                  BUILDCONFIG=$(BACNETNATIVE_BUILDCONFIG) \
                                  BIN_DIR=$(BACNETNATIVE_BUILD_DIR) \
                                  TARGET_ARCH=$(PTXCONF_ARCH_STRING) \
                                  ARM_ARCH_VERSION=7 \
                                  SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

BACNETNATIVE_PATH              := PATH=$(CROSS_PATH)
BACNETNATIVE_PACKAGE_NAME      := $(BACNETNATIVE)_$(BACNETNATIVE_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
BACNETNATIVE_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

BACNETNATIVE_PACKAGE_DIR := $(PTXDIST_TEMPDIR)/package/$(BACNETNATIVE_PACKAGE_NAME)

#$(info BACNETNATIVE_SRC_DIR="$(BACNETNATIVE_SRC_DIR)")
#$(info BACNETNATIVE_URL="$(BACNETNATIVE_URL)")

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

ifdef PTXCONF_BACNETNATIVE_SOURCE_DEV

$(BACNETNATIVE_SRC_DIR):
	{ cd $(PTXDIST_WORKSPACE)/wago_intern && git clone $(BACNETNATIVE_GIT_URL) $(BACNETNATIVE_SRC_DIR); } \
    || rm -fr $(BACNETNATIVE_SRC_DIR)
ifdef PTXCONF_BACNETNATIVE_SOURCE_DEV_BRANCH
	{ cd $(BACNETNATIVE_SRC_DIR) && git checkout $(PTXCONF_BACNETNATIVE_SOURCE_DEV_BRANCH); } \
    || rm -fr $(BACNETNATIVE_SRC_DIR)
endif

$(STATEDIR)/bacnetnative.get: | $(BACNETNATIVE_SRC_DIR)

endif

ifdef PTXCONF_BACNETNATIVE_SOURCE_RELEASED
$(STATEDIR)/bacnetnative.get:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call ptx/in-path, PTXDIST_PATH, scripts/wago/artifactory.sh) fetch \
    '$(BACNETNATIVE_URL)' wago_intern/artifactory_sources/$(BACNETNATIVE_ARCHIVE) '$(BACNETNATIVE_MD5_FILE)'
endif

	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/bacnetnative.extract:
	@$(call targetinfo)
	@mkdir -p $(BACNETNATIVE_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
ifdef PTXCONF_BACNETNATIVE_SOURCE_RELEASED
	@mkdir -p $(BACNETNATIVE_DIR)
	@tar xvf wago_intern/artifactory_sources/$(BACNETNATIVE_ARCHIVE) -C $(BACNETNATIVE_DIR) --strip-components=1
	@$(call patchin, BACNETNATIVE)
endif
ifndef PTXCONF_BACNETNATIVE_SOURCE_RELEASED
	@if [ ! -L $(BACNETNATIVE_DIR) ]; then \
	  	ln -s $(BACNETNATIVE_SRC_DIR) $(BACNETNATIVE_DIR); \
	fi
endif
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract.post
# ----------------------------------------------------------------------------

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/bacnetnative.extract.post:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/bacnetnative.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, BACNETNATIVE)
endif
	@$(call touch)
# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/bacnetnative.compile:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, BACNETNATIVE)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/bacnetnative.install:
	@$(call targetinfo)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
#   BSP mode: install by extracting tgz file
	@mkdir -p $(BACNETNATIVE_PKGDIR) && \
  	tar xvzf $(BACNETNATIVE_PLATFORMCONFIGPACKAGEDIR)/$(BACNETNATIVE_PACKAGE_NAME).tgz -C $(BACNETNATIVE_PKGDIR)
else	
# 	normal mode, call "make install"

	@$(call world/install, BACNETNATIVE)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(BACNETNATIVE_PLATFORMCONFIGPACKAGEDIR)
	@cd $(BACNETNATIVE_PKGDIR) && tar cvzf $(BACNETNATIVE_PLATFORMCONFIGPACKAGEDIR)/$(BACNETNATIVE_PACKAGE_NAME).tgz *
endif
endif

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/bacnetnative.targetinstall:
	@$(call targetinfo, $@)
	@$(call install_init, $(BACNETNATIVE))
	@$(call install_fixup, $(BACNETNATIVE),PRIORITY,optional)
	@$(call install_fixup, $(BACNETNATIVE),VERSION,$(BACNETNATIVE_VERSION))
	@$(call install_fixup, $(BACNETNATIVE),SECTION,base)
	@$(call install_fixup, $(BACNETNATIVE),AUTHOR,"Rolf Schulenburg <rolf.schulenburg@wago.com>")
	@$(call install_fixup, $(BACNETNATIVE),DESCRIPTION,"Standalone BACnet HUB")

	@$(call install_copy, $(BACNETNATIVE), 0, 0, 0755, -, /usr/bin/bacnetapp)
	@$(call install_link, $(BACNETNATIVE), ../init.d/bacnet, /etc/rc.d/S99_bacnet) # install copy by libbacnetconfig

#ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
#	# Extract precompiled binaries from archive
#   mkdir -p $(BACNETNATIVE_PACKAGE_DIR)
#	cd $(BACNETNATIVE_PACKAGE_DIR) && \
#	ar -xov $(BACNETNATIVE_PLATFORMCONFIGPACKAGEDIR)/$(BACNETNATIVE_PACKAGE_NAME).ipk  
#	@$(call install_archive, bacnetnative, -, -, $(BACNETNATIVE_PACKAGE_DIR)/data.tar.gz, )
#else
#	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
#	@$(call install_copy, bacnetnative, 0, 0, 0755, $(BACNETNATIVE_DIR)/bin/bacnetnative, /usr/bin/bacnetnative, y)
#endif

	@$(call install_finish, $(BACNETNATIVE))
#ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
#	# Backup binaries in configs/@platform@/packages/
#	cp $(PKGDIR)/$(BACNETNATIVE_PACKAGE_NAME).ipk $(BACNETNATIVE_PLATFORMCONFIGPACKAGEDIR)/
#endif
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/bacnetnative.clean:
	rm -rf $(STATEDIR)/bacnetnative.*
	rm -rf $(PKGDIR)/bacnetnative*
	rm -rf $(BACNETNATIVE_DIR)
