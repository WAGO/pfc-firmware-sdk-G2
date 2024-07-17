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
PACKAGES-$(PTXCONF_DOCKER_USERLAND_PROXY) += docker-userland-proxy

#
# Paths and names
#
DOCKER_USERLAND_PROXY_VERSION		:= 0dde5c895075df6e3630e76f750a447cf63f4789
DOCKER_USERLAND_PROXY_MD5				:= 802fcc5a4230cd98a88b9717510787b1
DOCKER_USERLAND_PROXY_GITCOMMIT	:= $(DOCKER_USERLAND_PROXY_VERSION)
DOCKER_USERLAND_PROXY						:= libnetwork-$(DOCKER_USERLAND_PROXY_VERSION)
DOCKER_USERLAND_PROXY_SUFFIX		:= tar.gz
DOCKER_USERLAND_PROXY_URL				:= https://github.com/moby/libnetwork/archive/$(DOCKER_USERLAND_PROXY_GITCOMMIT).$(DOCKER_USERLAND_PROXY_SUFFIX)
DOCKER_USERLAND_PROXY_SOURCE		:= $(SRCDIR)/$(DOCKER_USERLAND_PROXY).$(DOCKER_USERLAND_PROXY_SUFFIX)
DOCKER_USERLAND_PROXY_DIR				:= $(BUILDDIR)/$(DOCKER_USERLAND_PROXY)
DOCKER_USERLAND_PROXY_LICENSE		:= Apache-2.0

DOCKER_USERLAND_PROXY_INSTALLDIR := $(DOCKER_USERLAND_PROXY_PKGDIR)/bin/docker

DOCKER_USERLAND_PROXY_CONF_TOOL:=NO
DOCKER_USERLAND_PROXY_GO_ENV:= \
	AR=$(CROSS_AR) \
	CC=$(CROSS_CC) \
	CXX=$(CROSS_CXX) \
	CGO_CFLAGS=$(CROSS_CFLAGS) \
	CGO_LDFLAGS=$(CROSS_LDFLAGS) \
	PKG_CONFIG=$(CROSS_PKG_CONFIG) \
	GOPATH=$(DOCKER_USERLAND_PROXY_DIR) \
	GO111MODULE=auto \
	GOOS=linux CGO_ENABLED=0

DOCKER_USERLAND_PROXY_GO_PACKAGE := github.com/libnetwork/cmd/proxy

DOCKER_USERLAND_PROXY_BUILDMODE := -buildmode=pie
DOCKER_USERLAND_PROXY_LDFLAGS := -ldflags='-extldflags "-static"'

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(DOCKER_USERLAND_PROXY_SOURCE):
	@$(call targetinfo)
	@$(call get, DOCKER_USERLAND_PROXY)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/docker-userland-proxy.prepare:
	@$(call targetinfo)
	mkdir -p $(DOCKER_USERLAND_PROXY_DIR)/src/github.com/libnetwork && \
	rsync -aq --exclude src $(DOCKER_USERLAND_PROXY_DIR)/ $(DOCKER_USERLAND_PROXY_DIR)/src/github.com/libnetwork/
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/docker-userland-proxy.compile:
	@$(call targetinfo)
	@PATH=$(CROSS_PATH):$(HOST_GO_BIN_DIR) ; \
		cd $(DOCKER_USERLAND_PROXY_DIR) &&  \
			$(DOCKER_USERLAND_PROXY_GO_ENV) $(GO_ARCH) go build \
				-o docker-proxy \
				$(DOCKER_USERLAND_PROXY_BUILDMODE) \
				$(DOCKER_USERLAND_PROXY_LDFLAGS) \
				$(DOCKER_USERLAND_PROXY_GO_PACKAGE)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/docker-userland-proxy.install:
	@$(call targetinfo)
	mkdir -p $(DOCKER_USERLAND_PROXY_INSTALLDIR) && \
	cp $(DOCKER_USERLAND_PROXY_DIR)/docker-proxy $(DOCKER_USERLAND_PROXY_INSTALLDIR)/
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/docker-userland-proxy.targetinstall:
	@$(call targetinfo)

	@$(call install_init, docker-userland-proxy)
	@$(call install_fixup, docker-userland-proxy,PRIORITY,optional)
	@$(call install_fixup, docker-userland-proxy,SECTION,base)
	@$(call install_fixup, docker-userland-proxy,AUTHOR,"<Marius Warning, Simon Dick>")
	@$(call install_fixup, docker-userland-proxy,DESCRIPTION,missing)

	@$(call install_copy, docker-userland-proxy, 0, 0, 0644, $(DOCKER_USERLAND_PROXY_DIR)/LICENSE, /usr/share/licenses/oss/license.docker-userland-proxy_$(DOCKER_USERLAND_PROXY_VERSION).txt)
	
	@$(call install_finish, docker-userland-proxy)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/docker-userland-proxy.clean:
	@$(call targetinfo)
	@$(call clean_pkg, DOCKER_USERLAND_PROXY)

# vim: syntax=make
