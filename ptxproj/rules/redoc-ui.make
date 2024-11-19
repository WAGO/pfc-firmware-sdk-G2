# -*-makefile-*-
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_REDOC_UI) += redoc-ui

#
# Paths and names
#
REDOC_UI_VERSION       := 2.1.5
REDOC_UI_VERSION_PTX   := $(shell echo -n $(REDOC_UI_VERSION) | sed s/-/~/g)
REDOC_UI_MD5           := d81d2af1dc41d79a81f4e3761ba5e8d5
REDOC_UI               := redoc-ui-$(REDOC_UI_VERSION)
REDOC_UI_SUFFIX        := tgz
REDOC_UI_URL           := https://registry.npmjs.org/redoc/-/redoc-$(REDOC_UI_VERSION).$(REDOC_UI_SUFFIX)
REDOC_UI_SOURCE        := $(SRCDIR)/$(REDOC_UI).$(REDOC_UI_SUFFIX)
REDOC_UI_DIR           := $(BUILDDIR)/$(REDOC_UI)
REDOC_UI_OPENAPI_DIR   := var/www/openapi
REDOC_UI_LICENSE_DIR   := usr/share/licenses/oss

REDOC_UI_LICENSE       := MIT
REDOC_UI_LICENSE_FILE  := LICENSE
REDOC_UI_LICENSE_FILES := file://$(REDOC_UI_LICENSE_FILE);md5=cf2d48dc6713f3b9cfdb48d5c9c7edc5

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# use ptxdist default

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/redoc-ui.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/redoc-ui.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/redoc-ui.install:
	@$(call targetinfo)

	# install js library
	mkdir -p $(REDOC_UI_PKGDIR)/$(REDOC_UI_OPENAPI_DIR) && \
	 cp $(REDOC_UI_DIR)/bundles/redoc.standalone.js $(REDOC_UI_PKGDIR)/$(REDOC_UI_OPENAPI_DIR)/redoc.standalone.js

	# install license files as combined file
	mkdir -p $(REDOC_UI_PKGDIR)/$(REDOC_UI_LICENSE_DIR) && \
	 cat $(REDOC_UI_DIR)/$(REDOC_UI_LICENSE_FILE) $(REDOC_UI_DIR)/bundles/redoc.standalone.js.LICENSE.txt > $(REDOC_UI_PKGDIR)/$(REDOC_UI_LICENSE_DIR)/license.redoc-ui_$(REDOC_UI_VERSION_PTX).txt

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/redoc-ui.targetinstall:
	@$(call targetinfo)
	@$(call install_init, redoc-ui)
	@$(call install_fixup, redoc-ui,PRIORITY,optional)
	@$(call install_fixup, redoc-ui,SECTION,base)
	@$(call install_fixup, redoc-ui,AUTHOR,"Marius Hellmeier, WAGO GmbH \& Co. KG")
	@$(call install_fixup, redoc-ui,DESCRIPTION,missing)

	# create target directory itself
	@$(call install_copy, redoc-ui, 0, 0, 0755, /$(REDOC_UI_OPENAPI_DIR))

	# install redoc-ui lib files
	@$(call install_copy, redoc-ui, 0, 0, 0644, -, /$(REDOC_UI_OPENAPI_DIR)/redoc.standalone.js)
	@$(call install_copy, redoc-ui, 0, 0, 0644, -, /$(REDOC_UI_LICENSE_DIR)/license.redoc-ui_$(REDOC_UI_VERSION_PTX).txt)

	# Install webserver config
	@$(call install_alternative, redoc-ui, 0, 0, 0600, \
		/etc/lighttpd/apps.confd/openapi.conf)

	@$(call install_finish, redoc-ui)
	@$(call touch)

# vim: syntax=make
