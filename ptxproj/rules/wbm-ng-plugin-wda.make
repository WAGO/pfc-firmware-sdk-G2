# -*-makefile-*-
#
# Copyright (C) 2021 by Markus Röhling (markus.roehling@wago.com), WAGO GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_WBM_NG_PLUGIN_WDA) += wbm-ng-plugin-wda

#
# Paths and names
#
WBM_NG_PLUGIN_WDA_VERSION        := 1.0.3
WBM_NG_PLUGIN_WDA                := wbm-wda-$(WBM_NG_PLUGIN_WDA_VERSION)
WBM_NG_PLUGIN_WDA_URL            := $(call jfrog_template_to_url, WBM_NG_PLUGIN_WDA)
WBM_NG_PLUGIN_WDA_SUFFIX         := $(suffix $(WBM_NG_PLUGIN_WDA_URL))
WBM_NG_PLUGIN_WDA_SOURCE         := $(SRCDIR)/$(WBM_NG_PLUGIN_WDA)$(WBM_NG_PLUGIN_WDA_SUFFIX)
WBM_NG_PLUGIN_WDA_MD5             = $(shell [ -f $(WBM_NG_PLUGIN_WDA_MD5_FILE) ] && cat $(WBM_NG_PLUGIN_WDA_MD5_FILE))
WBM_NG_PLUGIN_WDA_MD5_FILE       := $(WBM_NG_PLUGIN_WDA_SOURCE).md5
WBM_NG_PLUGIN_WDA_ARTIFACT        = $(call jfrog_get_filename,$(WBM_NG_PLUGIN_WDA_URL))
WBM_NG_PLUGIN_WDA_BUILDROOT_DIR  := $(BUILDDIR)/wbm-ng-plugin-wda
WBM_NG_PLUGIN_WDA_DIR            := $(WBM_NG_PLUGIN_WDA_BUILDROOT_DIR)
WBM_NG_PLUGIN_WDA_LICENSE        := unknown
WBM_NG_PLUGIN_WDA_MAKE_ENV       :=
ifeq ($(PTXCONF_WBM),y)
WBM_NG_PLUGIN_WDA_TARGET_DIR     := /var/www/wbm-ng/plugins/wbm-wda
else
WBM_NG_PLUGIN_WDA_TARGET_DIR     := /var/www/wbm/plugins/wbm-wda
endif

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(WBM_NG_PLUGIN_WDA_SOURCE):
	@$(call targetinfo)
	$(call ptx/in-path, PTXDIST_PATH, scripts/wago/artifactory.sh) fetch \
        '$(WBM_NG_PLUGIN_WDA_URL)' '$@' '$(WBM_NG_PLUGIN_WDA_MD5_FILE)'

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# use ptxdist default

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-wda.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-wda.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-wda.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-wda.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wbm-ng-plugin-wda)
	@$(call install_fixup, wbm-ng-plugin-wda, PRIORITY, optional)
	@$(call install_fixup, wbm-ng-plugin-wda, SECTION, base)
	@$(call install_fixup, wbm-ng-plugin-wda, AUTHOR,"Markus Roehling, WAGO GmbH \& Co. KG")
	@$(call install_fixup, wbm-ng-plugin-wda, DESCRIPTION, missing)

	# create target directory itself
	@$(call install_copy, wbm-ng-plugin-wda, 0, 0, 0755, $(WBM_NG_PLUGIN_WDA_TARGET_DIR))

	# loop over all files and subdirectories (deep)
	@cd $(WBM_NG_PLUGIN_WDA_DIR) && \
	for object in $$( find ./* \( -path './series' -o -path './.ptxdist*' -o -path './.pc*' \) -prune -o -print ); do \
		if test -f $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-wda, 0, 0, 0644, $(WBM_NG_PLUGIN_WDA_DIR)/$$object, $(WBM_NG_PLUGIN_WDA_TARGET_DIR)/$$object); \
		elif test -d $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-wda, 0, 0, 0755, $(WBM_NG_PLUGIN_WDA_TARGET_DIR)/$$object); \
		fi; \
	done;


	@$(call install_finish, wbm-ng-plugin-wda)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-wda.clean:
	@$(call targetinfo)
	@rm -rf $(PTXCONF_SYSROOT_TARGET)$(WBM_NG_PLUGIN_WDA_TARGET_DIR)
	@$(call clean_pkg, WBM_NG_PLUGIN_WDA)
	@rm -rf $(WBM_NG_PLUGIN_WDA_BUILDROOT_DIR)

# vim: syntax=make
