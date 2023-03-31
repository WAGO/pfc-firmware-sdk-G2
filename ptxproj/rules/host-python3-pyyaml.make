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
HOST_PACKAGES-$(PTXCONF_HOST_PYTHON3_PYYAML) += host-python3-pyyaml

#
# Paths and names
#
HOST_PYTHON3_PYYAML_VERSION	:= 3.12
HOST_PYTHON3_PYYAML_MD5	:= 5c41a91fae3da4f5302e25e5d8f4deeb
HOST_PYTHON3_PYYAML		:= pyyaml-$(HOST_PYTHON3_PYYAML_VERSION)
HOST_PYTHON3_PYYAML_SUFFIX	:= tar.gz
HOST_PYTHON3_PYYAML_URL	:= https://github.com/yaml/pyyaml/archive/$(HOST_PYTHON3_PYYAML_VERSION).$(HOST_PYTHON3_PYYAML_SUFFIX)
HOST_PYTHON3_PYYAML_SOURCE	:= $(SRCDIR)/$(HOST_PYTHON3_PYYAML).$(HOST_PYTHON3_PYYAML_SUFFIX)
HOST_PYTHON3_PYYAML_DIR	:= $(HOST_BUILDDIR)/$(HOST_PYTHON3_PYYAML)
HOST_PYTHON3_PYYAML_LICENSE	:= MIT

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

HOST_PYTHON3_PYYAML_CONF_TOOL	:= python3

# vim: syntax=make
