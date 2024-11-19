# -*-makefile-*-
#
# Copyright (C) 2007 by Daniel Schnell
#		2008, 2009 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIGHTTPD) += lighttpd

#
# WAGO: -added self-signed certificate generation
#       -added installation routine for a default lighttpd-htpasswd.user and its backup
#

#
# Paths and names
#
LIGHTTPD_BASE_VERSION  := 1.4.76
LIGHTTPD_WAGO_VERSION  := wago1
LIGHTTPD_VERSION       := $(LIGHTTPD_BASE_VERSION)+$(LIGHTTPD_WAGO_VERSION)
LIGHTTPD_ARCHIVE_NAME  := lighttpd-$(LIGHTTPD_BASE_VERSION)
LIGHTTPD               := lighttpd-$(LIGHTTPD_VERSION)
LIGHTTPD_SUFFIX        := tar.xz
LIGHTTPD_URL           := https://download.lighttpd.net/lighttpd/releases-1.4.x/$(LIGHTTPD_ARCHIVE_NAME).$(LIGHTTPD_SUFFIX)
LIGHTTPD_SOURCE        := $(SRCDIR)/$(LIGHTTPD_ARCHIVE_NAME).$(LIGHTTPD_SUFFIX)
LIGHTTPD_MD5           := f9018cda389b1aa6dae4c5f962c20825
LIGHTTPD_DIR           := $(BUILDDIR)/$(LIGHTTPD)
LIGHTTPD_LICENSE       := BSD-3-Clause AND BSD-2-Clause AND OML AND RSA-MD AND Apache-2.0
LIGHTTPD_LICENSE_FILES := \
	file://COPYING;md5=e4dac5c6ab169aa212feb5028853a579

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
LIGHTTPD_CONF_TOOL	:= meson
LIGHTTPD_CONF_OPT	:= \
	$(CROSS_MESON_USR) \
	-D build_extra_warnings=true \
	-D with_libev=disabled \
	-D with_mysql=disabled \
	-D with_pgsql=disabled \
	-D with_dbi=disabled \
	-D with_sasl=disabled \
	-D with_ldap=disabled \
	-D with_pam=disabled \
	-D with_xattr=$(call ptx/truefalse, PTXCONF_LIGHTTPD_ATTR) \
	-D with_libunwind=disabled \
	-D with_krb5=disabled \
	-D with_openssl=$(call ptx/truefalse,PTXCONF_LIGHTTPD_OPENSSL) \
	-D with_wolfssl=false \
	-D with_pcre=$(call ptx/ifdef,PTXCONF_LIGHTTPD_PCRE,pcre,disabled) \
	-D with_pcre2=false \
	-D with_zlib=$(call ptx/endis,PTXCONF_LIGHTTPD_ZLIB)d \
	-D with_bzip=$(call ptx/endis,PTXCONF_LIGHTTPD_BZ2LIB)d \
	-D with_fam=disabled \
	-D with_webdav_props=$(call ptx/endis, PTXCONF_LIGHTTPD_WEBDAV_PROPS)d \
	-D with_webdav_locks=$(call ptx/endis, PTXCONF_LIGHTTPD_WEBDAV_LOCKS)d \
	-D with_maxminddb=disabled \
  -D with_lua=$(call ptx/truefalse,PTXCONF_LIGHTTPD_LUA) \
  -D with_brotli=disabled \
	-D with_zstd=disabled

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
$(STATEDIR)/lighttpd.compile:
	@$(call targetinfo)
	@$(call world/compile, LIGHTTPD)

ifdef PTXCONF_LIGHTTPD_HTTPS_GEN_CERT
# Generate a long-lasting self-signed certificate for testing
# openssl suffers from y2038 bug => use number of days until the "doomsday"
# to move the expiration date as far as possible into the future
	@mkdir -p $(LIGHTTPD_DIR) && \
	export PASSPHRASE=$$(head -c 128 /dev/urandom | uuencode - | grep -v '^end' | tr '\n' 'd') && \
