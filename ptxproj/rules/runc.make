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
PACKAGES-$(PTXCONF_RUNC) += runc

#
# Paths and names
#
RUNC_VERSION	:= 1.1.12
RUNC_MD5			:= e77fb3f6d3070da60f58688bde3e7583
RUNC_GITCOMMIT:= 51d5e94601ceffbbd85688df1c928ecccbfa4685
RUNC					:= runc-$(RUNC_VERSION)
RUNC_SUFFIX		:= tar.gz
RUNC_URL			:= https://github.com/opencontainers/runc/archive/refs/tags/v$(RUNC_VERSION).$(RUNC_SUFFIX)
RUNC_SOURCE		:= $(SRCDIR)/$(RUNC).$(RUNC_SUFFIX)
RUNC_DIR			:= $(BUILDDIR)/$(RUNC)
RUNC_LICENSE	:= Apache-2.0

RUNC_INSTALLDIR := $(RUNC_PKGDIR)/bin/docker

RUNC_CONF_TOOL:=NO
RUNC_GO_ENV:= \
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

$(RUNC_SOURCE):
	@$(call targetinfo)
	@$(call get, RUNC)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/runc.compile:
	@$(call targetinfo)
	@PATH=$(CROSS_PATH):$(HOST_GO_BIN_DIR) ; \
		cd $(RUNC_DIR) &&  \
		$(RUNC_GO_ENV) $(GO_ARCH) $(MAKE) target=static COMMIT=$(RUNC_GITCOMMIT)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/runc.install:
	@$(call targetinfo)
	mkdir -p $(RUNC_INSTALLDIR) && \
	cp $(RUNC_DIR)/runc $(RUNC_INSTALLDIR)/
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/runc.targetinstall:
	@$(call targetinfo)

	@$(call install_init, runc)
	@$(call install_fixup, runc,PRIORITY,optional)
	@$(call install_fixup, runc,SECTION,base)
	@$(call install_fixup, runc,AUTHOR,"<Marius Warning, Simon Dick>")
	@$(call install_fixup, runc,DESCRIPTION,missing)

	@$(call install_copy, runc, 0, 0, 0644, $(RUNC_DIR)/LICENSE, /usr/share/licenses/oss/license.runc_$(RUNC_VERSION).txt)
	
	@$(call install_finish, runc)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/runc.clean:
	@$(call targetinfo)
	@$(call clean_pkg, RUNC)

# vim: syntax=make
