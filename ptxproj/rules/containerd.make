# -*-makefile-*-
#
# Copyright (C) 2022 by <Simon Dick>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CONTAINERD) += containerd

#
# Paths and names
#
CONTAINERD_VERSION	:= 1.7.13
CONTAINERD_MD5			:= a678f360eddf6bf0f9f1eac77cb79901
CONTAINERD_GITCOMMIT:= 7c3aca7
CONTAINERD					:= containerd-$(CONTAINERD_VERSION)
CONTAINERD_SUFFIX		:= tar.gz
CONTAINERD_URL			:= https://github.com/containerd/containerd/archive/refs/tags/v$(CONTAINERD_VERSION).$(CONTAINERD_SUFFIX)
CONTAINERD_SOURCE		:= $(SRCDIR)/$(CONTAINERD).$(CONTAINERD_SUFFIX)
CONTAINERD_DIR			:= $(BUILDDIR)/$(CONTAINERD)
CONTAINERD_LICENSE	:= Apache-2.0

CONTAINERD_INSTALLDIR := $(CONTAINERD_PKGDIR)/bin/docker

BUILDTAGS=no_btrfs

CONTAINERD_CONF_TOOL:=NO
CONTAINERD_GO_ENV:= \
	AR=$(CROSS_AR) \
	CC=$(CROSS_CC) \
	CXX=$(CROSS_CXX) \
	CGO_CFLAGS=$(CROSS_CFLAGS) \
	CGO_LDFLAGS=$(CROSS_LDFLAGS) \
	PKG_CONFIG=$(CROSS_PKG_CONFIG) \
	GOOS=linux CGO_ENABLED=1

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(CONTAINERD_SOURCE):
	@$(call targetinfo)
	@$(call get, CONTAINERD)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/containerd.compile:
	@$(call targetinfo)
	@PATH=$(CROSS_PATH):$(HOST_GO_BIN_DIR) ; \
		cd $(CONTAINERD_DIR) &&  \
		BUILDTAGS=$(BUILDTAGS) $(CONTAINERD_GO_ENV) $(GO_ARCH) $(MAKE) VERSION=$(CONTAINERD_VERSION) REVISION=$(CONTAINERD_GITCOMMIT)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/containerd.install:
	@$(call targetinfo)
	mkdir -p $(CONTAINERD_INSTALLDIR) && \
	cp $(CONTAINERD_DIR)/bin/containerd $(CONTAINERD_INSTALLDIR)/ && \
	cp $(CONTAINERD_DIR)/bin/containerd-shim $(CONTAINERD_INSTALLDIR)/ && \
	cp $(CONTAINERD_DIR)/bin/containerd-shim-runc-v2 $(CONTAINERD_INSTALLDIR)/ && \
	cp $(CONTAINERD_DIR)/bin/ctr $(CONTAINERD_INSTALLDIR)/
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/containerd.targetinstall:
	@$(call targetinfo)

	@$(call install_init, containerd)
	@$(call install_fixup, containerd,PRIORITY,optional)
	@$(call install_fixup, containerd,SECTION,base)
	@$(call install_fixup, containerd,AUTHOR,"<Simon Dick, Marius Warning>")
	@$(call install_fixup, containerd,DESCRIPTION,missing)
	
	@$(call install_copy, containerd, 0, 0, 0644, $(CONTAINERD_DIR)/LICENSE, /usr/share/licenses/oss/license.containerd_$(CONTAINERD_VERSION).txt)

	@$(call install_finish, containerd)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/containerd.clean:
	@$(call targetinfo)
	@$(call clean_pkg, CONTAINERD)

# vim: syntax=make
