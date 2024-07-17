# -*-makefile-*-
#
# Copyright (C) 2022 by <Falk Werner>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_GO) += host-go

#
# Paths and names
#
HOST_GO_VERSION	:= 1.21.8
HOST_GO_MD5	:= a3e6cd4ed28d6b25572d30bc011d5ef1
HOST_GO		:= go$(HOST_GO_VERSION)
HOST_GO_PLATFORM	:= linux-amd64
HOST_GO_SUFFIX	:= tar.gz
HOST_GO_URL	:= https://go.dev/dl/go$(HOST_GO_VERSION).$(HOST_GO_PLATFORM).$(HOST_GO_SUFFIX)
HOST_GO_SOURCE	:= $(SRCDIR)/$(HOST_GO).$(HOST_GO_PLATFORM).$(HOST_GO_SUFFIX)
HOST_GO_DIR	:= $(HOST_BUILDDIR)/$(HOST_GO)
HOST_GO_INSTALL_DIR := $(PTXDIST_SYSROOT_HOST)/usr/local/go
HOST_GO_BIN_DIR := $(HOST_GO_INSTALL_DIR)/bin

ifeq ($(PTXCONF_ARCH_STRING), arm64)
	GO_ARCH:=GOARCH=arm64
else 
	GO_ARCH:=GOARCH=arm GOARM=7
endif

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(HOST_GO_SOURCE):
	@$(call targetinfo)
	@$(call get, HOST_GO)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/host-go.extract:
	@$(call targetinfo)
	@$(call clean, $(HOST_GO_DIR))
	@$(call extract, HOST_GO)
	@$(call patchin, HOST_GO)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/host-go.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/host-go.install:
	@$(call targetinfo)
	@mkdir -p $(HOST_GO_INSTALL_DIR)
	@cp -rf $(HOST_GO_DIR)/* $(HOST_GO_INSTALL_DIR)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/host-go.clean:
	@$(call targetinfo)
	@$(call clean_pkg, HOST_GO)
	@rm -rf $(HOST_GO_INSTALL_DIR)

# vim: syntax=make
