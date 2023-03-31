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
PACKAGES-$(PTXCONF_PP_DOCKER) += pp_docker

#
# Paths and names
#
PP_DOCKER_VERSION        := 1.0.0
PP_DOCKER_MD5            :=
PP_DOCKER                := pp_docker
PP_DOCKER_BUILDCONFIG    := Release
PP_DOCKER_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(PP_DOCKER)
PP_DOCKER_BUILDROOT_DIR  := $(BUILDDIR)/$(PP_DOCKER)
PP_DOCKER_DIR            := $(PP_DOCKER_BUILDROOT_DIR)/src
PP_DOCKER_BUILD_DIR      := $(PP_DOCKER_BUILDROOT_DIR)/bin/$(PP_DOCKER_BUILDCONFIG)
PP_DOCKER_LICENSE        := WAGO
PP_DOCKER_CONF_TOOL      := NO
PP_DOCKER_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(PP_DOCKER_BUILDCONFIG) \
BIN_DIR=$(PP_DOCKER_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

PP_DOCKER_PACKAGE_NAME             := $(PP_DOCKER)_$(PP_DOCKER_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
PP_DOCKER_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/pp_docker.extract: 
	@$(call targetinfo)
	@mkdir -p $(PP_DOCKER_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(PP_DOCKER_DIR) ]; then \
	  ln -s $(PP_DOCKER_SRC_DIR) $(PP_DOCKER_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_docker.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, PP_DOCKER)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_docker.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, PP_DOCKER)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_docker.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(PP_DOCKER_PKGDIR) && \
	  tar xvzf $(PP_DOCKER_PLATFORMCONFIGPACKAGEDIR)/$(PP_DOCKER_PACKAGE_NAME).tgz -C $(PP_DOCKER_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, PP_DOCKER)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(PP_DOCKER_PLATFORMCONFIGPACKAGEDIR)
	@cd $(PP_DOCKER_PKGDIR) && tar cvzf $(PP_DOCKER_PLATFORMCONFIGPACKAGEDIR)/$(PP_DOCKER_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_docker.targetinstall:
	@$(call targetinfo)

	@$(call install_init, pp_docker)
	@$(call install_fixup, pp_docker,PRIORITY,optional)
	@$(call install_fixup, pp_docker,SECTION,base)
	@$(call install_fixup, pp_docker,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, pp_docker,DESCRIPTION,missing)

	@$(call install_finish, pp_docker)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_docker.clean:
	@$(call targetinfo)
	@if [ -d $(PP_DOCKER_DIR) ]; then \
		$(PP_DOCKER_MAKE_ENV) $(PP_DOCKER_PATH) $(MAKE) $(MFLAGS) -C $(PP_DOCKER_DIR) clean; \
	fi
	@$(call clean_pkg, PP_DOCKER)
	@rm -rf $(PP_DOCKER_BUILDROOT_DIR)

# vim: syntax=make
