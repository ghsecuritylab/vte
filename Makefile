#
#    Top-level Makefile for LTP. See INSTALL for more info.
#
#    Copyright (C) 2009, Cisco Systems Inc.
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License along
#    with this program; if not, write to the Free Software Foundation, Inc.,
#    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
# Garrett Cooper, July 2009
#

# Force IDcheck.sh to fix any issues found with $(DESTDIR)/etc/group and
# $(DESTDIR)/etc/passwd automatically when after running the top-level
# install target.
CREATE_ENTRIES		?= 0

top_srcdir		?= $(CURDIR)

include $(top_srcdir)/include/mk/env_pre.mk
include $(top_srcdir)/include/mk/automake.mk

.SUFFIXES:
.SUFFIXES: .am .default .h .in .m4 .mk

vpath %.am		$(top_srcdir)/m4
vpath %.default		$(top_srcdir)/include
vpath %.h		$(top_srcdir)/include:$(top_builddir)/include
vpath %.in		$(top_srcdir)/include
vpath %.m4		$(top_srcdir)/m4
vpath %.mk		$(top_srcdir)/mk:$(top_srcdir)/mk/include

# Skip running IDcheck.sh at the end of install?
SKIP_IDCHECK		?= 0

# User wants uclinux binaries?
UCLINUX			?= 0
export UCLINUX

# CLEAN_TARGETS:	Targets which exist solely in clean.
# COMMON_TARGETS:	Targets which exist in all, clean, and install.
# INSTALL_TARGETS:	Targets which exist in clean and install (contains
# 			COMMON_TARGETS).
# BOOTSTRAP_TARGETS:	Directories required to bootstrap out-of-build-tree
# 			support.

# We're not using uclinux based targets (default).
ifneq ($(UCLINUX),1)
COMMON_TARGETS		:= pan utils
INSTALL_TARGETS		:= doc
endif

define target_to_dir_dep_mapping
ifeq ($$(filter %-clean,$(1)),) # not *-clean
$(1): | $$(abs_top_builddir)/$$(basename $$(subst -,.,$(1)))
else				# clean
$(1):: | $$(abs_top_builddir)/$$(basename $$(subst -,.,$(1)))
endif
endef

COMMON_TARGETS		+= testcases tools
# Don't want to nuke the original files if we're installing in-build-tree.
ifneq ($(BUILD_TREE_STATE),$(BUILD_TREE_SRCDIR_INSTALL))
INSTALL_TARGETS		+= runtest testscripts
CLEAN_TARGETS		+= include runtest testscripts
endif
INSTALL_TARGETS		+= $(COMMON_TARGETS)
CLEAN_TARGETS		+= $(COMMON_TARGETS) lib
BOOTSTRAP_TARGETS	:= $(sort $(COMMON_TARGETS) $(CLEAN_TARGETS) $(INSTALL_TARGETS))

CLEAN_TARGETS		:= $(addsuffix -clean,$(CLEAN_TARGETS))
INSTALL_TARGETS		:= $(addsuffix -install,$(INSTALL_TARGETS))
MAKE_TARGETS		:= $(addsuffix -all,$(filter-out lib,$(COMMON_TARGETS)))

# There's no reason why we should run `all' twice. Otherwise we're just wasting
# 3+ mins of useful CPU cycles on a modern machine, and even more time on an
# overtaxed one, or one where -j => 1 was specified.
all: $(addsuffix -all,$(COMMON_TARGETS)) Version

$(MAKE_TARGETS): lib-all

.PHONY: include-all include-install
include-install: $(top_builddir)/include/config.h include/mk/config.mk include-all

INSTALL_DIR		:= $(DESTDIR)/$(prefix)

# DO NOT REMOVE THIS CALL (see clean_install_dir call below...)!!!!
ifdef MAKE_3_80_COMPAT
INSTALL_DIR		:= $(call MAKE_3_80_abspath,$(INSTALL_DIR))
else
INSTALL_DIR		:= $(abspath $(INSTALL_DIR))
endif

# build tree bootstrap targets and $(INSTALL_DIR) target.
$(sort $(addprefix $(abs_top_builddir)/,$(BOOTSTRAP_TARGETS)) $(INSTALL_DIR) $(DESTDIR)/$(bindir)):
	mkdir -m 00755 -p "$@"

## Pattern based subtarget rules.
lib-install: lib-all

$(MAKE_TARGETS) include-all lib-all:
	$(MAKE) -C "$(subst -all,,$@)" \
		-f "$(abs_top_srcdir)/$(subst -all,,$@)/Makefile" all

# Let's not conflict with ac-clean, maintainer-clean, etc, so.
$(filter-out include-clean,$(CLEAN_TARGETS))::
	-$(MAKE) -C "$(subst -clean,,$@)" \
		 -f "$(abs_top_srcdir)/$(subst -clean,,$@)/Makefile" clean 

# Just like everything depends on include-all / -install, we need to get rid
# of include last to ensure that things won't be monkey screwed up. Only do
# this if we're invoking clean or a subclean directly though.
ifneq ($(filter clean,$(MAKECMDGOALS)),)
INCLUDE_CLEAN_RDEP_SUBJECT	:= $(CLEAN_TARGETS)
else
ifneq ($(filter %clean,$(MAKECMDGOALS)),)
INCLUDE_CLEAN_RDEP_SUBJECT	:= $(MAKECMDGOALS)
endif
endif

# Remove potential for circular dependencies.
INCLUDE_CLEAN_RDEPS		:= $(filter-out include-clean,$(INCLUDE_CLEAN_RDEP_SUBJECT))

