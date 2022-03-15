# -*-makefile-*-
#
# Copyright (C) 2021 by WAGO GmbH @AUTHOR@ Co. KG
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_SSSD_REPO_IPK) += sssd-repo-ipk

SSSD_REPO_IPK_VERSION	:= 1.0
SSSD_REPO_IPK		:= sssd-repo-ipk-$(SSSD_REPO_IPK_VERSION)
SSSD_DIR		:= $(BUILDDIR)/$(SSSD_REPO_IPK)

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

#$(STATEDIR)/sssd-repo-ipk.get:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

#$(STATEDIR)/sssd-repo-ipk.extract:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#$(STATEDIR)/sssd-repo-ipk.prepare:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/sssd-repo-ipk.compile:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/sssd-repo-ipk.install: $(STATEDIR)/wago-custom-install.targetinstall
	@$(call targetinfo)

	# create repo IPK for SSSD and dependencies
	$(PTXDIST_WORKSPACE)/scripts/sssd-helpers/make-meta-ipk-sssd.sh $(SSSD_REPO_IPK_VERSION) $(SSSD_DIR)
#      #$(PTXDIST_WORKSPACE)/scripts/bacnet-helpers/make-metaipk_bacnet.sh $(BACNET_VERSION) $(BACNETSTACK_REVISION) \
#      #       $(PTXCONF_PROJECT) $(PTXCONF_PROJECT_VERSION) $(PTXCONF_OPKG_OPKG_CONF_HOST)

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/sssd-repo-ipk.targetinstall:
	@$(call targetinfo)

#	#@$(call install_init, sssd-repo-ipk)
#	#@$(call install_fixup,sssd-repo-ipk,PRIORITY,optional)
#	#@$(call install_fixup,sssd-repo-ipk,SECTION,base)
#	#@$(call install_fixup,sssd-repo-ipk,AUTHOR,"WAGO GmbH \& Co. KG")
#	#@$(call install_fixup,sssd-repo-ipk,DESCRIPTION,missing)
#	# ...
#	#@$(call install_finish,sssd-repo-ipk)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/sssd-repo-ipk.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, SSSD_REPO_IPK)

# vim: syntax=make
