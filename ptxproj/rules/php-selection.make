# -*-makefile-*-
#
# Copyright (C) 2022-2023 by Patrick Enns <patrick.enns@wago.com>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_PHP_SELECTION) += php-selection

#
# Paths and names
#
PHP_SELECTION_VERSION       := 1.2.0
PHP_SELECTION_MD5           := 
PHP_SELECTION               := php-$(PHP_VERSION)
PHP_SELECTION_SUFFIX        := 
PHP_SELECTION_SOURCE        := 
PHP_SELECTION_DIR           := $(BUILDDIR)/$(PHP_SELECTION)
PHP_SELECTION_LICENSE       := MPL-2.0
PHP_SELECTION_URL           := 

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/php-selection.extract:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/php-selection.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/php-selection.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/php-selection.install:
	@$(call targetinfo)
	@rm -f $(PTXDIST_SYSROOT_TARGET)/usr/include/php
	@rm -f $(PTXDIST_SYSROOT_TARGET)/usr/bin/php-config
	@rm -f $(PTXDIST_SYSROOT_TARGET)/usr/bin/phpize

ifdef PTXCONF_PHP_SELECTION_PREFER_PHP7
	@ln -s $(PTXDIST_SYSROOT_TARGET)/usr/include/php7    $(PTXDIST_SYSROOT_TARGET)/usr/include/php
	@ln -s $(PTXDIST_SYSROOT_TARGET)/usr/bin/php7-config $(PTXDIST_SYSROOT_TARGET)/usr/bin/php-config
	@ln -s $(PTXDIST_SYSROOT_TARGET)/usr/bin/phpize7     $(PTXDIST_SYSROOT_TARGET)/usr/bin/phpize
endif
ifdef PTXCONF_PHP_SELECTION_PREFER_PHP8
	@ln -s $(PTXDIST_SYSROOT_TARGET)/usr/include/php8    $(PTXDIST_SYSROOT_TARGET)/usr/include/php
	@ln -s $(PTXDIST_SYSROOT_TARGET)/usr/bin/php8-config $(PTXDIST_SYSROOT_TARGET)/usr/bin/php-config
	@ln -s $(PTXDIST_SYSROOT_TARGET)/usr/bin/phpize8     $(PTXDIST_SYSROOT_TARGET)/usr/bin/phpize
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/php-selection.targetinstall:
	@$(call targetinfo)

	@$(call install_init, php-selection)
	@$(call install_fixup, php-selection,PRIORITY,optional)
	@$(call install_fixup, php-selection,SECTION,base)
	@$(call install_fixup, php-selection,AUTHOR,"Patrick Enns <patrick.enns@wago.com>")
	@$(call install_fixup, php-selection,DESCRIPTION,missing)

ifdef PTXCONF_PHP_SELECTION_PREFER_PHP7
	@$(call install_link, php-selection, php7, /etc/php)
ifdef PTXCONF_PHP7_SAPI_CLI
	@$(call install_link, php-selection, php7, /usr/bin/php)
endif
ifdef PTXCONF_PHP7_SAPI_CGI
	@$(call install_link, php-selection, php7-cgi, /usr/bin/php-cgi)
endif
ifdef PTXCONF_PHP7_SAPI_FPM
	@$(call install_link, php-selection, php7-fpm, /usr/bin/php-fpm)
endif
ifdef PTXCONF_PHP7_EXT_OPCACHE
	@$(call install_link, php-selection, ../php7/opcache.so, /usr/lib/php/opcache.so)
endif
endif

ifdef PTXCONF_PHP_SELECTION_PREFER_PHP8
	@$(call install_link, php-selection, php8, /etc/php)
ifdef PTXCONF_PHP8_SAPI_CLI
	@$(call install_link, php-selection, php8, /usr/bin/php)
endif
ifdef PTXCONF_PHP8_SAPI_CGI
	@$(call install_link, php-selection, php8-cgi, /usr/bin/php-cgi)
endif
ifdef PTXCONF_PHP8_SAPI_FPM
	@$(call install_link, php-selection, php8-fpm, /usr/bin/php-fpm)
endif
ifdef PTXCONF_PHP8_EXT_OPCACHE
	@$(call install_link, php-selection, ../php8/opcache.so, /usr/lib/php/opcache.so)
endif
endif

	@$(call install_finish, php-selection)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/php-selection.clean:
	@$(call targetinfo)
	@rm -f $(PTXDIST_SYSROOT_TARGET)/usr/include/php
	@rm -f $(PTXDIST_SYSROOT_TARGET)/usr/bin/php-config
	@rm -f $(PTXDIST_SYSROOT_TARGET)/usr/bin/phpize
	@$(call clean_pkg, PHP_SELECTION)

# vim: syntax=make