include-clean:: $(INCLUDE_CLEAN_RDEPS) | $(abs_top_builddir)/include
	-$(MAKE) -C include -f "$(abs_top_srcdir)/include/Makefile" clean

# include-install is separate to avoid creating a circular dependency below in
# the install target.
$(INSTALL_TARGETS) include-install lib-install:
	$(MAKE) -C "$(subst -install,,$@)" \
		-f "$(abs_top_srcdir)/$(subst -install,,$@)/Makefile" install

# Just in case configure hasn't been run yet, let's not overambitiously remove
# the $(INSTALL_DIR).
.PHONY: clean_install_dir
clean_install_dir::
	$(RM) -Rf "$(INSTALL_DIR)"

# Clean the directory if the build-tree is properly configured and not set to
# the srcdir.
ifeq ($(filter $(BUILD_TREE_STATE),$(BUILD_TREE_SRCDIR_INSTALL) $(BUILD_TREE_UNCONFIGURED)),)
# Make sure that we don't whack `/'!!!!!
ifneq ($(INSTALL_DIR),/)
CLEAN_TARGETS	+= clean_install_dir
endif
endif

clean:: $(CLEAN_TARGETS)
	$(RM) -f Version
	-$(MAKE) -C openlibs clean
	@$(MAKE) -C testcases/third_party_suite clean
	@$(MAKE) -C testcases/module_test clean
	@$(MAKE) -C testcases/vte_tests_suite clean
	rm -rf testcases/bin/

$(foreach tgt,$(MAKE_TARGETS) include-all lib-all $(filter-out clean_install_dir,$(CLEAN_TARGETS)) $(INSTALL_TARGETS) include-install lib-install,$(eval $(call target_to_dir_dep_mapping,$(tgt))))

BINDIR_INSTALL_SCRIPTS	:= execltp
SRCDIR_INSTALL_SCRIPTS	:= IDcheck.sh runalltests.sh runltp runltplite.sh ver_linux
SRCDIR_INSTALL_READONLY	:= Version
SRCDIR_INSTALL_TARGETS	:= $(SRCDIR_INSTALL_SCRIPTS) $(SRCDIR_INSTALL_READONLY)

# Save space. We only need the first line in ChangeLog for runltp*.
Version: $(top_srcdir)/ChangeLog
	head -n 1 "$^" > "$@"

$(INSTALL_DIR)/Version: Version
	install -m 00644 "$(top_builddir)/$(@F)" "$@"

$(addprefix $(DESTDIR)/$(bindir)/,$(BINDIR_INSTALL_SCRIPTS)): %:
	install -m 00755 "$(top_builddir)/$(@F)" "$@"

$(addprefix $(INSTALL_DIR)/,$(SRCDIR_INSTALL_SCRIPTS)): %:
	install -m 00755 "$(top_srcdir)/$(@F)" "$@"

ifneq ($(BUILD_TREE_STATE),$(BUILD_TREE_SRCDIR_INSTALL))
INSTALL_TARGETS		+= $(addprefix $(INSTALL_DIR)/,$(SRCDIR_INSTALL_TARGETS))
endif
INSTALL_TARGETS		+= $(addprefix $(DESTDIR)/$(bindir)/,$(BINDIR_INSTALL_SCRIPTS))

$(INSTALL_TARGETS): $(INSTALL_DIR) $(DESTDIR)/$(bindir)

## Install
install: $(INSTALL_TARGETS)
ifeq ($(SKIP_IDCHECK),0)
	-@CREATE_ENTRIES=$(CREATE_ENTRIES) \
	 DESTDIR="$(DESTDIR)" \
	 "$(top_srcdir)/IDcheck.sh"
else
	@echo "*******************************************************"
	@echo "** Will not run IDcheck.sh (SKIP_IDCHECK set to 1).  **"
	@echo "*******************************************************"
endif

## Misc targets.

## Help
.PHONY: help
help:
	@echo "Please read the Configuration section in $(top_srcdir)/INSTALL"
	@exit 1

## Menuconfig
menuconfig:
	@$(SHELL) "$(top_srcdir)/ltpmenu"

## Package
package: 
	@$(RPMBUILD) -ba ltp-devel.spec

#vte related
vte:  tools lib-all utils
	@echo "***********************************************"
	@echo "** MAKE ALL - VTE tests suite                **"
	@echo "***********************************************"
	@echo
	@$(MAKE) -C testcases/vte_tests_suite all
	@$(MAKE) -C testcases/module_test all
	@echo
	@echo "***********************************************"
	@echo "** MAKE INSTALL - VTE tests suite            **"
	@echo "***********************************************"
	@echo
	@$(MAKE) -C testcases/vte_tests_suite install
	@$(MAKE) -C testcases/module_test install
	@$(MAKE) -C pan
	@$(MAKE) -C tools
	#@$(MAKE) -C tools install
	@-ln -f tools/apicmds/tst_* testcases/bin/
	@echo
	@echo "***********************************************"
	@echo "** VTE tests suite is available              **"
	@echo "***********************************************"
	@echo
	@echo "***********************************************"
	@echo "**        build Finished                     **"
	@echo "***********************************************"
apps:
	-mkdir -p testcases/bin/
	@$(MAKE) -C testcases/third_party_suite all
	@$(MAKE) -C testcases/third_party_suite install
	@echo
	@echo "***********************************************"
	@echo "**        apps build Finished                **"
	@echo "***********************************************"




## End misc targets.
