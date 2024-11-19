# -*-makefile-*-
#
# Copyright (C) 2022 by Marius Hellmeier (marius.hellmeier@wago.com), WAGO GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_WAGO_PARAMETER_SERVICE_WDA_OPENAPI) += wago-parameter-service-wda-openapi

#
# Paths and names
#
WAGO_PARAMETER_SERVICE_WDA_OPENAPI_VERSION        := 1.4.1
WAGO_PARAMETER_SERVICE_WDA_OPENAPI                := wda-openapi-$(WAGO_PARAMETER_SERVICE_WDA_OPENAPI_VERSION)
WAGO_PARAMETER_SERVICE_WDA_OPENAPI_URL            := $(call jfrog_template_to_url, WAGO_PARAMETER_SERVICE_WDA_OPENAPI)
WAGO_PARAMETER_SERVICE_WDA_OPENAPI_SUFFIX         := $(suffix $(WAGO_PARAMETER_SERVICE_WDA_OPENAPI_URL))
WAGO_PARAMETER_SERVICE_WDA_OPENAPI_SOURCE         := $(SRCDIR)/$(WAGO_PARAMETER_SERVICE_WDA_OPENAPI)$(WAGO_PARAMETER_SERVICE_WDA_OPENAPI_SUFFIX)
WAGO_PARAMETER_SERVICE_WDA_OPENAPI_MD5             = $(shell [ -f $(WAGO_PARAMETER_SERVICE_WDA_OPENAPI_MD5_FILE) ] && cat $(WAGO_PARAMETER_SERVICE_WDA_OPENAPI_MD5_FILE))
WAGO_PARAMETER_SERVICE_WDA_OPENAPI_MD5_FILE       := $(WAGO_PARAMETER_SERVICE_WDA_OPENAPI_SOURCE).md5
WAGO_PARAMETER_SERVICE_WDA_OPENAPI_ARTIFACT        = $(call jfrog_get_filename,$(WAGO_PARAMETER_SERVICE_WDA_OPENAPI_URL))

WAGO_PARAMETER_SERVICE_WDA_OPENAPI_BUILDROOT_DIR  := $(BUILDDIR)/wago-parameter-service-wda-openapi
WAGO_PARAMETER_SERVICE_WDA_OPENAPI_DIR            := $(WAGO_PARAMETER_SERVICE_WDA_OPENAPI_BUILDROOT_DIR)
WAGO_PARAMETER_SERVICE_WDA_OPENAPI_TARGET_DIR     := /var/www/openapi