DAYS_VALID=$$(echo "2038 01 18" | awk '{dt=mktime($$0 " 00 00 00")-systime(); print int(dt/86400);}') && \
echo $$DAYS_VALID && echo "" | awk '{print systime();}' && \
SUBJ=/C='DE'/ST='NRW'/L='Minden'/O='WAGO GmbH & Co. KG'/OU='AUTOMATION'/CN='Self-signed certificate for testing' && \
	openssl genrsa -des3 -out $(LIGHTTPD_DIR)/server.key -passout env:PASSPHRASE 3072 && \
	cp $(LIGHTTPD_DIR)/server.key $(LIGHTTPD_DIR)/server.key.org && \
	openssl rsa -in $(LIGHTTPD_DIR)/server.key.org -out $(LIGHTTPD_DIR)/server.key -passin env:PASSPHRASE && \
	openssl req -new -x509 -subj "$${SUBJ}" -days $${DAYS_VALID} -extensions v3_req -key $(LIGHTTPD_DIR)/server.key -out $(LIGHTTPD_DIR)/server.crt -sha256 && \
	cat $(LIGHTTPD_DIR)/server.crt $(LIGHTTPD_DIR)/server.key > $(LIGHTTPD_DIR)/https-cert.pem && echo $${DAYS_VALID} && \
	openssl x509 -text -noout -in $(LIGHTTPD_DIR)/https-cert.pem && \
	openssl dhparam -out $(LIGHTTPD_DIR)/dh3072.pem -outform PEM -2 3072
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------
#$(STATEDIR)/lighttpd.install:
#	@$(call targetinfo)
#	@$(call world/install, LIGHTTPD)
#	@install -vD -m 0644 "$(LIGHTTPD_DIR)/doc/config/conf.d/mime.conf" \
#		"$(LIGHTTPD_PKGDIR)/etc/lighttpd/conf.d/mime.conf"
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

LIGHTTPD_MODULES-y :=
LIGHTTPD_MODULES-$(PTXCONF_LIGHTTPD_MOD_ACCESS)		+= mod_access
LIGHTTPD_MODULES-$(PTXCONF_LIGHTTPD_MOD_ACCESSLOG)	+= mod_accesslog
LIGHTTPD_MODULES-$(PTXCONF_LIGHTTPD_MOD_ALIAS)		+= mod_alias
LIGHTTPD_MODULES-$(PTXCONF_LIGHTTPD_MOD_AUTH)		+= mod_auth
LIGHTTPD_MODULES-$(PTXCONF_LIGHTTPD_MOD_AUTH)		+= mod_authn_file
LIGHTTPD_MODULES-$(PTXCONF_LIGHTTPD_MOD_COMPRESS)	+= mod_compress
LIGHTTPD_MODULES-$(PTXCONF_LIGHTTPD_MOD_FASTCGI)	+= mod_fastcgi
LIGHTTPD_MODULES-$(PTXCONF_LIGHTTPD_MOD_MAGNET)		+= mod_magnet
LIGHTTPD_MODULES-$(PTXCONF_LIGHTTPD_OPENSSL)		+= mod_openssl
LIGHTTPD_MODULES-$(PTXCONF_LIGHTTPD_MOD_REWRITE)	+= mod_rewrite
LIGHTTPD_MODULES-$(PTXCONF_LIGHTTPD_MOD_WEBDAV)		+= mod_webdav
LIGHTTPD_MODULES-y += $(call remove_quotes,$(PTXCONF_LIGHTTPD_MOD_EXTRA))

