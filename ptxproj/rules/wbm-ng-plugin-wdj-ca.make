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
PACKAGES-$(PTXCONF_WBM_NG_PLUGIN_WDJCA) += wbm-ng-plugin-wdj-ca

#
# Paths and names
#
WBM_NG_PLUGIN_WDJCA_VERSION        := 1.0.0
WBM_NG_PLUGIN_WDJCA                := wbm-wdj-ca-$(WBM_NG_PLUGIN_WDJCA_VERSION)
WBM_NG_PLUGIN_WDJCA_URL            := $(call jfrog_template_to_url, WBM_NG_PLUGIN_WDJCA)
WBM_NG_PLUGIN_WDJCA_SUFFIX         := $(suffix $(WBM_NG_PLUGIN_WDJCA_URL))
WBM_NG_PLUGIN_WDJCA_SOURCE         := $(SRCDIR)/$(WBM_NG_PLUGIN_WDJCA)$(WBM_NG_PLUGIN_WDJCA_SUFFIX)
WBM_NG_PLUGIN_WDJCA_MD5             = $(shell [ -f $(WBM_NG_PLUGIN_WDJCA_MD5_FILE) ] && cat $(WBM_NG_PLUGIN_WDJCA_MD5_FILE))
WBM_NG_PLUGIN_WDJCA_MD5_FILE       := $(WBM_NG_PLUGIN_WDJCA_SOURCE).md5
WBM_NG_PLUGIN_WDJCA_ARTIFACT        = $(call jfrog_get_filename,$(WBM_NG_PLUGIN_WDJCA_URL))
WBM_NG_PLUGIN_WDJCA_BUILDROOT_DIR  := $(BUILDDIR)/wbm-ng-plugin-wdj-ca
WBM_NG_PLUGIN_WDJCA_DIR            := $(WBM_NG_PLUGIN_WDJCA_BUILDROOT_DIR)
WBM_NG_PLUGIN_WDJCA_LICENSE        := unknown
WBM_NG_PLUGIN_WDJCA_MAKE_ENV       :=
ifeq ($(PTXCONF_WBM),y)
	WBM_NG_PLUGIN_WDJCA_TARGET_DIR     := /var/www/wbm-ng/plugins/wbm-wdj-ca
else
	WBM_NG_PLUGIN_WDJCA_TARGET_DIR     := /var/www/wbm/plugins/wbm-wdj-ca
endif

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(WBM_NG_PLUGIN_WDJCA_SOURCE):
	@$(call targetinfo)
	$(call ptx/in-path, PTXDIST_PATH, scripts/wago/artifactory.sh) fetch \
        '$(WBM_NG_PLUGIN_WDJCA_URL)' '$@' '$(WBM_NG_PLUGIN_WDJCA_MD5_FILE)'

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# use ptxdist default

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-wdj-ca.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-wdj-ca.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-wdj-ca.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-wdj-ca.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wbm-ng-plugin-wdj-ca)
	@$(call install_fixup, wbm-ng-plugin-wdj-ca, PRIORITY, optional)
	@$(call install_fixup, wbm-ng-plugin-wdj-ca, SECTION, base)
	@$(call install_fixup, wbm-ng-plugin-wdj-ca, AUTHOR,"Marius Warning, WAGO GmbH \& Co. KG")
	@$(call install_fixup, wbm-ng-plugin-wdj-ca, DESCRIPTION, missing)

	# create target directory itself
	@$(call install_copy, wbm-ng-plugin-wdj-ca, 0, 0, 0755, $(WBM_NG_PLUGIN_WDJCA_TARGET_DIR))

	# loop over all files and subdirectories (deep)
	@cd $(WBM_NG_PLUGIN_WDJCA_DIR) && \
	for object in $$( find ./* \( -path './series' -o -path './.ptxdist*' -o -path './.pc*' \) -prune -o -print ); do \
		if test -f $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-wdj-ca, 0, 0, 0644, $(WBM_NG_PLUGIN_WDJCA_DIR)/$$object, $(WBM_NG_PLUGIN_WDJCA_TARGET_DIR)/$$object); \
		elif test -d $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-wdj-ca, 0, 0, 0755, $(WBM_NG_PLUGIN_WDJCA_TARGET_DIR)/$$object); \
		fi; \
	done;

	# install the sudoers file
	@$(call install_copy, wbm-ng-plugin-wdj-ca, 0, 0, 0444, $(WBM_NG_PLUGIN_WDJCA_DIR)/etc/sudoers.d/wdj-ca, /etc/sudoers.d/wdj-ca)

	@$(call install_finish, wbm-ng-plugin-wdj-ca)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-wdj-ca.clean:
	@$(call targetinfo)
	@rm -rf $(PTXCONF_SYSROOT_TARGET)$(WBM_NG_PLUGIN_WDJCA_TARGET_DIR)
	@$(call clean_pkg, WBM_NG_PLUGIN_WDJCA)
	@rm -rf $(WBM_NG_PLUGIN_WDJCA_BUILDROOT_DIR)

# vim: syntax=make