WAGO_PARAMETER_SERVICE_WDA_OPENAPI_PACKAGE_NAME := wago-parameter-service-wda-openapi_$(WAGO_PARAMETER_SERVICE_WDA_OPENAPI_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
WAGO_PARAMETER_SERVICE_WDA_OPENAPI_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

WAGO_PARAMETER_SERVICE_WDA_OPENAPI_LICENSE        := unknown
WAGO_PARAMETER_SERVICE_WDA_OPENAPI_MAKE_ENV       :=


# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(WAGO_PARAMETER_SERVICE_WDA_OPENAPI_SOURCE):
	@$(call targetinfo)
	$(call ptx/in-path, PTXDIST_PATH, scripts/wago/artifactory.sh) fetch \
        '$(WAGO_PARAMETER_SERVICE_WDA_OPENAPI_URL)' '$@' '$(WAGO_PARAMETER_SERVICE_WDA_OPENAPI_MD5_FILE)'

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# use ptxdist default

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-parameter-service-wda-openapi.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-parameter-service-wda-openapi.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-parameter-service-wda-openapi.install:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# "normal" mode: install static files into target directories

	@mkdir -p $(WAGO_PARAMETER_SERVICE_WDA_OPENAPI_PKGDIR)/$(WAGO_PARAMETER_SERVICE_WDA_OPENAPI_TARGET_DIR)
	@cp $(WAGO_PARAMETER_SERVICE_WDA_OPENAPI_BUILDROOT_DIR)/dist/wda.openapi.json $(WAGO_PARAMETER_SERVICE_WDA_OPENAPI_PKGDIR)$(WAGO_PARAMETER_SERVICE_WDA_OPENAPI_TARGET_DIR)/
ifdef PTXCONF_WAGO_PARAMETER_SERVICE_WDA_OPENAPI_HTML
	@cp $(WAGO_PARAMETER_SERVICE_WDA_OPENAPI_BUILDROOT_DIR)/dist/wda.openapi.html $(WAGO_PARAMETER_SERVICE_WDA_OPENAPI_PKGDIR)$(WAGO_PARAMETER_SERVICE_WDA_OPENAPI_TARGET_DIR)/
endif
	@chmod 644 $(WAGO_PARAMETER_SERVICE_WDA_OPENAPI_PKGDIR)$(WAGO_PARAMETER_SERVICE_WDA_OPENAPI_TARGET_DIR)/*

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
# avoid to install header files in BSP mode: for internal use only

	@mkdir -p $(WAGO_PARAMETER_SERVICE_WDA_OPENAPI_PLATFORMCONFIGPACKAGEDIR) && \
	  cd $(WAGO_PARAMETER_SERVICE_WDA_OPENAPI_PKGDIR) && tar cvzf $(WAGO_PARAMETER_SERVICE_WDA_OPENAPI_PLATFORMCONFIGPACKAGEDIR)/$(WAGO_PARAMETER_SERVICE_WDA_OPENAPI_PACKAGE_NAME).tgz *

endif

else
# BSP mode: install ipk contents to packages/<project name>
	@mkdir -p $(WAGO_PARAMETER_SERVICE_WDA_OPENAPI_PKGDIR) && \
	  tar xvzf $(WAGO_PARAMETER_SERVICE_WDA_OPENAPI_PLATFORMCONFIGPACKAGEDIR)/$(WAGO_PARAMETER_SERVICE_WDA_OPENAPI_PACKAGE_NAME).tgz -C $(WAGO_PARAMETER_SERVICE_WDA_OPENAPI_PKGDIR)

endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-parameter-service-wda-openapi.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wago-parameter-service-wda-openapi)
	@$(call install_fixup, wago-parameter-service-wda-openapi, PRIORITY, optional)
	@$(call install_fixup, wago-parameter-service-wda-openapi, SECTION, base)
	@$(call install_fixup, wago-parameter-service-wda-openapi, AUTHOR,"Marius Hellmeier, WAGO GmbH \& Co. KG")
	@$(call install_fixup, wago-parameter-service-wda-openapi, DESCRIPTION, missing)

	# install definition file and html file
	@$(call install_copy, wago-parameter-service-wda-openapi, 0, 0, 0755, $(WAGO_PARAMETER_SERVICE_WDA_OPENAPI_TARGET_DIR))
	@$(call install_glob, wago-parameter-service-wda-openapi, 0, 0, -, $(WAGO_PARAMETER_SERVICE_WDA_OPENAPI_TARGET_DIR), *)

	# add symlink to license file
	@$(call install_link, wago-parameter-service-wda-openapi, /usr/share/licenses/wago.txt, $(WAGO_PARAMETER_SERVICE_WDA_OPENAPI_TARGET_DIR)/wda.license.txt)

	@$(call install_finish, wago-parameter-service-wda-openapi)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-parameter-service-wda-openapi.clean:
	@$(call targetinfo)
	@rm -rf $(PTXCONF_SYSROOT_TARGET)$(WAGO_PARAMETER_SERVICE_WDA_OPENAPI_TARGET_DIR)
	@$(call clean_pkg, WAGO_PARAMETER_SERVICE_WDA_OPENAPI)
	@rm -rf $(WAGO_PARAMETER_SERVICE_WDA_OPENAPI_BUILDROOT_DIR)

# vim: syntax=make
