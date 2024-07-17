#######################################################################################################################
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of project parameter-service (PTXdist wdaphpdemo).
#
# Copyright (c) 2023 WAGO GmbH & Co. KG
#
# Contributors:
#   PEn: WAGO GmbH & Co. KG
#######################################################################################################################
# Makefile for PTXdist package wdaphpdemo.
#
# For further information about the PTXdist project and license conditions
# see the PTXdist README file.
#

#
# We provide this package
#
LAZY_PACKAGES-$(PTXCONF_WDAPHPDEMO) +=wdaphpdemo

#
# Paths and names
#
WDAPHPDEMO_VERSION  := 1.0.0
WDAPHPDEMO_MD5      :=
WDAPHPDEMO          := wdaphpdemo
WDAPHPDEMO_LICENSE  := MIT

WDAPHPDEMO_PACKAGE_NAME := $(WDAPHPDEMO)_$(WDAPHPDEMO_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
WDAPHPDEMO_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages
WDAPHPDEMO_PACKAGE_DIR := $(PTXDIST_TEMPDIR)/package/$(WDAPHPDEMO_PACKAGE_NAME)

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/wdaphpdemo.get:
	@$(call targetinfo, $@)
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/wdaphpdemo.extract:
	@$(call targetinfo, $@)
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wdaphpdemo.prepare:
	@$(call targetinfo, $@)
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wdaphpdemo.compile:
	@$(call targetinfo, $@)
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wdaphpdemo.install:
	@$(call targetinfo, $@)
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wdaphpdemo.targetinstall:
	@$(call targetinfo, $@)

	@$(call install_init, wdaphpdemo)

	@$(call install_fixup,wdaphpdemo,PRIORITY,optional)
	@$(call install_fixup,wdaphpdemo,VERSION,$(WDAPHPDEMO_VERSION))	
	@$(call install_fixup,wdaphpdemo,SECTION,base)
	@$(call install_fixup,wdaphpdemo,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup,wdaphpdemo,DESCRIPTION,missing)

	@$(call install_copy,        wdaphpdemo, 0, 0, 0755, /var/www/demo-wda)
	@$(call install_alternative, wdaphpdemo, 0, 0, 0644, /var/www/demo-wda/index.php)
	@$(call install_alternative, wdaphpdemo, 0, 0, 0644, /var/www/demo-wda/style.css)
	@$(call install_alternative, wdaphpdemo, 0, 0, 0644, /var/www/demo-wda/wago-logo.svg)

	@$(call install_finish,wdaphpdemo)
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wdaphpdemo.clean:
	@$(call targetinfo)
	@$(call clean_pkg, WDAPHPDEMO)


# vim: syntax=make
