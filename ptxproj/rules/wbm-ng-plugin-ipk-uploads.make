# -*-makefile-*-
#
# Copyright (C) 2017 by Marius Hellmeier (marius.hellmeier@wago.com), WAGO GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_WBM_NG_PLUGIN_IPK_UPLOADS) += wbm-ng-plugin-ipk-uploads

#
# Paths and names
#
WBM_NG_PLUGIN_IPK_UPLOADS_VERSION        := 2.1.1
WBM_NG_PLUGIN_IPK_UPLOADS                := wbm-ipk-uploads-$(WBM_NG_PLUGIN_IPK_UPLOADS_VERSION)
WBM_NG_PLUGIN_IPK_UPLOADS_URL            := $(call jfrog_template_to_url, WBM_NG_PLUGIN_IPK_UPLOADS)
WBM_NG_PLUGIN_IPK_UPLOADS_SUFFIX         := $(suffix $(WBM_NG_PLUGIN_IPK_UPLOADS_URL))
WBM_NG_PLUGIN_IPK_UPLOADS_SOURCE         := $(SRCDIR)/$(WBM_NG_PLUGIN_IPK_UPLOADS)$(WBM_NG_PLUGIN_IPK_UPLOADS_SUFFIX)
WBM_NG_PLUGIN_IPK_UPLOADS_MD5             = $(shell [ -f $(WBM_NG_PLUGIN_IPK_UPLOADS_MD5_FILE) ] && cat $(WBM_NG_PLUGIN_IPK_UPLOADS_MD5_FILE))
WBM_NG_PLUGIN_IPK_UPLOADS_MD5_FILE       := $(WBM_NG_PLUGIN_IPK_UPLOADS_SOURCE).md5
WBM_NG_PLUGIN_IPK_UPLOADS_ARTIFACT        = $(call jfrog_get_filename,$(WBM_NG_PLUGIN_IPK_UPLOADS_URL))
WBM_NG_PLUGIN_IPK_UPLOADS_BUILDROOT_DIR  := $(BUILDDIR)/wbm-ng-plugin-ipk-uploads
WBM_NG_PLUGIN_IPK_UPLOADS_DIR            := $(WBM_NG_PLUGIN_IPK_UPLOADS_BUILDROOT_DIR)
WBM_NG_PLUGIN_IPK_UPLOADS_LICENSE        := unknown
WBM_NG_PLUGIN_IPK_UPLOADS_MAKE_ENV       :=
ifeq ($(PTXCONF_WBM),y)
WBM_NG_PLUGIN_IPK_UPLOADS_TARGET_DIR     := /var/www/wbm-ng/plugins/wbm-ipk-uploads
else
WBM_NG_PLUGIN_IPK_UPLOADS_TARGET_DIR     := /var/www/wbm/plugins/wbm-ipk-uploads
endif

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(WBM_NG_PLUGIN_IPK_UPLOADS_SOURCE):
	@$(call targetinfo)
	$(call ptx/in-path, PTXDIST_PATH, scripts/wago/artifactory.sh) fetch \
        '$(WBM_NG_PLUGIN_IPK_UPLOADS_URL)' '$@' '$(WBM_NG_PLUGIN_IPK_UPLOADS_MD5_FILE)'

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# use ptxdist default

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-ipk-uploads.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-ipk-uploads.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-ipk-uploads.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-ipk-uploads.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wbm-ng-plugin-ipk-uploads)
	@$(call install_fixup, wbm-ng-plugin-ipk-uploads, PRIORITY, optional)
	@$(call install_fixup, wbm-ng-plugin-ipk-uploads, SECTION, base)
	@$(call install_fixup, wbm-ng-plugin-ipk-uploads, AUTHOR,"Marius Hellmeier, WAGO GmbH \& Co. KG")
	@$(call install_fixup, wbm-ng-plugin-ipk-uploads, DESCRIPTION, missing)

	# create a directory owned by user and group "www" to store the uploaded file for configtool
	@$(call install_copy, wbm-ng-plugin-ipk-uploads, 12, 102, 0750, /var/downloads/update-script)

	# create target directories
	$(call install_copy, wbm-ng-plugin-ipk-uploads, 0, 0, 0755, $(WBM_NG_PLUGIN_IPK_UPLOADS_TARGET_DIR)/platform/pfcXXX/parameter/infos);
	$(call install_copy, wbm-ng-plugin-ipk-uploads, 0, 0, 0755, $(WBM_NG_PLUGIN_IPK_UPLOADS_TARGET_DIR)/platform/pfcXXX/parameter/mappings);
	
	# Config Tools und Schnipseldatei
	$(call install_copy, wbm-ng-plugin-ipk-uploads, 0, 0, 0755, $(WBM_NG_PLUGIN_IPK_UPLOADS_DIR)/etc/config-tools/install_ipk, /etc/config-tools/install_ipk);
	$(call install_copy, wbm-ng-plugin-ipk-uploads, 0, 0, 0444, $(WBM_NG_PLUGIN_IPK_UPLOADS_DIR)/etc/sudoers.d/install_ipk, /etc/sudoers.d/install_ipk);
	
	# WBM-Plugin
	$(call install_copy, wbm-ng-plugin-ipk-uploads, 0, 0, 0644, $(WBM_NG_PLUGIN_IPK_UPLOADS_DIR)/manifest.json, $(WBM_NG_PLUGIN_IPK_UPLOADS_TARGET_DIR)/manifest.json);
	$(call install_copy, wbm-ng-plugin-ipk-uploads, 0, 0, 0644, $(WBM_NG_PLUGIN_IPK_UPLOADS_DIR)/ipk-uploads.js, $(WBM_NG_PLUGIN_IPK_UPLOADS_TARGET_DIR)/ipk-uploads.js);	
	$(call install_copy, wbm-ng-plugin-ipk-uploads, 0, 0, 0644, $(WBM_NG_PLUGIN_IPK_UPLOADS_DIR)/platform/pfcXXX/parameter/infos/activate-software.json, $(WBM_NG_PLUGIN_IPK_UPLOADS_TARGET_DIR)/platform/pfcXXX/parameter/infos/activate-software.json);
	$(call install_copy, wbm-ng-plugin-ipk-uploads, 0, 0, 0644, $(WBM_NG_PLUGIN_IPK_UPLOADS_DIR)/platform/pfcXXX/parameter/mappings/activate-software.json, $(WBM_NG_PLUGIN_IPK_UPLOADS_TARGET_DIR)/platform/pfcXXX/parameter/mappings/activate-software.json);
	
	@$(call install_finish, wbm-ng-plugin-ipk-uploads)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-ipk-uploads.clean:
	@$(call targetinfo)
	@rm -rf $(PTXCONF_SYSROOT_TARGET)$(WBM_NG_PLUGIN_IPK_UPLOADS_TARGET_DIR)
	@$(call clean_pkg, WBM_NG_PLUGIN_IPK_UPLOADS)
	@rm -rf $(WBM_NG_PLUGIN_IPK_UPLOADS_BUILDROOT_DIR)

# vim: syntax=make
