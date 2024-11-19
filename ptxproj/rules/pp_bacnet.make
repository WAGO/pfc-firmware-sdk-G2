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
PACKAGES-$(PTXCONF_PP_BACNET) += pp_bacnet

#
# Paths and names
#
PP_BACNET                := pp_bacnet
PP_BACNET_VERSION        := 1.3.0
PP_BACNET_FOLDER         := pp_bacnet_git

ifdef PTXCONF_LIBBACNETCONFIG_SOURCE_DEV
PP_BACNET_GIT_URL           := git@svgithub01001.wago.local:BU-Automation/bacnet-ptxdist-pp_bacnet.git
endif

PP_BACNET_REL_PATH          := wago_intern/$(PP_BACNET_FOLDER)
PP_BACNET_SRC_DIR           := $(PTXDIST_WORKSPACE)/$(PP_BACNET_REL_PATH)

ifdef PTXCONF_PP_BACNET_SOURCE_RELEASED
PP_BACNET_URL               := $(call jfrog_template_to_url, PP_BACNET)
PP_BACNET_ARCHIVE           := $(PP_BACNET)-$(PP_BACNET_VERSION)$(PP_BACNET_SUFFIX)
else
ifdef PTXCONF_PP_BACNET_ARTIFACTORY_DEV
PP_BACNET_URL               := $(call jfrog_template_to_url, PP_BACNET_ARTIFACTORY_DEV)
PP_BACNET_ARCHIVE           := $(PP_BACNET)-$(PP_BACNET_VERSION)$(PP_BACNET_SUFFIX)
else
PP_BACNET_URL               := file://$(PP_BACNET_SRC_DIR)
endif
endif

PP_BACNET_SUFFIX            := $(suffix $(PP_BACNET_URL))
PP_BACNET_MD5                = $(shell [ -f $(PP_BACNET_MD5_FILE) ] && cat $(PP_BACNET_MD5_FILE))
PP_BACNET_MD5_FILE          := wago_intern/artifactory_sources/$(PP_BACNET)$(PP_BACNET_SUFFIX).md5
PP_BACNET_ARTIFACT           = $(call jfrog_get_filename,$(PP_BACNET_URL))

PP_BACNET_BUILDCONFIG    := Release
PP_BACNET_BUILDROOT_DIR  := $(BUILDDIR)/$(PP_BACNET)
PP_BACNET_DIR            := $(PP_BACNET_BUILDROOT_DIR)/src
PP_BACNET_BUILD_DIR      := $(PP_BACNET_BUILDROOT_DIR)/bin/$(PP_BACNET_BUILDCONFIG)
PP_BACNET_LICENSE        := WAGO
PP_BACNET_CONF_TOOL      := NO
PP_BACNET_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(PP_BACNET_BUILDCONFIG) \
BIN_DIR=$(PP_BACNET_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

PP_BACNET_PACKAGE_NAME             := $(PP_BACNET)_$(PP_BACNET_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
PP_BACNET_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages


# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

ifdef PTXCONF_PP_BACNET_SOURCE_DEV

$(PP_BACNET_SRC_DIR):
	{ cd $(PTXDIST_WORKSPACE)/wago_intern && git clone $(PP_BACNET_GIT_URL) $(PP_BACNET_SRC_DIR); } \
    || rm -fr $(PP_BACNET_SRC_DIR)
ifdef PTXCONF_PP_BACNET_SOURCE_DEV_BRANCH
	{ cd $(PP_BACNET_SRC_DIR) && git checkout $(PTXCONF_PP_BACNET_SOURCE_DEV_BRANCH); } \
    || rm -fr $(PP_BACNET_SRC_DIR)
endif

$(STATEDIR)/pp_bacnet.get: | $(PP_BACNET_SRC_DIR)

endif

ifdef PTXCONF_PP_BACNET_SOURCE_RELEASED
$(STATEDIR)/pp_bacnet.get:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call ptx/in-path, PTXDIST_PATH, scripts/wago/artifactory.sh) fetch \
    '$(PP_BACNET_URL)' wago_intern/artifactory_sources/$(PP_BACNET_ARCHIVE) '$(PP_BACNET_MD5_FILE)'
endif

	@$(call touch)
endif

ifdef PTXCONF_PP_BACNET_ARTIFACTORY_DEV
$(STATEDIR)/pp_bacnet.get:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call ptx/in-path, PTXDIST_PATH, scripts/wago/artifactory.sh) fetch \
    '$(PP_BACNET_URL)' wago_intern/artifactory_sources/$(PP_BACNET_ARCHIVE) '$(PP_BACNET_MD5_FILE)'
endif

	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/pp_bacnet.extract:
	@$(call targetinfo)
	@mkdir -p $(PP_BACNET_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
ifdef PTXCONF_PP_BACNET_SOURCE_RELEASED
	@mkdir -p $(PP_BACNET_DIR)
	@tar xvf wago_intern/artifactory_sources/$(PP_BACNET_ARCHIVE) -C $(PP_BACNET_DIR) --strip-components=1
	@$(call patchin, PP_BACNET)
endif
ifdef PTXCONF_PP_BACNET_ARTIFACTORY_DEV
	@mkdir -p $(PP_BACNET_DIR)
	@tar xvf wago_intern/artifactory_sources/$(PP_BACNET_ARCHIVE) -C $(PP_BACNET_DIR) --strip-components=1
	@$(call patchin, PP_BACNET)
else
ifndef PTXCONF_PP_BACNET_SOURCE_RELEASED
	@if [ ! -L $(PP_BACNET_DIR) ]; then \
	  ln -s $(PP_BACNET_SRC_DIR) $(PP_BACNET_DIR); \
	fi
endif
endif
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract.post
# ----------------------------------------------------------------------------

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/pp_bacnet.extract.post:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_bacnet.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, PP_BACNET)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_bacnet.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, PP_BACNET)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_bacnet.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(PP_BACNET_PKGDIR) && \
	  tar xvzf $(PP_BACNET_PLATFORMCONFIGPACKAGEDIR)/$(PP_BACNET_PACKAGE_NAME).tgz -C $(PP_BACNET_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, PP_BACNET)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(PP_BACNET_PLATFORMCONFIGPACKAGEDIR)
	@cd $(PP_BACNET_PKGDIR) && tar cvzf $(PP_BACNET_PLATFORMCONFIGPACKAGEDIR)/$(PP_BACNET_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_bacnet.targetinstall:
	@$(call targetinfo)

	@$(call install_init, pp_bacnet)
	@$(call install_fixup, pp_bacnet,PRIORITY,optional)
	@$(call install_fixup, pp_bacnet,SECTION,base)
	@$(call install_fixup, pp_bacnet,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, pp_bacnet,DESCRIPTION, "BACnet parameter provider for WAGO Device Access: WDx)

	@$(call install_copy, pp_bacnet, 0, 0, 0750, -, /usr/sbin/pp_bacnet)
	@$(call install_copy, pp_bacnet, 0, 0, 0700, -, /etc/init.d/pp_bacnet)
	@$(call install_link, pp_bacnet, ../init.d/pp_bacnet, /etc/rc.d/S87_pp_bacnet)

	@$(call install_finish, pp_bacnet)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_bacnet.clean:
	@$(call targetinfo)
	@if [ -d $(PP_BACNET_DIR) ]; then \
		$(PP_BACNET_MAKE_ENV) $(PP_BACNET_PATH) $(MAKE) $(MFLAGS) -C $(PP_BACNET_DIR) clean; \
	fi
	@$(call clean_pkg, PP_BACNET)
	@rm -rf $(PP_BACNET_BUILDROOT_DIR)

# vim: syntax=make
