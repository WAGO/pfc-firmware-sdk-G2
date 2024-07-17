# -*-makefile-*-
#
# Copyright (C) 2023 by <Marius Warning, Simon Dick>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_ROOTLESSKIT_DEP) += rootlesskit-dep

#
# Paths and names
#
ROOTLESSKIT_DEP_VERSION		:= 2.0.2
ROOTLESSKIT_DEP_MD5				:= 2a4a44c2ab0dca2adaa9b93e1e99a742
ROOTLESSKIT_DEP						:= rootlesskit-dep-$(ROOTLESSKIT_DEP_VERSION)
ROOTLESSKIT_DEP_SUFFIX		:= tar.gz
ROOTLESSKIT_DEP_URL				:= https://github.com/rootless-containers/rootlesskit-dep/archive/refs/tags/v$(ROOTLESSKIT_DEP_VERSION).$(ROOTLESSKIT_DEP_SUFFIX)
ROOTLESSKIT_DEP_SOURCE		:= $(SRCDIR)/$(ROOTLESSKIT_DEP).$(ROOTLESSKIT_DEP_SUFFIX)
ROOTLESSKIT_DEP_DIR				:= $(BUILDDIR)/$(ROOTLESSKIT_DEP)
ROOTLESSKIT_DEP_LICENSE		:= Apache-2.0

ROOTLESSKIT_DEP_INSTALLDIR := $(ROOTLESSKIT_DEP_PKGDIR)/bin/rootlesskit

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(ROOTLESSKIT_DEP_SOURCE):
	@$(call targetinfo)
	@$(call get, ROOTLESSKIT_DEP)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/rootlesskit-dep.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/rootlesskit-dep.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/rootlesskit-dep.install:
	@$(call targetinfo)
	mkdir -p $(ROOTLESSKIT_DEP_INSTALLDIR) && \
	cp $(ROOTLESSKIT_DEP_DIR)/$(ROOTLESSKIT_DEP).$(ROOTLESSKIT_DEP_SUFFIX) $(ROOTLESSKIT_DEP_INSTALLDIR)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/rootlesskit-dep.targetinstall:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/rootlesskit-dep.clean:
	@$(call targetinfo)
	@$(call clean_pkg, ROOTLESSKIT_DEP)

# vim: syntax=make
