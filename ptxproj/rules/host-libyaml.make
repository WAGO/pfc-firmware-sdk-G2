#######################################################################################################################
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2021 WAGO GmbH & Co. KG
#
# Contributors:
#   MaHe: WAGO GmbH & Co. KG
#######################################################################################################################
#
# For further information about the PTXdist project and license conditions
# see the PTXdist README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_LIBYAML) += host-libyaml

#
# Paths and names
#
HOST_LIBYAML_VERSION	:= 0.2.1
HOST_LIBYAML_MD5		:= 7dbd52fb4d30602ef064ffb4aa4cddab

HOST_LIBYAML		:= libyaml-dist-$(HOST_LIBYAML_VERSION)
HOST_LIBYAML_SUFFIX	:= tar.gz
HOST_LIBYAML_URL	:= https://github.com/yaml/libyaml/archive/dist-$(HOST_LIBYAML_VERSION).$(HOST_LIBYAML_SUFFIX)
HOST_LIBYAML_SOURCE	:= $(SRCDIR)/$(HOST_LIBYAML).$(HOST_LIBYAML_SUFFIX)
HOST_LIBYAML_DIR	:= $(HOST_BUILDDIR)/$(HOST_LIBYAML)
HOST_LIBYAML_LICENSE	:= MIT

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
HOST_LIBYAML_CONF_TOOL	:= autoconf
HOST_LIBYAML_CONF_OPT	:= $(HOST_AUTOCONF)

# vim: syntax=make
