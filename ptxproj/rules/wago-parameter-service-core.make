#######################################################################################################################
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of project parameter-service (PTXdist package wago-parameter-service-core).
#
# Copyright (c) 2019-2022 WAGO GmbH & Co. KG
#
# Contributors:
#   MP:  WAGO GmbH & Co. KG
#   PEn: WAGO GmbH & Co. KG
#######################################################################################################################
# Makefile for PTXdist package wago-parameter-service-core.
#
# For further information about the PTXdist project and license conditions
# see the PTXdist README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_WAGO_PARAMETER_SERVICE_CORE) += wago-parameter-service-core

#
# Paths and names
#

ifndef PTXCONF_WAGO_PARAMETER_SERVICE_CORE_DEV

WAGO_PARAMETER_SERVICE_CORE_VERSION        := 1.5.3
WAGO_PARAMETER_SERVICE_CORE                := wago-parameter-service-core-$(WAGO_PARAMETER_SERVICE_CORE_VERSION)
WAGO_PARAMETER_SERVICE_CORE_DIR            := $(BUILDDIR)/$(WAGO_PARAMETER_SERVICE_CORE)
WAGO_PARAMETER_SERVICE_CORE_URL            := $(call jfrog_template_to_url, WAGO_PARAMETER_SERVICE_CORE)
WAGO_PARAMETER_SERVICE_CORE_SUFFIX         := $(suffix $(WAGO_PARAMETER_SERVICE_CORE_URL))
WAGO_PARAMETER_SERVICE_CORE_MD5             = $(shell [ -f $(WAGO_PARAMETER_SERVICE_CORE_MD5_FILE) ] && cat $(WAGO_PARAMETER_SERVICE_CORE_MD5_FILE))
WAGO_PARAMETER_SERVICE_CORE_SOURCE_BASEDIR := wago_intern/artifactory_sources
WAGO_PARAMETER_SERVICE_CORE_MD5_FILE       := $(WAGO_PARAMETER_SERVICE_CORE_SOURCE_BASEDIR)/$(WAGO_PARAMETER_SERVICE_CORE)$(WAGO_PARAMETER_SERVICE_CORE_SUFFIX).md5
WAGO_PARAMETER_SERVICE_CORE_ARTIFACT        = $(call jfrog_get_filename,$(WAGO_PARAMETER_SERVICE_CORE_URL))

else

WAGO_PARAMETER_SERVICE_CORE_VERSION        := git
WAGO_PARAMETER_SERVICE_CORE                := wago-parameter-service-core-$(WAGO_PARAMETER_SERVICE_CORE_VERSION)
WAGO_PARAMETER_SERVICE_CORE_DIR            := $(BUILDDIR)/$(WAGO_PARAMETER_SERVICE_CORE)
WAGO_PARAMETER_SERVICE_CORE_GIT_URL        := https://svgithub01001.wago.local/BU-Automation/wdx-core
WAGO_PARAMETER_SERVICE_CORE_URL            := file://repo_src/$(WAGO_PARAMETER_SERVICE_CORE)
WAGO_PARAMETER_SERVICE_CORE_SUFFIX         := git
WAGO_PARAMETER_SERVICE_CORE_MD5            := undefined

endif

WAGO_PARAMETER_SERVICE_CORE_BUILDCONFIG    := Release
WAGO_PARAMETER_SERVICE_CORE_LICENSE        := WAGO

