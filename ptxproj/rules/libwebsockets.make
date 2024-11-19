# -*-makefile-*-
#
# Copyright (C) 2014 by Michael Olbrich <m.olbrich@pengutronix.de>
# Copyright (C) 2018 by Marc Oestermann <marc.oestermann@wago.com>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBWEBSOCKETS) += libwebsockets

#
# Paths and names
#
LIBWEBSOCKETS_VERSION	:= 4.3.0+972f154a
LIBWEBSOCKETS_MD5		:= ddf713f986ab950ddbee5e24bea04219
LIBWEBSOCKETS			:= libwebsockets-$(LIBWEBSOCKETS_VERSION)
LIBWEBSOCKETS_SUFFIX	:= zip
# This is the actual package URL. We renamed the archive file. To get file server mirror to work, a dummy URL with
# the changed file name is required.
#LIBWEBSOCKETS_URL		:= https://github.com/warmcat/libwebsockets/archive/v$(LIBWEBSOCKETS_VERSION)-stable.$(LIBWEBSOCKETS_SUFFIX)
LIBWEBSOCKETS_URL		:= https://github.com/warmcat/libwebsockets/archive/$(LIBWEBSOCKETS).$(LIBWEBSOCKETS_SUFFIX)
LIBWEBSOCKETS_SOURCE	:= $(SRCDIR)/$(LIBWEBSOCKETS).$(LIBWEBSOCKETS_SUFFIX)
LIBWEBSOCKETS_DIR		:= $(BUILDDIR)/$(LIBWEBSOCKETS)
LIBWEBSOCKETS_LICENSE	:= MIT WITH custom-exception
LIBWEBSOCKETS_LICENSE_FILES := file://LICENSE;md5=382bfdf329e774859fd401eaf850d29b

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

LIBWEBSOCKETS_CONF_TOOL	:= cmake
LIBWEBSOCKETS_CONF_OPT	:= \
	$(CROSS_CMAKE_USR) \
	-DLWS_WITH_SSL=ON \
	-DLWS_SSL_CLIENT_USE_OS_CA_CERTS=OFF \
	-DLWS_WITHOUT_BUILTIN_GETIFADDRS=OFF \
	-DLWS_WITHOUT_CLIENT=OFF \
	-DLWS_WITHOUT_SERVER=OFF \
	-DLWS_WITH_EXTERNAL_POLL=ON \
	-DLWS_LINK_TESTAPPS_DYNAMIC=ON \
	-DLWS_WITHOUT_EXTENSIONS=ON \
	-DLWS_WITHOUT_DAEMONIZE=ON \
	-DLWS_WITH_LIBEV=OFF \
	-DLWS_WITH_EVLIB_PLUGINS=OFF \
	-DLWS_IPV6=OFF \
	-DLWS_WITH_HTTP2=ON \
	-DLWS_WITHOUT_EVENTFD=ON \
	-DLWS_WITH_NETLINK=OFF \
	-DLWS_WITH_SYS_SMD=OFF \
	-DLWS_SUPPRESS_DEPRECATED_API_WARNINGS=OFF \
	-DLWS_WITH_MINIMAL_EXAMPLES=OFF \
	-DLWS_WITHOUT_TESTAPPS=$(call ptx/ifdef, PTXCONF_LIBWEBSOCKETS_TESTS,OFF,ON) \
	-DLWS_WITHOUT_TEST_SERVER=$(call ptx/ifdef, PTXCONF_LIBWEBSOCKETS_TESTS,OFF,ON) \
	-DLWS_WITHOUT_TEST_SERVER_EXTPOLL=$(call ptx/ifdef, PTXCONF_LIBWEBSOCKETS_TESTS,OFF,ON) \
	-DLWS_WITHOUT_TEST_PING=$(call ptx/ifdef, PTXCONF_LIBWEBSOCKETS_TESTS,OFF,ON) \
	-DLWS_WITHOUT_TEST_CLIENT=$(call ptx/ifdef, PTXCONF_LIBWEBSOCKETS_TESTS,OFF,ON)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libwebsockets.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libwebsockets)
	@$(call install_fixup, libwebsockets,PRIORITY,optional)
	@$(call install_fixup, libwebsockets,SECTION,base)
	@$(call install_fixup, libwebsockets,AUTHOR,"Michael Olbrich <m.olbrich@pengutronix.de>")
	@$(call install_fixup, libwebsockets,DESCRIPTION,missing)

	@$(call install_lib, libwebsockets, 0, 0, 0644, libwebsockets)
ifdef PTXCONF_LIBWEBSOCKETS_TESTS
	@$(call install_copy, libwebsockets, 0, 0, 0755, -, /usr/bin/libwebsockets-test-client)
	@$(call install_copy, libwebsockets, 0, 0, 0755, -, /usr/bin/libwebsockets-test-echo)
	@$(call install_copy, libwebsockets, 0, 0, 0755, -, /usr/bin/libwebsockets-test-ping)
	@$(call install_copy, libwebsockets, 0, 0, 0755, -, /usr/bin/libwebsockets-test-server)
	@$(call install_copy, libwebsockets, 0, 0, 0755, -, /usr/bin/libwebsockets-test-server-extpoll)
	@$(call install_tree, libwebsockets, 0, 0, -, /usr/share/libwebsockets-test-server)
endif

	@$(call install_copy, libwebsockets, 0, 0, 0644, $(LIBWEBSOCKETS_DIR)/LICENSE, /usr/share/licenses/oss/license.libwebsockets_$(LIBWEBSOCKETS_VERSION).txt)
	@$(call install_finish, libwebsockets)

	@$(call touch)


# vim: syntax=make
