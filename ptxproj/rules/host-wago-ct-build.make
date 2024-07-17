# -*-makefile-*-
#
# Copyright (C) 2014 by <AGa>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_CT_BUILD) += host-ct-build

#
# Paths and names
#
HOST_CT_BUILD_LICENSE := MPL-2.0
HOST_CT_BUILD         := ct-build
HOST_CT_BUILD_DIR     := $(HOST_BUILDDIR)/$(HOST_CT_BUILD)

ifdef PTXCONF_HOST_CT_BUILD_DEV
HOST_CT_BUILD_URL     := file://local_src/ct-build
HOST_CT_BUILD_GIT_URL := git@svgithub01001.wago.local:BU-Automation/buildsystems-ct-build.git
else
HOST_CT_BUILD_VERSION := 1.18.2
HOST_CT_BUILD_SUFFIX  := tar.xz
HOST_CT_BUILD_MD5     := bac5f03870c4d8f4e9f3dda1696328e7
HOST_CT_BUILD_URL     := https://svsv01003.wago.local/wago-ptxdist-src/$(HOST_CT_BUILD)-$(HOST_CT_BUILD_VERSION).$(HOST_CT_BUILD_SUFFIX)
HOST_CT_BUILD_SOURCE  := $(SRCDIR)/$(HOST_CT_BUILD)-$(HOST_CT_BUILD_VERSION).$(HOST_CT_BUILD_SUFFIX)
endif


# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

ifdef PTXCONF_HOST_CT_BUILD_DEV

$(PTXDIST_WORKSPACE)/local_src/ct-build:
	@if ! ( \
	  cd "$(PTXDIST_WORKSPACE)/local_src" \
	  && git clone $(HOST_CT_BUILD_GIT_URL) ${HOST_CT_BUILD} \
	  && cd ${HOST_CT_BUILD} \
	  && git checkout ${PTXCONF_HOST_CT_BUILD_GIT_CHECKOUT_REF} \
	) \
	then \
	  printf "\nError cloning from ${HOST_CT_BUILD_GIT_URL}.\nRemoving ${PTXDIST_WORKSPACE}/local_src/${HOST_CT_BUILD}.\n\n"; \
	  rm -fr "$(PTXDIST_WORKSPACE)/local_src/${HOST_CT_BUILD}"; \
	fi

$(STATEDIR)/host-ct-build.get: | $(PTXDIST_WORKSPACE)/local_src/ct-build
	[ -d "$(firstword $|)" ] && cd $(firstword $|) && echo "INFO: packaging $$(git describe --tags) from GIT."

else

$(HOST_CT_BUILD_SOURCE):
	@$(call targetinfo)
	@$(call get, HOST_CT_BUILD)

endif

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

ifndef PTXCONF_HOST_CT_BUILD_DEV

$(STATEDIR)/host-ct-build.extract:
	@$(call targetinfo)
	@$(call clean, $(HOST_CT_BUILD_DIR))
	@$(call extract, HOST_CT_BUILD)
# In-project patching of ct-build is discouraged in favour of change requests
# aimed at the maintainers (Team DevTools) 
#	@$(call patchin, HOST_CT_BUILD)
	@$(call touch)

endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/host-ct-build.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/host-ct-build.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

HOST_CT_BUILD_INSTALL_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

$(STATEDIR)/host-ct-build.install: $(STATEDIR)/glibc.targetinstall $(STATEDIR)/gcclibs.targetinstall $(STATEDIR)/usr-rootfs.targetinstall
	@$(call targetinfo)
	@mkdir -p $(HOST_CT_BUILD_INSTALL_DIR)
	@cp -rf $(HOST_CT_BUILD_DIR)/* $(HOST_CT_BUILD_INSTALL_DIR)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/host-ct-build.clean:
	@$(call targetinfo)
	@$(call clean_pkg, HOST_CT_BUILD)
	@rm -rf $(HOST_CT_BUILD_INSTALL_DIR)


# vim: syntax=make
