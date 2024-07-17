# -*-makefile-*-
#
# Copyright (C) 2007 by Bjoern Buerger <b.buerger@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LESS) += less

#
# Paths and names
#
LESS_VERSION	:= 643
LESS_MD5	:= cf05e2546a3729492b944b4874dd43dd
LESS		:= less-$(LESS_VERSION)
LESS_SUFFIX	:= tar.gz
LESS_URL	:= $(call ptx/mirror, GNU, less/$(LESS).$(LESS_SUFFIX))
LESS_SOURCE	:= $(SRCDIR)/$(LESS).$(LESS_SUFFIX)
LESS_DIR	:= $(BUILDDIR)/$(LESS)
LESS_LICENSE	:= GPL-3.0-or-later AND BSD-2-Clause

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LESS_CONF_ENV	:= \
	$(CROSS_ENV) \
	ac_cv_lib_tinfo_tgoto=no \
	ac_cv_lib_xcurses_initscr=no \
	ac_cv_lib_ncursesw_initscr=$(call ptx/yesno, PTXCONF_LESS_NCURSESW) \
	ac_cv_lib_ncurses_initscr=$(call ptx/yesno, PTXCONF_LESS_NCURSES) \
	ac_cv_lib_curses_initscr=no \
	ac_cv_lib_termcap_tgetent=$(call ptx/yesno, PTXCONF_LESS_USE_TERMCAP) \
	ac_cv_lib_termlib_tgetent=no

#
# autoconf
#
LESS_CONF_TOOL	:= autoconf
LESS_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	$(GLOBAL_LARGE_FILE_OPTION)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/less.targetinstall:
	@$(call targetinfo)

	@$(call install_init, less)
	@$(call install_fixup, less,PRIORITY,optional)
	@$(call install_fixup, less,SECTION,base)
	@$(call install_fixup, less,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, less,DESCRIPTION,missing)

ifdef PTXCONF_LESS_BIN
	@$(call install_copy, less, 0, 0, 0755, -, /usr/bin/less)
endif

ifdef PTXCONF_LESS_KEY
	@$(call install_copy, less, 0, 0, 0755, -, /usr/bin/lesskey)
endif

ifdef PTXCONF_LESS_ECHO
	@$(call install_copy, less, 0, 0, 0755, -, /usr/bin/lessecho)
endif

	@$(call install_finish, less)

	@$(call touch)

# vim: syntax=make
