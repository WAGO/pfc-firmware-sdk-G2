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
PACKAGES-$(PTXCONF_DOCKERD) += dockerd

#
# Paths and names
#
DOCKERD_VERSION		:= 25.0.4
DOCKERD_MD5				:= 43f87e5a65cc258858de78981de661fa
DOCKERD_GITCOMMIT	:= 061aa95809be396a6b5542618d8a34b02a21ff77
DOCKERD						:= moby-$(DOCKERD_VERSION)
DOCKERD_SUFFIX		:= tar.gz
DOCKERD_URL				:= https://github.com/moby/moby/archive/refs/tags/v$(DOCKERD_VERSION).$(DOCKERD_SUFFIX)
DOCKERD_SOURCE		:= $(SRCDIR)/$(DOCKERD).$(DOCKERD_SUFFIX)
DOCKERD_DIR				:= $(BUILDDIR)/$(DOCKERD)
DOCKERD_LICENSE		:= Apache-2.0

DOCKERD_INSTALLDIR := $(DOCKERD_PKGDIR)/bin/docker

DOCKERD_GO_PACKAGE:= github.com/docker/docker/cmd/dockerd

DOCKERD_CONF_TOOL:=NO
DOCKERD_GO_ENV:= \
	AR=$(CROSS_AR) \
	CC=$(CROSS_CC) \
	CXX=$(CROSS_CXX) \
	CGO_CFLAGS=$(CUSTOM_CGO_CFLAGS) \
	CGO_LDFLAGS=$(CROSS_LDFLAGS) \
	PKG_CONFIG=$(CROSS_PKG_CONFIG) \
	GOPATH=$(DOCKERD_DIR) \
	GO111MODULE=off \
	GOOS=linux CGO_ENABLED=1

DOCKERD_BUILDTAGS:=-tags ' \
	netgo \
	osusergo \
	static_build \
	apparmor \
	exclude_graphdriver_btrfs \
	exclude_graphdriver_devicemapper \
	exclude_graphdriver_aufs seccomp journald' \
  -installsuffix netgo
DOCKERD_BUILDMODE:=-buildmode=pie
TIMESTAMP = $(shell date +"%d.%m.%YT%H:%M:%SZ")
DOCKERD_LDFLAGS:=-ldflags '-w \
	-X "github.com/docker/docker/dockerversion.Version=$(DOCKERD_VERSION)" \
	-X "github.com/docker/docker/dockerversion.GitCommit=$(DOCKERD_GITCOMMIT)" \
	-X "github.com/docker/docker/dockerversion.BuildTime=$(TIMESTAMP)" \
	-X "github.com/docker/docker/dockerversion.IAmStatic=true" \
	-X "github.com/docker/docker/dockerversion.PlatformName=g2" \
	-X "github.com/docker/docker/dockerversion.ProductName=pfc" \
	-X "github.com/docker/docker/dockerversion.DefaultProductLicense=$(DOCKERD_LICENSE)" \
	-extldflags "-static"'

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(DOCKERD_SOURCE):
	@$(call targetinfo)
	@$(call get, DOCKERD)


# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

# This step is necessary due to Go's expected file structure. A more elegant solution is yet to be found :(
$(STATEDIR)/dockerd.prepare:
	@$(call targetinfo)
	mkdir -p $(DOCKERD_DIR)/src/github.com/docker/docker && \
	rsync -aq --exclude src $(DOCKERD_DIR)/ $(DOCKERD_DIR)/src/github.com/docker/docker/ && \
	echo $(DOCKERD_LDFLAGS)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/dockerd.compile:
	@$(call targetinfo)
	PATH=$(CROSS_PATH):$(HOST_GO_BIN_DIR) ; \
	cd $(DOCKERD_DIR) &&  \
		$(DOCKERD_GO_ENV) $(GO_ARCH) go build \
			-o bundles/cross/linux/armv7/dockerd-$(DOCKERD_VERSION)-wago \
			$(DOCKERD_BUILDTAGS) \
			$(DOCKERD_BUILDMODE) \
			$(DOCKERD_LDFLAGS) \
			$(DOCKERD_GO_PACKAGE)
	@$(call touch)		

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/dockerd.install:
	@$(call targetinfo)
	mkdir -p $(DOCKERD_INSTALLDIR) && \
	cp $(DOCKERD_DIR)/bundles/cross/linux/armv7/dockerd-$(DOCKERD_VERSION)-wago $(DOCKERD_INSTALLDIR)/ && \
	ln -sf dockerd-$(DOCKERD_VERSION)-wago $(DOCKERD_INSTALLDIR)/dockerd
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/dockerd.targetinstall:
	@$(call targetinfo)

	@$(call install_init, dockerd)
	@$(call install_fixup, dockerd,PRIORITY,optional)
	@$(call install_fixup, dockerd,SECTION,base)
	@$(call install_fixup, dockerd,AUTHOR,"<Marius Warning, Simon Dick>")
	@$(call install_fixup, dockerd,DESCRIPTION,missing)

	@$(call install_copy, dockerd, 0, 0, 0644, $(DOCKERD_DIR)/LICENSE, /usr/share/licenses/oss/license.dockerd_$(DOCKERD_VERSION).txt)

	@$(call install_finish, dockerd)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/dockerd.clean:
	@$(call targetinfo)
	@$(call clean_pkg, DOCKERD)

# vim: syntax=make
