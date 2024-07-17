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
PACKAGES-$(PTXCONF_DOCKER_CLI) += docker-cli

#
# Paths and names
#
DOCKER_CLI_VERSION	:= 25.0.4
DOCKER_CLI_MD5			:= 5eec531f6d6f00aed2e9cb0d80a62b6b
DOCKER_CLI_GITCOMMIT:= 1a576c50a9a33dd7ab2bcd78db1982cb965812b0
DOCKER_CLI					:= cli-$(DOCKER_CLI_VERSION)
DOCKER_CLI_SUFFIX		:= tar.gz
DOCKER_CLI_URL			:= https://github.com/docker/cli/archive/refs/tags/v$(DOCKER_CLI_VERSION).$(DOCKER_CLI_SUFFIX)
DOCKER_CLI_SOURCE		:= $(SRCDIR)/$(DOCKER_CLI).$(DOCKER_CLI_SUFFIX)
DOCKER_CLI_DIR			:= $(BUILDDIR)/$(DOCKER_CLI)
DOCKER_CLI_LICENSE	:= Apache-2.0

DOCKER_CLI_DEVPKG	:= NO

DOCKER_CLI_INSTALLDIR := $(DOCKER_CLI_PKGDIR)/bin/docker

DOCKER_CLI_GO_PACKAGE := github.com/docker/cli/cmd/docker

DOCKER_CLI_BUILDMODE := -buildmode=pie
DOCKER_CLI_CONF_TOOL	:= NO

DOCKER_CLI_ENV:= \
	AR=$(CROSS_AR) \
	CC=$(CROSS_CC) \
	CXX=$(CROSS_CXX) \
	CGO_CFLAGS=$(CUSTOM_CGO_CFLAGS) \
	CGO_LDFLAGS=$(CROSS_LDFLAGS) \
	PKG_CONFIG=$(CROSS_PKG_CONFIG) \
	GOPATH=$(DOCKER_CLI_DIR) \
	GO111MODULE=auto \
	GOOS=linux CGO_ENABLED=1 

TIMESTAMP = $(shell date +"%d.%m.%YT%H:%M:%SZ")
DOCKER_CLI_BUILD_TAGS:= \
	-tags \
	--ldflags	--ldflags ' -w \
		-X "github.com/docker/cli/cli/version.GitCommit=$(DOCKER_CLI_GITCOMMIT)" \
		-X "github.com/docker/cli/cli/version.Version=$(DOCKER_CLI_VERSION)" \
		-X "github.com/docker/cli/cli/version.BuildTime=$(TIMESTAMP)"'

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(DOCKER_CLI_SOURCE):
	@$(call targetinfo)
	@$(call get, DOCKER_CLI)


# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

# This step is necessary due to Go's expected file structure. A more elegant solution is yet to be found :(
$(STATEDIR)/docker-cli.prepare:
	@$(call targetinfo)
	mkdir -p $(DOCKER_CLI_DIR)/src/github.com/docker/cli && \
	rsync -aq --exclude src $(DOCKER_CLI_DIR)/ $(DOCKER_CLI_DIR)/src/github.com/docker/cli/
	@$(call touch)

	
# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/docker-cli.compile:
	@$(call targetinfo)
		@PATH=$(CROSS_PATH):$(HOST_GO_BIN_DIR) ; \
		cd $(DOCKER_CLI_DIR) &&  \
		$(DOCKER_CLI_ENV) $(GO_ARCH) go build \
			$(DOCKER_CLI_BUILDMODE) \
			-o build/docker-linux-arm-v7 \
			$(DOCKER_CLI_BUILD_TAGS) \
			$(DOCKER_CLI_GO_PACKAGE)
			
	@$(call touch)


# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/docker-cli.install:
	@$(call targetinfo)
	mkdir -p $(DOCKER_CLI_INSTALLDIR) && \
	cp $(DOCKER_CLI_DIR)/build/docker-linux-arm-v7 $(DOCKER_CLI_INSTALLDIR)/ && \
	ln -s docker-linux-arm-v7 $(DOCKER_CLI_INSTALLDIR)/docker
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/docker-cli.targetinstall:
	@$(call targetinfo)

	@$(call install_init, docker-cli)
	@$(call install_fixup, docker-cli,PRIORITY,optional)
	@$(call install_fixup, docker-cli,SECTION,base)
	@$(call install_fixup, docker-cli,AUTHOR,"<Marius Warning, Simon Dick>")
	@$(call install_fixup, docker-cli,DESCRIPTION,missing)
	
	@$(call install_copy, docker-cli, 0, 0, 0644, $(DOCKER_CLI_DIR)/LICENSE, /usr/share/licenses/oss/license.docker-cli_$(DOCKER_CLI_VERSION).txt)

	@$(call install_finish, docker-cli)
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/docker-cli.clean:
	@$(call targetinfo)
	@$(call clean_pkg, DOCKER_CLI)

# vim: syntax=make
