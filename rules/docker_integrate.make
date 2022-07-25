# -*-makefile-*-
#
# Copyright (C) 2022 by WAGO GmbH
# <Marius.Warning@wago.com>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_DOCKER_INTEGRATE) += docker_integrate


#
# Paths and names
#

DOCKER_BASE								:= docker
DOCKER_INTEGRATE					:= $(DOCKER_BASE)-integrate
DOCKER_IPK_SUFFIX					:= armhf.ipk
ARTIFACTORY_BASE_URL			:= $(call remove_quotes, ${PTXCONF_ARTIFACTORY_BASE_URL})
DOCKER_REPO								:= pfc-generic-dev-local
DOCKER_REPO_PATH					:= wago/pfc/development/trunk/pfcXXX-docker-ipk/platform-wago-pfcXXX/packages
DOCKER_BASE_VERSION				:= 20.10.14
DOCKER_INTEGRATE_VERSION	:= $(DOCKER_BASE_VERSION)
DOCKER_IPK								:= $(DOCKER_BASE)_$(DOCKER_BASE_VERSION)_$(DOCKER_IPK_SUFFIX)
DOCKER_IPK_URL						:= ${ARTIFACTORY_BASE_URL}/$(DOCKER_REPO)/$(DOCKER_REPO_PATH)/$(DOCKER_IPK)

DOCKER_INTEGRATE_DIR			:= $(BUILDDIR)/$(DOCKER_INTEGRATE)
DOCKER_MD5_FILE						:= ${DOCKER_INTEGRATE_DIR}/$(DOCKER)_$(DOCKER_VERSION)_$(DOCKER_IPK_SUFFIX).md5
DOCKER_INTEGRATE_MD5			= $(shell [ -f "$(DOCKER_MD5_FILE)" ] && cat "$(DOCKER_MD5_FILE)")
DOCKER_INTEGRATE_LICENSE	:= Apache 2.0

DOCKER_INTEGRATE_PACKAGE_NAME             := $(DOCKER_INTEGRATE)_$(DOCKER_INTEGRATE_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
DOCKER_INTEGRATE_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/docker_integrate.get:
	@$(call targetinfo)
	
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	mkdir -p $(DOCKER_INTEGRATE_DIR) && \
		tar xvzf $(DOCKER_INTEGRATE_PLATFORMCONFIGPACKAGEDIR)/$(DOCKER_INTEGRATE_PACKAGE_NAME).tgz -C $(DOCKER_INTEGRATE_DIR)

else
	@mkdir -p $(DOCKER_INTEGRATE_DIR)
	${PTXDIST_WORKSPACE}/scripts/wago/artifactory.sh fetch \
		'$(DOCKER_IPK_URL)' '${DOCKER_INTEGRATE_DIR}/$(DOCKER_IPK)' '$(DOCKER_MD5_FILE)'
	
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	mkdir -p $(DOCKER_INTEGRATE_PLATFORMCONFIGPACKAGEDIR)
	cd $(DOCKER_INTEGRATE_DIR) && tar cvzf $(DOCKER_INTEGRATE_PLATFORMCONFIGPACKAGEDIR)/$(DOCKER_INTEGRATE_PACKAGE_NAME).tgz *
endif
endif
		
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/docker_integrate.extract:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/docker_integrate.prepare:
	@$(call targetinfo)	
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/docker_integrate.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/docker_integrate.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/docker_integrate.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init, docker_integrate)
	@$(call install_fixup,docker_integrate,PRIORITY,optional)
	@$(call install_fixup,docker_integrate,SECTION,base)
	@$(call install_fixup,docker_integrate,AUTHOR,"Marius Warning <Marius.Warning@wago.com>")
	@$(call install_fixup,docker_integrate,DESCRIPTION,"copies docker ipk to /opt/wago-docker directory")
	
	
	@$(call install_copy, docker_integrate, 0, 0, 0755, $(DOCKER_INTEGRATE_DIR)/$(DOCKER_IPK), /opt/wago-docker/$(DOCKER_IPK))
	
	@$(call install_finish,docker_integrate)
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/docker_integrate.clean:
	@$(call targetinfo)
	@$(call clean_pkg, DOCKER_INTEGRATE)

# vim: syntax=make
