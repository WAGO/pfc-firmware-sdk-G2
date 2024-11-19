# -*-makefile-*-
#
# Copyright (C) 2003 by Benedikt Spranger
#               2004-2009 by the ptxdist project
#               2010 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBXML2) += libxml2

#
# Paths and names
#
LIBXML2_VERSION	:= 2.13.4
LIBXML2_MD5	:= 1c5f9cfeee665eb6f1ddc186aba4cf2f
LIBXML2		:= libxml2-$(LIBXML2_VERSION)
LIBXML2_SUFFIX	:= tar.xz
LIBXML2_SOURCE	:= $(SRCDIR)/$(LIBXML2).$(LIBXML2_SUFFIX)
LIBXML2_DIR	:= $(BUILDDIR)/$(LIBXML2)
LIBXML2_LICENSE	:= MIT AND ISC
LIBXML2_LICENSE_FILES := \
	file://Copyright;md5=f437ed9058e8e5135e47c01e973376ba \
	file://dict.c;startline=5;endline=16;md5=6bf674402d04fa793fdc1f4d26635d33 \
	file://list.c;startline=4;endline=15;md5=3fca05145285fa81f48c16c86a4a70b8

LIBXML2_URL := \
	https://download.gnome.org/sources/libxml2/$(basename $(LIBXML2_VERSION))/$(LIBXML2).$(LIBXML2_SUFFIX)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
LIBXML2_CONF_TOOL := autoconf
LIBXML2_CONF_OPT := \
	$(CROSS_AUTOCONF_USR) \
	--disable-static \
	$(GLOBAL_IPV6_OPTION) \
	--oldincludedir=$(SYSROOT)/usr/include \
	--$(call ptx/wwo, PTXCONF_LIBXML2_C14N)-c14n \
	--$(call ptx/wwo, PTXCONF_LIBXML2_CATALOG)-catalog \
	--$(call ptx/wwo, PTXCONF_LIBXML2_DEBUG)-debug \
	--$(call ptx/wwo, PTXCONF_LIBXML2_FTP)-ftp \
	--$(call ptx/wwo, PTXCONF_LIBXML2_HISTORY)-history \
	--$(call ptx/wwo, PTXCONF_LIBXML2_HTML)-html \
	--$(call ptx/wwo, PTXCONF_LIBXML2_HTTP)-http \
	--without-icu \
	--$(call ptx/wwo, PTXCONF_LIBXML2_ISO8859X)-iso8859x \
	--$(call ptx/wwo, PTXCONF_LIBXML2_LZMA)-lzma \
	--$(call ptx/wwo, PTXCONF_LIBXML2_MODULES)-modules \
	--$(call ptx/wwo, PTXCONF_LIBXML2_OUTPUT)-output \
	--$(call ptx/wwo, PTXCONF_LIBXML2_PATTERN)-pattern \
	--$(call ptx/wwo, PTXCONF_LIBXML2_PUSH)-push \
	--without-python \
	--$(call ptx/wwo, PTXCONF_LIBXML2_READER)-reader \
	--$(call ptx/wwo, PTXCONF_LIBXML2_REGEXPS)-regexps \
	--$(call ptx/wwo, PTXCONF_LIBXML2_SAX1)-sax1 \
	--$(call ptx/wwo, PTXCONF_LIBXML2_SCHEMAS)-schemas \
	--$(call ptx/wwo, PTXCONF_LIBXML2_SCHEMATRON)-schematron \
	--$(call ptx/wwo, PTXCONF_LIBXML2_THREADS)-threads \
	--$(call ptx/wwo, PTXCONF_LIBXML2_THREADS_ALLOC)-thread-alloc \
	--$(call ptx/wwo, PTXCONF_LIBXML2_DOM)-tree \
	--$(call ptx/wwo, PTXCONF_LIBXML2_DTD)-valid \
	--$(call ptx/wwo, PTXCONF_LIBXML2_WRITER)-writer \
	--$(call ptx/wwo, PTXCONF_LIBXML2_XINCLUDE)-xinclude \
	--$(call ptx/wwo, PTXCONF_LIBXML2_XPATH)-xpath \
	--$(call ptx/wwo, PTXCONF_LIBXML2_XPTR)-xptr \
	--$(call ptx/wwo, PTXCONF_LIBXML2_XPTR)-xptr-locs \
	--$(call ptx/wwo, PTXCONF_LIBXML2_MINIMUM)-minimum \
	--$(call ptx/wwo, PTXCONF_LIBXML2_LEGACY)-legacy \
	--with-tls

ifdef PTXCONF_ICONV
# --with-iconv=yes -> does the right thing for libc-iconv
LIBXML2_CONF_OPT += --with-iconv=yes
else
LIBXML2_CONF_OPT += --with-iconv=no
endif

ifdef PTXCONF_LIBXML2_ZLIB
LIBXML2_CONF_OPT += --with-zlib=$(SYSROOT)/usr
else
LIBXML2_CONF_OPT += --without-zlib
endif

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libxml2.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libxml2)
	@$(call install_fixup, libxml2,PRIORITY,optional)
	@$(call install_fixup, libxml2,SECTION,base)
	@$(call install_fixup, libxml2,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, libxml2,DESCRIPTION,missing)

	@$(call install_lib, libxml2, 0, 0, 0644, libxml2)

ifdef PTXCONF_LIBXML2_XMLLINT
	@$(call install_copy, libxml2, 0, 0, 0755, -, /usr/bin/xmllint)
endif

	@$(call install_finish, libxml2)

	@$(call touch)

# vim: syntax=make
