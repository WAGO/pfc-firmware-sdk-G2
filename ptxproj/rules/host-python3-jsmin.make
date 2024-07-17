#######################################################################################################################
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2024 WAGO GmbH & Co. KG
#
# Contributors:
#   AGa: WAGO GmbH & Co. KG
#######################################################################################################################
#
# For further information about the PTXdist project and license conditions
# see the PTXdist README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_PYTHON3_JSMIN) += host-python3-jsmin

#
# Paths and names
#
HOST_PYTHON3_JSMIN_VERSION	:= 3.0.1
HOST_PYTHON3_JSMIN_MD5	:= ef46d32b3634bb049d501e6a1be2e010
HOST_PYTHON3_JSMIN		:= python3-jsmin-$(HOST_PYTHON3_JSMIN_VERSION)
HOST_PYTHON3_JSMIN_SUFFIX	:= tgz
HOST_PYTHON3_JSMIN_URL	:= https://github.com/tikitu/jsmin/archive/release-$(HOST_PYTHON3_JSMIN_VERSION).$(HOST_PYTHON3_JSMIN_SUFFIX)
HOST_PYTHON3_JSMIN_SOURCE	:= $(SRCDIR)/$(HOST_PYTHON3_JSMIN).$(HOST_PYTHON3_JSMIN_SUFFIX)
HOST_PYTHON3_JSMIN_DIR	:= $(HOST_BUILDDIR)/$(HOST_PYTHON3_JSMIN)
HOST_PYTHON3_JSMIN_LICENSE	:= MIT

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

HOST_PYTHON3_JSMIN_CONF_TOOL	:= python3

# vim: syntax=make
