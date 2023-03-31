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
PACKAGES-$(PTXCONF_ROOTLESSKIT) += rootlesskit

#
# Paths and names
#
ROOTLESSKIT_VERSION		:= 0.14.4
ROOTLESSKIT_MD5				:= 2cb86f71c902d26931c392a49c3b21b6
ROOTLESSKIT						:= rootlesskit-$(ROOTLESSKIT_VERSION)
ROOTLESSKIT_SUFFIX		:= tar.gz
ROOTLESSKIT_URL				:= https://github.com/rootless-containers/rootlesskit/archive/refs/tags/v$(ROOTLESSKIT_VERSION).$(ROOTLESSKIT_SUFFIX)
ROOTLESSKIT_SOURCE		:= $(SRCDIR)/$(ROOTLESSKIT).$(ROOTLESSKIT_SUFFIX)
ROOTLESSKIT_DIR				:= $(BUILDDIR)/$(ROOTLESSKIT)
ROOTLESSKIT_LICENSE		:= Apache-2.0

ROOTLESSKIT_INSTALLDIR := $(ROOTLESSKIT_PKGDIR)/bin/docker

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(ROOTLESSKIT_SOURCE):
	@$(call targetinfo)
	@$(call get, ROOTLESSKIT)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

ROOTLESSKIT_CONF_ENV	:= $(CROSS_ENV)
ROOTLESSKIT_CONF_TOOL	:= NO

ROOTLESSKIT_GO_ENV:= \
	AR=$(CROSS_AR) \
	CC=$(CROSS_CC) \
	CXX=$(CROSS_CXX) \
	CGO_CFLAGS=$(CUSTOM_CGO_CFLAGS) \
	CGO_LDFLAGS=$(CROSS_LDFLAGS) \
	PKG_CONFIG=$(CROSS_PKG_CONFIG) \
	GOPATH=$(ROOTLESSKIT_DIR)/go \
	GOOS=linux GOARCH=arm GOARM=7 CGO_ENABLED=1

$(STATEDIR)/rootlesskit.prepare:
	@$(call targetinfo)
	mkdir -p $(ROOTLESSKIT_DIR) && cd $(ROOTLESSKIT_DIR) && \
	cp $(PTXDIST_SYSROOT_TARGET)/bin/rootlesskit/rootlesskit-dep-$(ROOTLESSKIT_VERSION).$(ROOTLESSKIT_SUFFIX) . && \
	tar -xzf rootlesskit-dep-$(ROOTLESSKIT_VERSION).$(ROOTLESSKIT_SUFFIX) 
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/rootlesskit.compile:
	@$(call targetinfo)
	@PATH=$(CROSS_PATH):$(HOST_GO_BIN_DIR) ; \
		cd $(ROOTLESSKIT_DIR) &&  \
		$(ROOTLESSKIT_GO_ENV) make
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/rootlesskit.install:
	@$(call targetinfo)
	mkdir -p $(ROOTLESSKIT_INSTALLDIR) && \
	cp $(ROOTLESSKIT_DIR)/bin/rootlesskit $(ROOTLESSKIT_INSTALLDIR)/ && \
	cp $(ROOTLESSKIT_DIR)/bin/rootlesskit-docker-proxy $(ROOTLESSKIT_INSTALLDIR)/
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/rootlesskit.targetinstall:
	@$(call targetinfo)

	@$(call install_init, rootlesskit)
	@$(call install_fixup, rootlesskit,PRIORITY,optional)
	@$(call install_fixup, rootlesskit,SECTION,base)
	@$(call install_fixup, rootlesskit,AUTHOR,"<Marius Warning, Simon Dick>")
	@$(call install_fixup, rootlesskit,DESCRIPTION,missing)

	@$(call install_copy, rootlesskit, 0, 0, 0644, $(ROOTLESSKIT_DIR)/LICENSE, /usr/share/licenses/oss/license.rootlesskit_$(ROOTLESSKIT_VERSION).txt)

	@$(call install_finish, rootlesskit)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/rootlesskit.clean:
	@$(call targetinfo)
	@$(call clean_pkg, ROOTLESSKIT)

# vim: syntax=make
