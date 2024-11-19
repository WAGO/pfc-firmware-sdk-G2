# -*-makefile-*-
#
# Copyright (C) 2006-2009 by Robert Schwebel <r.schwebel@pengutronix.de>
#                            Pengutronix <info@pengutronix.de>, Germany
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_GLIB) += glib

#
# Paths and names
#
GLIB_VERSION	:= 2.78.6
GLIB_MD5	:= a8039f0e1a9f29c5c5e4f4176c6d0419
GLIB		:= glib-$(GLIB_VERSION)
GLIB_SUFFIX	:= tar.xz
GLIB_URL	:= http://ftp.gnome.org/pub/GNOME/sources/glib/$(basename $(GLIB_VERSION))/glib-$(GLIB_VERSION).$(GLIB_SUFFIX)
GLIB_SOURCE	:= $(SRCDIR)/$(GLIB).$(GLIB_SUFFIX)
GLIB_DIR	:= $(BUILDDIR)/$(GLIB)

GLIB_LICENSE	:= LGPL-2.1-or-later
GLIB_LICENSE_FILES := \
	file://glib/glib.h;startline=1;endline=18;md5=c97f6829778db537db59d1ce41090b51 \
	file://LICENSES/LGPL-2.1-or-later.txt;md5=41890f71f740302b785c27661123bff5
# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

GLIB_MESON_CROSS_FILE := $(call ptx/get-alternative, config/meson, glib-cross-file.meson)
#
# meson
#
GLIB_CONF_TOOL	:= meson
GLIB_CONF_OPT	:= \
	$(CROSS_MESON_USR) \
	-Dbsymbolic_functions=true \
	-Ddtrace=false \
	-Dforce_posix_threads=true \
	-Dglib_assert=true \
	-Dglib_checks=true \
	-Dglib_debug=enabled \
	-Dgtk_doc=false \
	-Dinstalled_tests=false \
	-Dlibelf=disabled \
	-Dlibmount=$(call ptx/endis, PTXCONF_GLIB_LIBMOUNT)d \
	-Dman=false \
	-Dnls=disabled \
	-Doss_fuzz=disabled \
	-Dselinux=disabled \
	-Dsysprof=disabled \
	-Dsystemtap=false \
	-Dtests=false \
	-Dxattr=false \
	\
	--cross-file $(GLIB_MESON_CROSS_FILE)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/glib.install.post:
	@$(call targetinfo)
	@sed -i 's;^bindir=.*;bindir=$(PTXDIST_SYSROOT_HOST)/bin;' \
		 $(GLIB_PKGDIR)/usr/lib/pkgconfig/gio-2.0.pc
	@$(call world/install.post, GLIB)
	@cd $(PTXDIST_SYSROOT_TARGET) && cp usr/lib/glib-2.0/include/glibconfig.h usr/include/glibconfig.h
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/glib.targetinstall:
	@$(call targetinfo)

	@$(call install_init, glib)
	@$(call install_fixup, glib,PRIORITY,optional)
	@$(call install_fixup, glib,SECTION,base)
	@$(call install_fixup, glib,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, glib,DESCRIPTION,missing)

	@$(call install_copy, glib, 0, 0, 0755, /usr/lib/gio/modules)

	@$(foreach lib, libgio-2.0 libglib-2.0 libgmodule-2.0 libgobject-2.0 libgthread-2.0, \
		$(call install_lib, glib, 0, 0, 0644, $(lib))$(ptx/nl))

ifdef PTXCONF_GLIB_GDBUS
	@$(call install_copy, glib, 0, 0, 0755, -, /usr/bin/gdbus)
endif
	@cat $(GLIB_DIR)/docs/reference/AUTHORS > $(GLIB_DIR)/LICENSE
	@echo -e "\n" >> $(GLIB_DIR)/LICENSE
	@cat $(GLIB_DIR)/docs/reference/COPYING >> $(GLIB_DIR)/LICENSE
	@echo -e "\n" >> $(GLIB_DIR)/LICENSE
	@cat $(GLIB_DIR)/LICENSES/LGPL-2.1-or-later.txt >> $(GLIB_DIR)/LICENSE
	@echo -e "\n" >> $(GLIB_DIR)/LICENSE
	# --- gmodule ---
	@echo -e "\nLicense of gmodule\n" >> $(GLIB_DIR)/LICENSE
	@cat $(GLIB_DIR)/gmodule/AUTHORS >> $(GLIB_DIR)/LICENSE
	@echo -e "\n" >> $(GLIB_DIR)/LICENSE
	@cat $(GLIB_DIR)/LICENSES/LGPL-2.1-or-later.txt >> $(GLIB_DIR)/LICENSE
	@echo -e "\n" >> $(GLIB_DIR)/LICENSE
	# --- gvdb ---
	@echo -e "\nLicense of gvdb\n" >> $(GLIB_DIR)/LICENSE
	@cat $(GLIB_DIR)/subprojects/gvdb/COPYING >> $(GLIB_DIR)/LICENSE
	@$(call install_copy, glib, 0, 0, 0644, $(GLIB_DIR)/LICENSE, /usr/share/licenses/oss/license.glib_$(GLIB_VERSION).txt)

	@$(call install_finish, glib)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/glib.clean:
	@$(call targetinfo)
	@$(call clean_pkg, glib)
	rm -f $(STATEDIR)/glib.*
	rm -rf $(GLIB_DIR)
	rm -Rf $(PTXCONF_SYSROOT_TARGET)/usr/include/glib-2.0

# vim: syntax=make