WAGO_PARAMETER_SERVICE_CORE_PACKAGE_NAME   := $(WAGO_PARAMETER_SERVICE_CORE)_$(WAGO_PARAMETER_SERVICE_CORE_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
WAGO_PARAMETER_SERVICE_CORE_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
ifndef PTXCONF_WAGO_PARAMETER_SERVICE_CORE_DEV

$(STATEDIR)/wago-parameter-service-core.get:
	$(call ptx/in-path, PTXDIST_PATH, scripts/wago/artifactory.sh) fetch \
	  '$(WAGO_PARAMETER_SERVICE_CORE_URL)' $(WAGO_PARAMETER_SERVICE_CORE_SOURCE_BASEDIR)/$(WAGO_PARAMETER_SERVICE_CORE)$(WAGO_PARAMETER_SERVICE_CORE_SUFFIX) '$(WAGO_PARAMETER_SERVICE_CORE_MD5_FILE)'

else

$(STATEDIR)/wago-parameter-service-core.get: | $(PTXDIST_WORKSPACE)/repo_src/$(WAGO_PARAMETER_SERVICE_CORE)

$(PTXDIST_WORKSPACE)/repo_src/$(WAGO_PARAMETER_SERVICE_CORE):
	@mkdir -p $(PTXDIST_WORKSPACE)/repo_src
	{ cd $(PTXDIST_WORKSPACE)/repo_src && git clone $(WAGO_PARAMETER_SERVICE_CORE_GIT_URL) $(WAGO_PARAMETER_SERVICE_CORE); } \
	  || rm -rf $(PTXDIST_WORKSPACE)/repo_src/$(WAGO_PARAMETER_SERVICE_CORE)
ifdef PTXCONF_WAGO_PARAMETER_SERVICE_CORE_DEV_BRANCH
	{ cd $(PTXDIST_WORKSPACE)/repo_src/$(WAGO_PARAMETER_SERVICE_CORE) && git checkout $(PTXCONF_WAGO_PARAMETER_SERVICE_CORE_DEV_BRANCH); } \
	  || rm -rf $(PTXDIST_WORKSPACE)/repo_src/$(WAGO_PARAMETER_SERVICE_CORE)
endif

endif

else

$(STATEDIR)/wago-parameter-service-core.get:

endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-parameter-service-core.extract:
	@$(call targetinfo)
	@mkdir -p $(WAGO_PARAMETER_SERVICE_CORE_DIR)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
ifndef PTXCONF_WAGO_PARAMETER_SERVICE_CORE_DEV

	@tar xvzf $(WAGO_PARAMETER_SERVICE_CORE_SOURCE_BASEDIR)/$(WAGO_PARAMETER_SERVICE_CORE)$(WAGO_PARAMETER_SERVICE_CORE_SUFFIX) -C $(WAGO_PARAMETER_SERVICE_CORE_DIR) --strip-components=1
	@$(call patchin, WAGO_PARAMETER_SERVICE_CORE)

else

	@$(call clean, $(WAGO_PARAMETER_SERVICE_CORE_DIR))

	@if [ ! -L $(WAGO_PARAMETER_SERVICE_CORE_DIR) ]; then \
	  ln -s $(PTXDIST_WORKSPACE)/repo_src/$(WAGO_PARAMETER_SERVICE_CORE) $(WAGO_PARAMETER_SERVICE_CORE_DIR); \
	fi

endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

#WAGO_PARAMETER_SERVICE_CORE_PATH      := PATH=$(CROSS_PATH)
WAGO_PARAMETER_SERVICE_CORE_CONF_TOOL  := cmake
WAGO_PARAMETER_SERVICE_CORE_CONF_OPT   := $(CROSS_CMAKE_USR) -D CMAKE_BUILD_TYPE=$(WAGO_PARAMETER_SERVICE_CORE_BUILDCONFIG) -D WITHOUT_TEST:BOOL=ON
WAGO_PARAMETER_SERVICE_CORE_MAKE_ENV   := $(CROSS_ENV)

#$(STATEDIR)/wago-parameter-service-core.prepare:
#	@$(call targetinfo)
#	@$(call world/prepare, WAGO_PARAMETER_SERVICE_CORE)
#	@$(call touch)

else

$(STATEDIR)/wago-parameter-service-core.prepare:
	@$(call targetinfo)
	@$(call touch)

endif

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

#$(STATEDIR)/wago-parameter-service-core.compile:
#	@$(call targetinfo)
#	@$(call world/compile, WAGO_PARAMETER_SERVICE_CORE)
#	@$(call touch)

else

$(STATEDIR)/wago-parameter-service-core.compile:
	@$(call targetinfo)
	@$(call touch)

endif

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-parameter-service-core.install:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# "normal" mode: call "install"
	@$(call world/install, WAGO_PARAMETER_SERVICE_CORE)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory contents for BSP
	@mkdir -p $(WAGO_PARAMETER_SERVICE_CORE_PLATFORMCONFIGPACKAGEDIR) && \
	  cd $(WAGO_PARAMETER_SERVICE_CORE_PKGDIR) && tar cvzf $(WAGO_PARAMETER_SERVICE_CORE_PLATFORMCONFIGPACKAGEDIR)/$(WAGO_PARAMETER_SERVICE_CORE_PACKAGE_NAME).tgz *
endif

else
# BSP mode: install ipk contents to packages/<project name>
	@mkdir -p $(WAGO_PARAMETER_SERVICE_CORE_PKGDIR) && \
	  tar xvzf $(WAGO_PARAMETER_SERVICE_CORE_PLATFORMCONFIGPACKAGEDIR)/$(WAGO_PARAMETER_SERVICE_CORE_PACKAGE_NAME).tgz -C $(WAGO_PARAMETER_SERVICE_CORE_PKGDIR)

endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-parameter-service-core.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wago-parameter-service-core)
	@$(call install_fixup, wago-parameter-service-core,PRIORITY,optional)
	@$(call install_fixup, wago-parameter-service-core,SECTION,base)
	@$(call install_fixup, wago-parameter-service-core,AUTHOR,"PEn - WAGO GmbH \& Co. KG")
	@$(call install_fixup, wago-parameter-service-core,DESCRIPTION,missing)

#	@$(call install_lib, wago-parameter-service-core, 0, 0, 0755, libparaservcore)

	@$(call install_finish, wago-parameter-service-core)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/wago-parameter-service-core.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, WAGO_PARAMETER_SERVICE_CORE)

# vim: syntax=make
