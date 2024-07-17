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
PACKAGES-$(PTXCONF_WAGO_AUTH_OPENAPI) += wago-auth-openapi

#
# Paths and names
#
WAGO_AUTH_OPENAPI_VERSION        := 1.1.0
WAGO_AUTH_OPENAPI                := openapi-auth-$(WAGO_AUTH_OPENAPI_VERSION)
WAGO_AUTH_OPENAPI_URL            := $(call jfrog_template_to_url, WAGO_AUTH_OPENAPI)
WAGO_AUTH_OPENAPI_SUFFIX         := $(suffix $(WAGO_AUTH_OPENAPI_URL))
WAGO_AUTH_OPENAPI_SOURCE         := $(SRCDIR)/$(WAGO_AUTH_OPENAPI)$(WAGO_AUTH_OPENAPI_SUFFIX)
WAGO_AUTH_OPENAPI_MD5             = $(shell [ -f $(WAGO_AUTH_OPENAPI_MD5_FILE) ] && cat $(WAGO_AUTH_OPENAPI_MD5_FILE))
WAGO_AUTH_OPENAPI_MD5_FILE       := $(WAGO_AUTH_OPENAPI_SOURCE).md5
WAGO_AUTH_OPENAPI_ARTIFACT        = $(call jfrog_get_filename,$(WAGO_AUTH_OPENAPI_URL))

WAGO_AUTH_OPENAPI_BUILDROOT_DIR  := $(BUILDDIR)/wago-auth-openapi
WAGO_AUTH_OPENAPI_DIR            := $(WAGO_AUTH_OPENAPI_BUILDROOT_DIR)
WAGO_AUTH_OPENAPI_TARGET_DIR     := /var/www/openapi

WAGO_AUTH_OPENAPI_PACKAGE_NAME := wago-auth-openapi_$(WAGO_AUTH_OPENAPI_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
WAGO_AUTH_OPENAPI_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

WAGO_AUTH_OPENAPI_LICENSE        := unknown
WAGO_AUTH_OPENAPI_MAKE_ENV       :=


# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(WAGO_AUTH_OPENAPI_SOURCE):
	@$(call targetinfo)
	$(call ptx/in-path, PTXDIST_PATH, scripts/wago/artifactory.sh) fetch \
        '$(WAGO_AUTH_OPENAPI_URL)' '$@' '$(WAGO_AUTH_OPENAPI_MD5_FILE)'

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# use ptxdist default

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-auth-openapi.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-auth-openapi.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-auth-openapi.install:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# "normal" mode: install static files into target directories

	@mkdir -p $(WAGO_AUTH_OPENAPI_PKGDIR)/$(WAGO_AUTH_OPENAPI_TARGET_DIR)
	@cp $(WAGO_AUTH_OPENAPI_BUILDROOT_DIR)/dist/auth.openapi.json $(WAGO_AUTH_OPENAPI_PKGDIR)$(WAGO_AUTH_OPENAPI_TARGET_DIR)/
ifdef PTXCONF_WAGO_AUTH_OPENAPI_HTML
	@cp $(WAGO_AUTH_OPENAPI_BUILDROOT_DIR)/dist/auth.openapi.html $(WAGO_AUTH_OPENAPI_PKGDIR)$(WAGO_AUTH_OPENAPI_TARGET_DIR)/
endif
	@chmod 644 $(WAGO_AUTH_OPENAPI_PKGDIR)$(WAGO_AUTH_OPENAPI_TARGET_DIR)/*

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
# avoid to install header files in BSP mode: for internal use only

	@mkdir -p $(WAGO_AUTH_OPENAPI_PLATFORMCONFIGPACKAGEDIR) && \
	  cd $(WAGO_AUTH_OPENAPI_PKGDIR) && tar cvzf $(WAGO_AUTH_OPENAPI_PLATFORMCONFIGPACKAGEDIR)/$(WAGO_AUTH_OPENAPI_PACKAGE_NAME).tgz *

endif

else
# BSP mode: install ipk contents to packages/<project name>
	@mkdir -p $(WAGO_AUTH_OPENAPI_PKGDIR) && \
	  tar xvzf $(WAGO_AUTH_OPENAPI_PLATFORMCONFIGPACKAGEDIR)/$(WAGO_AUTH_OPENAPI_PACKAGE_NAME).tgz -C $(WAGO_AUTH_OPENAPI_PKGDIR)

endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-auth-openapi.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wago-auth-openapi)
	@$(call install_fixup, wago-auth-openapi, PRIORITY, optional)
	@$(call install_fixup, wago-auth-openapi, SECTION, base)
	@$(call install_fixup, wago-auth-openapi, AUTHOR,"Marius Hellmeier, WAGO GmbH \& Co. KG")
	@$(call install_fixup, wago-auth-openapi, DESCRIPTION, missing)

	# install definition file and html file
	@$(call install_copy, wago-auth-openapi, 0, 0, 0755, $(WAGO_AUTH_OPENAPI_TARGET_DIR))
	@$(call install_glob, wago-auth-openapi, 0, 0, -, $(WAGO_AUTH_OPENAPI_TARGET_DIR), *)

	# add symlink to license file
	@$(call install_link, wago-auth-openapi, /usr/share/licenses/wago.txt, $(WAGO_AUTH_OPENAPI_TARGET_DIR)/auth.license.txt)

	@$(call install_finish, wago-auth-openapi)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-auth-openapi.clean:
	@$(call targetinfo)
	@rm -rf $(PTXCONF_SYSROOT_TARGET)$(WAGO_AUTH_OPENAPI_TARGET_DIR)
	@$(call clean_pkg, WAGO_AUTH_OPENAPI)
	@rm -rf $(WAGO_AUTH_OPENAPI_BUILDROOT_DIR)

# vim: syntax=make
