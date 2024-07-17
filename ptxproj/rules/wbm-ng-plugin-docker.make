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
PACKAGES-$(PTXCONF_WBM_NG_PLUGIN_DOCKER) += wbm-ng-plugin-docker

#
# Paths and names
#
WBM_NG_PLUGIN_DOCKER_VERSION        := 1.1.1
WBM_NG_PLUGIN_DOCKER                := wbm-docker-$(WBM_NG_PLUGIN_DOCKER_VERSION)
WBM_NG_PLUGIN_DOCKER_URL            := $(call jfrog_template_to_url, WBM_NG_PLUGIN_DOCKER)
WBM_NG_PLUGIN_DOCKER_SUFFIX         := $(suffix $(WBM_NG_PLUGIN_DOCKER_URL))
WBM_NG_PLUGIN_DOCKER_SOURCE         := $(SRCDIR)/$(WBM_NG_PLUGIN_DOCKER)$(WBM_NG_PLUGIN_DOCKER_SUFFIX)
WBM_NG_PLUGIN_DOCKER_MD5             = $(shell [ -f $(WBM_NG_PLUGIN_DOCKER_MD5_FILE) ] && cat $(WBM_NG_PLUGIN_DOCKER_MD5_FILE))
WBM_NG_PLUGIN_DOCKER_MD5_FILE       := $(WBM_NG_PLUGIN_DOCKER_SOURCE).md5
WBM_NG_PLUGIN_DOCKER_ARTIFACT        = $(call jfrog_get_filename,$(WBM_NG_PLUGIN_DOCKER_URL))
WBM_NG_PLUGIN_DOCKER_BUILDROOT_DIR  := $(BUILDDIR)/wbm-ng-plugin-docker
WBM_NG_PLUGIN_DOCKER_DIR            := $(WBM_NG_PLUGIN_DOCKER_BUILDROOT_DIR)
WBM_NG_PLUGIN_DOCKER_LICENSE        := unknown
WBM_NG_PLUGIN_DOCKER_MAKE_ENV       :=
ifeq ($(PTXCONF_WBM),y)
	WBM_NG_PLUGIN_DOCKER_TARGET_DIR     := /var/www/wbm-ng/plugins/wbm-docker
else
	WBM_NG_PLUGIN_DOCKER_TARGET_DIR     := /var/www/wbm/plugins/wbm-docker
endif

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(WBM_NG_PLUGIN_DOCKER_SOURCE):
	@$(call targetinfo)
	$(call ptx/in-path, PTXDIST_PATH, scripts/wago/artifactory.sh) fetch \
        '$(WBM_NG_PLUGIN_DOCKER_URL)' '$@' '$(WBM_NG_PLUGIN_DOCKER_MD5_FILE)'

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# use ptxdist default

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-docker.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-docker.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-docker.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-docker.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wbm-ng-plugin-docker)
	@$(call install_fixup, wbm-ng-plugin-docker, PRIORITY, optional)
	@$(call install_fixup, wbm-ng-plugin-docker, SECTION, base)
	@$(call install_fixup, wbm-ng-plugin-docker, AUTHOR,"Marius Warning, WAGO GmbH \& Co. KG")
	@$(call install_fixup, wbm-ng-plugin-docker, DESCRIPTION, missing)

	# create target directory itself
	@$(call install_copy, wbm-ng-plugin-docker, 0, 0, 0755, $(WBM_NG_PLUGIN_DOCKER_TARGET_DIR))

	# loop over all files and subdirectories (deep)
	@cd $(WBM_NG_PLUGIN_DOCKER_DIR) && \
	for object in $$( find ./* \( -path './series' -o -path './.ptxdist*' -o -path './.pc*' \) -prune -o -print ); do \
		if test -f $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-docker, 0, 0, 0644, $(WBM_NG_PLUGIN_DOCKER_DIR)/$$object, $(WBM_NG_PLUGIN_DOCKER_TARGET_DIR)/$$object); \
		elif test -d $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-docker, 0, 0, 0755, $(WBM_NG_PLUGIN_DOCKER_TARGET_DIR)/$$object); \
		fi; \
	done;


	@$(call install_finish, wbm-ng-plugin-docker)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-docker.clean:
	@$(call targetinfo)
	@rm -rf $(PTXCONF_SYSROOT_TARGET)$(WBM_NG_PLUGIN_DOCKER_TARGET_DIR)
	@$(call clean_pkg, WBM_NG_PLUGIN_DOCKER)
	@rm -rf $(WBM_NG_PLUGIN_DOCKER_BUILDROOT_DIR)

# vim: syntax=make
