# -*-makefile-*-
#
# Copyright (C) 2021 by Marius Warning (marius.warning@wago.com), WAGO GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_WBM_NG_PLUGIN_WDS_CA) += wbm-ng-plugin-wds-ca

#
# Paths and names
#
WBM_NG_PLUGIN_WDS_CA_VERSION        := 1.0.2
WBM_NG_PLUGIN_WDS_CA                := wbm-wds-ca-$(WBM_NG_PLUGIN_WDS_CA_VERSION)
WBM_NG_PLUGIN_WDS_CA_URL            := $(call jfrog_template_to_url, WBM_NG_PLUGIN_WDS_CA)
WBM_NG_PLUGIN_WDS_CA_SUFFIX         := $(suffix $(WBM_NG_PLUGIN_WDS_CA_URL))
WBM_NG_PLUGIN_WDS_CA_SOURCE         := $(SRCDIR)/$(WBM_NG_PLUGIN_WDS_CA)$(WBM_NG_PLUGIN_WDS_CA_SUFFIX)
WBM_NG_PLUGIN_WDS_CA_MD5             = $(shell [ -f $(WBM_NG_PLUGIN_WDS_CA_MD5_FILE) ] && cat $(WBM_NG_PLUGIN_WDS_CA_MD5_FILE))
WBM_NG_PLUGIN_WDS_CA_MD5_FILE       := $(WBM_NG_PLUGIN_WDS_CA_SOURCE).md5
WBM_NG_PLUGIN_WDS_CA_ARTIFACT        = $(call jfrog_get_filename,$(WBM_NG_PLUGIN_WDS_CA_URL))
WBM_NG_PLUGIN_WDS_CA_BUILDROOT_DIR  := $(BUILDDIR)/wbm-ng-plugin-wds-ca
WBM_NG_PLUGIN_WDS_CA_DIR            := $(WBM_NG_PLUGIN_WDS_CA_BUILDROOT_DIR)
WBM_NG_PLUGIN_WDS_CA_LICENSE        := unknown
WBM_NG_PLUGIN_WDS_CA_MAKE_ENV       :=
ifeq ($(PTXCONF_WBM),y)
	WBM_NG_PLUGIN_WDS_CA_TARGET_DIR     := /var/www/wbm-ng/plugins/wbm-wds-ca
else
	WBM_NG_PLUGIN_WDS_CA_TARGET_DIR     := /var/www/wbm/plugins/wbm-wds-ca
endif

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(WBM_NG_PLUGIN_WDS_CA_SOURCE):
	@$(call targetinfo)
	$(call ptx/in-path, PTXDIST_PATH, scripts/wago/artifactory.sh) fetch \
        '$(WBM_NG_PLUGIN_WDS_CA_URL)' '$@' '$(WBM_NG_PLUGIN_WDS_CA_MD5_FILE)'

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# use ptxdist default

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-wds-ca.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-wds-ca.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-wds-ca.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-wds-ca.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wbm-ng-plugin-wds-ca)
	@$(call install_fixup, wbm-ng-plugin-wds-ca, PRIORITY, optional)
	@$(call install_fixup, wbm-ng-plugin-wds-ca, SECTION, base)
	@$(call install_fixup, wbm-ng-plugin-wds-ca, AUTHOR,"Marius Warning, WAGO GmbH \& Co. KG")
	@$(call install_fixup, wbm-ng-plugin-wds-ca, DESCRIPTION, missing)

	# create target directory itself
	@$(call install_copy, wbm-ng-plugin-wds-ca, 0, 0, 0755, $(WBM_NG_PLUGIN_WDS_CA_TARGET_DIR))

	# loop over all files and subdirectories (deep)
	@cd $(WBM_NG_PLUGIN_WDS_CA_DIR) && \
	for object in $$( find ./* \( -path './series' -o -path './.ptxdist*' -o -path './.pc*' \) -prune -o -print ); do \
		if test -f $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-wds-ca, 0, 0, 0644, $(WBM_NG_PLUGIN_WDS_CA_DIR)/$$object, $(WBM_NG_PLUGIN_WDS_CA_TARGET_DIR)/$$object); \
		elif test -d $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-wds-ca, 0, 0, 0755, $(WBM_NG_PLUGIN_WDS_CA_TARGET_DIR)/$$object); \
		fi; \
	done;

	# install the sudoers file
	@$(call install_copy, wbm-ng-plugin-wds-ca, 0, 0, 0444, $(WBM_NG_PLUGIN_WDS_CA_DIR)/etc/sudoers.d/wds-ca, /etc/sudoers.d/wds-ca)

	@$(call install_finish, wbm-ng-plugin-wds-ca)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-wds-ca.clean:
	@$(call targetinfo)
	@rm -rf $(PTXCONF_SYSROOT_TARGET)$(WBM_NG_PLUGIN_WDS_CA_TARGET_DIR)
	@$(call clean_pkg, WBM_NG_PLUGIN_WDS_CA)
	@rm -rf $(WBM_NG_PLUGIN_WDS_CA_BUILDROOT_DIR)

# vim: syntax=make