LIGHTTPD_MODULE_STRING := $(subst $(space),$(comma),$(addsuffix \",$(addprefix \",$(LIGHTTPD_MODULES-y))))

# add modules that are always loaded
LIGHTTPD_MODULES_INSTALL := mod_h2 mod_indexfile mod_dirlisting mod_staticfile $(LIGHTTPD_MODULES-y)

$(STATEDIR)/lighttpd.targetinstall:
	@$(call targetinfo)

	@$(call install_init, lighttpd)
	@$(call install_fixup, lighttpd,PRIORITY,optional)
	@$(call install_fixup, lighttpd,SECTION,base)
	@$(call install_fixup, lighttpd,AUTHOR,"Daniel Schnell <danielsch@marel.com>")
	@$(call install_fixup, lighttpd,DESCRIPTION,missing)

#	# bins
	@$(call install_copy, lighttpd, 0, 0, 0755, -, \
		/usr/sbin/lighttpd)
	@$(call install_copy, lighttpd, 0, 0, 0755, -, \
		/usr/sbin/lighttpd-angel)

ifdef PTXCONF_LIGHTTPD_INSTALL_SELECTED_MODULES
	@$(foreach mod,$(LIGHTTPD_MODULES_INSTALL), \
		$(call install_lib, lighttpd, 0, 0, 0644, lighttpd/$(mod));)
else
#	# modules
	@$(call install_tree, lighttpd, 0, 0, -, /usr/lib/lighttpd)
endif

#	#
#	# configs
#	#
	@$(call install_alternative, lighttpd, 0, 0, 0600, \
		/etc/lighttpd/mode_http+https.conf)
	@$(call install_alternative, lighttpd, 0, 0, 0600, \
		/etc/lighttpd/lighttpd.conf)
	@$(call install_alternative, lighttpd, 0, 0, 0600, \
		/etc/lighttpd/mode_https.conf)
	@$(call install_alternative, lighttpd, 0, 0, 0600, \
		/etc/lighttpd/mime_types.conf)
	@$(call install_alternative, lighttpd, 0, 0, 0600, \
		/etc/lighttpd/tls_extended_compatibility.conf)
	@$(call install_alternative, lighttpd, 0, 0, 0600, \
		/etc/lighttpd/tls_strong.conf)

# Config directory for application specific config files
	@$(call install_copy, lighttpd, 0, 0, 0755, \
		/etc/lighttpd/apps.confd);

ifdef PTXCONF_LIGHTTPD_MOD_FASTCGI_PHP
	@$(call install_alternative, lighttpd, 0, 0, 0600, \
		/etc/lighttpd/apps.confd/php.conf)
endif

ifdef PTXCONF_LIGHTTPD_HTTPS

#	#
#	# Default mode (https or https+http)
#	#
ifdef PTXCONF_LIGHTTPD_INSTALL_CONF_LINK_HTTPS
	@$(call install_link, lighttpd, mode_https.conf, \
		/etc/lighttpd/mode.conf)
else
	@$(call install_link, lighttpd, mode_http+https.conf, \
		/etc/lighttpd/mode.conf)
endif

#	#
#	# TLS configuration (strong or standard)
#	#
ifdef PTXCONF_LIGHTTPD_INSTALL_CONF_LINK_HTTPS_STRONG
	@$(call install_link, lighttpd, tls_strong.conf, \
		/etc/lighttpd/tls.conf)
else
	@$(call install_link, lighttpd, tls_extended_compatibility.conf, \
		/etc/lighttpd/tls.conf)
endif

endif # PTXCONF_LIGHTTPD_HTTPS

#	#
#	# Certificates and keys
#	#
ifdef PTXCONF_LIGHTTPD_HTTPS_GEN_CERT
	@$(call install_copy, lighttpd, 0, 0, 0400, \
		$(LIGHTTPD_DIR)/https-cert.pem, \
		/etc/lighttpd/https-cert.pem, n)
	@$(call install_copy, lighttpd, 0, 0, 0400, \
		$(LIGHTTPD_DIR)/dh3072.pem, \
		/etc/lighttpd/dh3072.pem, n)
else
	@$(call install_alternative, lighttpd, 0, 0, 0400, \
		/etc/lighttpd/https-cert.pem, n)
	@$(call install_alternative, lighttpd, 0, 0, 0400, \
		/etc/lighttpd/dh3072.pem, n)
endif

#	#
#	# Elrest Websocket
#	#
	@$(call install_alternative, lighttpd, 0, 0, 0600, \
		/etc/lighttpd/apps.confd/websocket/websocket_off.conf)
	@$(call install_alternative, lighttpd, 0, 0, 0600, \
		/etc/lighttpd/apps.confd/websocket/websocket_on.conf)
ifdef PTXCONF_CDS3_TSCWEBSOCKETSERVER
	@$(call install_link, lighttpd, websocket/websocket_on.conf, \
		/etc/lighttpd/apps.confd/websocket.conf)
else ifdef PTXCONF_CDS3_CMPWSSERVER
	@$(call install_link, lighttpd, websocket/websocket_on.conf, \
		/etc/lighttpd/apps.confd/websocket.conf)
else
	@$(call install_link, lighttpd, websocket/websocket_off.conf, \
		/etc/lighttpd/apps.confd/websocket.conf)
endif

#	#
#	# WBM
#	#
	@$(call install_alternative, lighttpd, 0, 0, 0600, \
		/etc/lighttpd/apps.confd/wbm/wbm_enabled.conf)
	@$(call install_alternative, lighttpd, 0, 0, 0600, \
		/etc/lighttpd/apps.confd/wbm/wbm_disabled.conf)
	@$(call install_alternative, lighttpd, 0, 0, 0600, \
		/etc/lighttpd/apps.confd/wbm/redirect_wbm.conf)
	@$(call install_link, lighttpd, wbm/wbm_enabled.conf, \
		/etc/lighttpd/apps.confd/wbm.conf)

# Redirect setting
	@$(call install_link, lighttpd, apps.confd/wbm/redirect_wbm.conf, \
		/etc/lighttpd/redirect_default.conf)

#	#
#	# WebVisu
#	#
	@$(call install_alternative, lighttpd, 0, 0, 0600, \
		/etc/lighttpd/apps.confd/webvisu.conf)
	@$(call install_alternative, lighttpd, 0, 0, 0600, \
		/etc/lighttpd/apps.confd/webvisu/webvisu_ports_default.conf)
	@$(call install_alternative, lighttpd, 0, 0, 0600, \
		/etc/lighttpd/apps.confd/webvisu/webvisu_ports_separated.conf)
	@$(call install_link, lighttpd, webvisu_ports_default.conf, \
		/etc/lighttpd/apps.confd/webvisu/webvisu_ports.conf)
	@$(call install_alternative, lighttpd, 0, 0, 0600, \
		/etc/lighttpd/apps.confd/webvisu/redirect_webvisu.conf)

#	#
#	# busybox init: start script
#	#
ifdef PTXCONF_INITMETHOD_BBINIT
ifdef PTXCONF_LIGHTTPD_STARTSCRIPT
	@$(call install_alternative, lighttpd, 0, 0, 0755, /etc/init.d/lighttpd)

ifneq ($(call remove_quotes, $(PTXCONF_LIGHTTPD_BBINIT_LINK)),)
	@$(call install_link, lighttpd, \
		../init.d/lighttpd, \
		/etc/rc.d/$(PTXCONF_LIGHTTPD_BBINIT_LINK))
endif
endif
endif

ifdef PTXCONF_LIGHTTPD_SYSTEMD_UNIT
	@$(call install_alternative, lighttpd, 0, 0, 0644, \
		/usr/lib/systemd/system/lighttpd.service)
	@$(call install_link, lighttpd, ../lighttpd.service, \
		/usr/lib/systemd/system/multi-user.target.wants/lighttpd.service)
endif

ifdef PTXCONF_LIGHTTPD_GENERIC_SITE
ifdef PTXCONF_LIGHTTPD_MOD_FASTCGI_PHP
	@$(call install_copy, lighttpd, www, www, 0644, \
		$(PTXDIST_TOPDIR)/projectroot/var/www/lighttpd.html, \
		/var/www/index.html)

	@$(call install_copy, lighttpd, www, www, 0644, \
		$(PTXDIST_TOPDIR)/projectroot/var/www/bottles.php, \
		/var/www/bottles.php)

	@$(call install_copy, lighttpd, www, www, 0644, \
		$(PTXDIST_TOPDIR)/projectroot/var/www/more_bottles.php, \
		/var/www/more_bottles.php)
else
	@$(call install_copy, lighttpd, www, www, 0644, \
		$(PTXDIST_TOPDIR)/projectroot/var/www/httpd.html, \
		/var/www/index.html)
endif
endif

# license
	@$(call install_copy, lighttpd, 0, 0, 0644, \
		$(PTXDIST_WORKSPACE)/projectroot/usr/share/licenses/oss/license.lighttpd_$(LIGHTTPD_BASE_VERSION).txt, \
		/usr/share/licenses/oss/license.lighttpd_$(LIGHTTPD_VERSION).txt)

	@$(call install_finish, lighttpd)
	@$(call touch)

# vim: syntax=make
