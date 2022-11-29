# -*-makefile-*-
#
# Copyright (C) 2018 by Jan Luebbe <jlu@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_SQLITE) += host-sqlite

#
# Paths and names
#
HOST_SQLITE	= $(SQLITE)
HOST_SQLITE_DIR	= $(HOST_BUILDDIR)/$(HOST_SQLITE)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

HOST_SQLITE_CONF_ENV := \
	$(HOST_ENV) \
	CPPFLAGS=" \
	-DSQLITE_ENABLE_COLUMN_METADATA=1 \
	-DSQLITE_ENABLE_FTS3_PARENTHESIS=1 \
	-DSQLITE_ENABLE_FTS4=1 \
	-DSQLITE_ENABLE_RTREE=1 \
	-DSQLITE_ENABLE_UNLOCK_NOTIFY=1 \
	-DSQLITE_SOUNDEX=1 \
	"

HOST_SQLITE_CONF_TOOL	:= autoconf
HOST_SQLITE_CONF_OPT	:= \
	$(HOST_AUTOCONF) \
	--disable-static \
	--disable-editline \
	--disable-readline \
	--enable-threadsafe \
	--disable-dynamic-extensions \
	--disable-fts5 \
	--disable-session \
	--disable-static-shell

# vim: syntax=make
