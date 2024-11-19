# -*-makefile-*-
#
# Copyright (C) 2017 by Clemens Gruber <clemens.gruber@pqgruber.com>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBGPIOD) += libgpiod

#
# Paths and names
#
LIBGPIOD_VERSION	:= 2.1
LIBGPIOD_MD5		:= 5477148db09290053d43322d32d032da
LIBGPIOD		:= libgpiod-$(LIBGPIOD_VERSION)
LIBGPIOD_SUFFIX		:= tar.gz
LIBGPIOD_URL		:= https://www.kernel.org/pub/software/libs/libgpiod/$(LIBGPIOD).$(LIBGPIOD_SUFFIX)
LIBGPIOD_SOURCE		:= $(SRCDIR)/$(LIBGPIOD).$(LIBGPIOD_SUFFIX)
LIBGPIOD_DIR		:= $(BUILDDIR)/$(LIBGPIOD)
LIBGPIOD_LICENSE	:= LGPL-2.1-only
LIBGPIOD_LICENSE_FILES	:= file://COPYING;md5=a2e97638167a74cd20bfa75831bf6075

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libgpiod.extract: $(STATEDIR)/autogen-tools $(STATEDIR)/host-autotools-autoconf-archive.install.post
	@$(call targetinfo)
	@$(call clean, $(LIBGPIOD_DIR))
	@$(call extract, LIBGPIOD)
	@$(call patchin, LIBGPIOD)
	cd $(LIBGPIOD_DIR) && [ -f configure ] || (autoreconf -i -f && sh autogen.sh)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIBGPIOD_CONF_TOOL	:= autoconf
LIBGPIOD_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--$(call ptx/endis, PTXCONF_LIBGPIOD_TOOLS)-tools \
	--disable-tests \
	--$(call ptx/endis, PTXCONF_LIBGPIOD_CXX)-bindings-cxx \
	--$(call ptx/endis, PTXCONF_LIBGPIOD_PYTHON3)-bindings-python

LIBGPIOD_CONF_ENV := \
	$(CROSS_ENV) \
	$(if $(PTXCONF_PYTHON3), ac_cv_path_PYTHON=$(CROSS_PYTHON3))

# libgpiod requires kernel headers >= 5.10
ifdef PTXCONF_KERNEL_HEADER
LIBGPIOD_CPPFLAGS	:= \
	-isystem $(KERNEL_HEADERS_INCLUDE_DIR)
endif

LIBGPIOD_TOOLS-$(PTXCONF_LIBGPIOD_GPIODETECT)	+= gpiodetect
LIBGPIOD_TOOLS-$(PTXCONF_LIBGPIOD_GPIOINFO)		+= gpioinfo
LIBGPIOD_TOOLS-$(PTXCONF_LIBGPIOD_GPIOGET)		+= gpioget
LIBGPIOD_TOOLS-$(PTXCONF_LIBGPIOD_GPIOSET)		+= gpioset
LIBGPIOD_TOOLS-$(PTXCONF_LIBGPIOD_GPIONOTIFY)	+= gpionotify
LIBGPIOD_TOOLS-$(PTXCONF_LIBGPIOD_GPIOMON)		+= gpiomon

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libgpiod.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libgpiod)
	@$(call install_fixup, libgpiod, PRIORITY, optional)
	@$(call install_fixup, libgpiod, SECTION, base)
	@$(call install_fixup, libgpiod, AUTHOR, "Clemens Gruber <clemens.gruber@pqgruber.com>")
	@$(call install_fixup, libgpiod, DESCRIPTION, "Linux GPIO character device library")

	@$(call install_lib, libgpiod, 0, 0, 0644, libgpiod)

	@for tool in $(LIBGPIOD_TOOLS-y); do \
		$(call install_copy, libgpiod, 0, 0, 0755, -, \
			/usr/bin/$$tool); \
	done

ifdef PTXCONF_LIBGPIOD_CXX
	@$(call install_lib, libgpiod, 0, 0, 0644, libgpiodcxx)
endif
ifdef PTXCONF_LIBGPIOD_PYTHON3
	@$(call install_glob, libgpiod, 0, 0, -, $(PYTHON3_SITEPACKAGES),, gpiod.*)
endif

	@$(call install_copy, libgpiod, 0, 0, 0644, $(LIBGPIOD_DIR)/COPYING, /usr/share/licenses/oss/license.libgpiod_$(LIBGPIOD_VERSION).txt)
	@$(call install_finish, libgpiod)

	@$(call touch)

# vim: syntax=make
