# -*-makefile-*-
#
# Copyright (C) 2022 by <Marius Warning, Simon Dick>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_TINI) += tini

#
# Paths and names
#
TINI_VERSION	:= 0.19.0
TINI_MD5			:= 72935bca9232313409a052833068fb1d
TINI_GITCOMMIT:= de40ad007797e0dcd8b7126f27bb87401d224240
TINI					:= tini-$(TINI_VERSION)
TINI_SUFFIX		:= tar.gz
TINI_URL			:= https://github.com/krallin/tini/archive/refs/tags/v$(TINI_VERSION).$(TINI_SUFFIX)
TINI_SOURCE		:= $(SRCDIR)/$(TINI).$(TINI_SUFFIX)
TINI_DIR			:= $(BUILDDIR)/$(TINI)
TINI_LICENSE	:= MIT

TINI_INSTALLDIR := $(TINI_PKGDIR)/bin/docker

TINI_CONF_TOOL	:= cmake
TINI_CONF_OPT		:= $(CROSS_CMAKE_USR) -DGITCOMMIT:STRING=$(TINI_GITCOMMIT)
TINI_MAKE_ENV 	:= $(CROSS_ENV)

TINI_GO_ENV:= \
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

$(TINI_SOURCE):
	@$(call targetinfo)
	@$(call get, TINI)
	
# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------


# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/tini.install:
	@$(call targetinfo)
	mkdir -p $(TINI_INSTALLDIR) && \
	cp $(TINI_DIR)-build/tini-static $(TINI_INSTALLDIR)/docker-init
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/tini.targetinstall:
	@$(call targetinfo)

	@$(call install_init, tini)
	@$(call install_fixup, tini,PRIORITY,optional)
	@$(call install_fixup, tini,SECTION,base)
	@$(call install_fixup, tini,AUTHOR,"<Marius Warning, Simon Dick>")
	@$(call install_fixup, tini,DESCRIPTION,missing)

	@$(call install_copy, tini, 0, 0, 0644, $(TINI_DIR)/LICENSE, /usr/share/licenses/oss/license.tini_$(TINI_VERSION).txt)
	
	@$(call install_finish, tini)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/tini.clean:
	@$(call targetinfo)
	@$(call clean_pkg, TINI)

# vim: syntax=make
