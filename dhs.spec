# The core DHS server only -- the five daemons GemBootStart launches, plus the
# shared libraries and the Quicklook server. dhs-QlTools and dhs-Console are
# workstation packages built from their own line (1.8.7-55792 on
# hbftelops-ld3); neither is installed on the DHS server, so %build passes
# DHS_GUI=NO and the Tcl/Tk GUI subdirs are skipped.
#
# DHS is a 32-bit build: conf/cc_*.mk and conf/cxx_*.mk hard-code `gcc -m32`,
# and it links the legacy Gemini i686 stack under /gemsoft (drama, dhsClient,
# tcl/tk 8.5, EPICS 3.14.12). Hence BuildArch i686 and the (x86-32)
# BuildRequires. Those packages are pinned and installed by
# custom-repo-setup.sh, which runs before `dnf builddep`.
%define _prefix /gemsoft
%define gemopt  opt
# Anchored to production: mkodhs1 runs dhs-1.8.5-16.el6.gemini, built
# 2022-03-14 on sbfswgdev4 from d4aed39 (hstecher, REL-3840) -- the last
# functional change to this tree. Everything since is EL9 build fixes.
# 1.8.5 stays; the release carries the increment, 16 -> 17.
#
# NB literal, not a macro. build_rpm.sh stage 2 derives the tarball name by
# GREPPING "^Version:"; on a macro it silently substitutes 1.0, then builds
# dhs-1.0.tar.gz against a spec asking for dhs-1.8.5.tar.gz and fails with
# "Bad source". Only the lightweight profile resolves it via rpmspec.
%global specrel 17

# $GIT_HASH first: build_rpm.sh computes it on the host and passes it in.
# Shelling out to git alone yields "nogit" inside the builder.
%define git_hash %(if [ -n "$GIT_HASH" ]; then echo "$GIT_HASH"; else git rev-parse --short HEAD 2>/dev/null || echo nogit; fi)

Summary: The DHS Server
Name:    dhs
Version: 1.8.5
Release: %{specrel}.git%{git_hash}%{?dist}
URL:     http://www.gemini.edu
License: Gemini
Source0: %{name}-%{version}.tar.gz
BuildArch: i686
Prefix:  %{_prefix}

# What the daemons load at runtime. Matches the DHS server's installed set:
# dhsClient, drama, dhs-libs. Deliberately unversioned -- see the pinning
# guidance in gemini-rtsw-ci/README.md.
Requires: gemini-top gemini-setup drama dhsClient cfitsio
Requires: dhs-libs = %{version}-%{release}
Requires: rsyslog
Requires: systemd
Requires(pre): shadow-utils

BuildRequires: gcc gcc-c++ make autoconf byacc binutils-gold
BuildRequires: systemd-rpm-macros
BuildRequires: glibc-devel(x86-32) libstdc++-devel(x86-32)
BuildRequires: drama dhsClient
BuildRequires: cfitsio-devel(x86-32)
BuildRequires: tcl-devel(x86-32) tk-devel(x86-32)
BuildRequires: epics-base-devel(x86-32)
BuildRequires: gemini-top

# From the el9 work on main (fec9f1a): suppress /usr/lib/.build-id/* symlinks.
# Several of these packages ship the same build-ids and would otherwise
# conflict on a host that installs more than one.
%define _build_id_links none

# stop rpm from stripping binaries (for debugging)
%define debug_package %{nil}
%define __strip /bin/true

%description
Gemini Data Handling System server(s).

%package devel
Summary: dhs
Requires: dhs
%description devel
Headers and static libraries for building against dhs.

%package libs
Summary: dhs
%description libs
DHS common libraries.

%package QlServer
Summary: dhs
Requires: gemini-top gemini-setup drama dhsClient cfitsio
Requires: dhs-libs = %{version}-%{release}
%description QlServer
Quicklook Server.

%prep
# build_rpm.sh builds the tarball as <name>-<version>/ (dir_name in its stage 2),
# so the default %setup layout is correct. The legacy spec said `-n %name`,
# which matched the old gemini-build tarball and would fail here with
# "cannot find dhs" the moment the pipeline unpacks dhs-1.9.0.tar.gz.
%setup -q

%build
cd dhs
autoconf
# ld.bfd 2.35.2-72.el9 segfaults on the i686 shared libraries this links
# against; ld.gold handles them. conf/cc_bin.mk overrides CC/CXX but does use
# $(LDFLAGS) on the link line, so this is the one place the flag survives.
export LDFLAGS="-fuse-ld=gold"

# configure.in derives every /gemsoft path from $GEMINI_TOP
# (DHS_CLIENT_INSTALL_BASE, SKYCAT, DRAMA). rpmbuild runs a non-login shell,
# so the gemini-top profile.d snippet is never sourced and the includes come
# out as -I/opt/dhsClient/include, which does not exist.
export GEMINI_TOP=/gemsoft

# dhsStatus/Makefile gates staChannel.C and merger.C behind
# `ifneq ($(EPICS_BASE),)` -- placed ABOVE its `include ../conf/cxx_bin.mk`,
# so make evaluates it before conf/include.mk has defined EPICS_BASE. The
# historical build got away with it because the gemsoft profile exported
# EPICS_BASE into the environment. Without it the two files are silently
# dropped and dhsStatus fails to link on cStaChannel::*.
export EPICS_BASE=/gemsoft/opt/epics/base

# -fcommon: this tree predates GCC 10. Headers carry tentative definitions --
# `unsigned short *dbuf;` in press/gzip.h, included by gzip.c, gzip_inflate.c
# and gzip_util.c -- which GCC 9 and earlier merged into a single COMMON
# symbol. GCC 10 made -fno-common the default, so each becomes a definition
# and the link fails with "multiple definition of dbuf". -fcommon restores
# the behaviour the code was written against; the alternative is editing
# every such header to say extern, which is a source change this migration
# has no reason to make.
#
# Probe with the SAME compiler the build uses. conf/include.mk.in appends
# -m32 after configure has already run its tests, so a plain `gcc` probe
# reports the 64-bit ABI -- SIZEOF_LONG=8 for a build whose longs are 4 bytes.
# --prefix feeds INSTALL_DIR, and conf/include.mk.in:174 uses INSTALL_DIR for
# NOTHING but -Wl,-rpath=$(INSTALL_DIR)/lib. Where files actually land is
# RELEASE_DIR, hardcoded to $(TOP)/release. The legacy spec passed
# --prefix=$PWD/../release, which baked the *build* directory into the RPATH
# of every shipped binary:
#   RPATH /root/rpmbuild/BUILD/dhs-1.9.0/dhs/../release/lib:...
# Harmless (the loader skips a path that does not exist, and
# /gemsoft/etc/ld.so.conf.d/dhs.so.conf covers the real one) but wrong, and
# rpmlint flags it. Point it at the install location instead: same build
# layout, correct RPATH.
./configure --prefix=%{_prefix}/opt/%{name} \
    CC="gcc -m32" CXX="g++ -m32" \
    CFLAGS="-g -O2 -fcommon" LDFLAGS="-fuse-ld=gold"

make DHS_GUI=NO
make DHS_GUI=NO install
cd ..

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/%{_prefix}/opt/%{name}/var
mkdir -p $RPM_BUILD_ROOT/%{_prefix}/etc/profile.d
mkdir -p $RPM_BUILD_ROOT/%{_prefix}/etc/ld.so.conf.d
mkdir -p $RPM_BUILD_ROOT/%{_prefix}/tmp
mkdir -p $RPM_BUILD_ROOT/tmp
mkdir -p $RPM_BUILD_ROOT/%{_prefix}/opt/%{name}/etc
mkdir -p $RPM_BUILD_ROOT/etc/rsyslog.d
mkdir -p $RPM_BUILD_ROOT%{_unitdir}
mkdir -p $RPM_BUILD_ROOT%{_sysconfdir}/sysconfig
cp -a dhs/release/* $RPM_BUILD_ROOT/%{_prefix}/opt/%{name}
rm -Rf $RPM_BUILD_ROOT/%{_prefix}/opt/%{name}/images
cp -a dhs/conf/dhs.conf* $RPM_BUILD_ROOT/%{_prefix}/tmp/
cp -a etc/dhs.profile.d $RPM_BUILD_ROOT/%{_prefix}/etc/profile.d/dhs.sh
cp -a etc/dhs.rsyslog.d $RPM_BUILD_ROOT/etc/rsyslog.d/dhs.conf
cp -a createDhsConfigDirs.sh $RPM_BUILD_ROOT/tmp/
cp -a dhs/conf/dhsconfig/* $RPM_BUILD_ROOT/%{_prefix}/opt/%{name}/var/
echo "%{_prefix}/%{gemopt}/dhs/lib" >  $RPM_BUILD_ROOT/%{_prefix}/etc/ld.so.conf.d/dhs.so.conf

cp -a dhs/conf/server.conf.* $RPM_BUILD_ROOT/%{_prefix}/opt/%{name}/etc/

chmod 755 $RPM_BUILD_ROOT/tmp/createDhsConfigDirs.sh

install -Dpm 0644 deploy/dhs.service   $RPM_BUILD_ROOT%{_unitdir}/dhs.service
install -Dpm 0644 deploy/dhs.sysconfig $RPM_BUILD_ROOT%{_sysconfdir}/sysconfig/dhs
exit 0

%pre
# gemdhs:gemini own the staging area, the log directory and the daemons. The
# uid/gid are fixed site-wide; they are NOT allocated dynamically.
getent group gemini >/dev/null || groupadd -g 2000 gemini
getent passwd gemdhs >/dev/null || useradd -g gemini -u 5052 -M -d %{_prefix}/opt/%{name} gemdhs
exit 0

%post
/tmp/createDhsConfigDirs.sh %{version}

# Staging area the daemons write into.
[ -d /staging ] || mkdir -p /staging
chmod 775 /staging
chown root:gemini /staging

%systemd_post dhs.service

%preun
%systemd_preun dhs.service

%postun
%systemd_postun_with_restart dhs.service

%post libs
/sbin/ldconfig

%postun libs
/sbin/ldconfig

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%attr(755, root, root) %{_prefix}/opt/%{name}/bin/dhsCommand
%attr(755, root, root) %{_prefix}/opt/%{name}/bin/dhsData
%attr(755, root, root) %{_prefix}/opt/%{name}/bin/dhsPut
%attr(755, root, root) %{_prefix}/opt/%{name}/bin/dhsSim4Data
%attr(755, root, root) %{_prefix}/opt/%{name}/bin/dhsStatus
%{_prefix}/opt/%{name}/man
%attr(755, root, root) %{_prefix}/opt/%{name}/scripts/dhsCleanup
%attr(755, root, root) %{_prefix}/opt/%{name}/scripts/GemBootStart
%attr(755, root, root) %{_prefix}/opt/%{name}/scripts/MakeClassicalMedia
%attr(755, root, root) %{_prefix}/opt/%{name}/scripts/MakePrImages
%attr(755, root, root) %{_prefix}/opt/%{name}/scripts/MakeUserMedia
%attr(755, root, root) %{_prefix}/opt/%{name}/scripts/newsyslog
%attr(755, root, root) %{_prefix}/opt/%{name}/scripts/TestBootStart
%attr(755, root, root) %{_prefix}/opt/%{name}/scripts/dhsServerStart.sh
%attr(755, root, root) %{_prefix}/opt/%{name}/scripts/dhsServerStop.sh
%attr(755, root, root) %{_prefix}/opt/%{name}/scripts/dhsServerRestart.sh
%attr(755, root, root) %{_prefix}/opt/%{name}/scripts/rescue
%{_prefix}/opt/%{name}/config
%{_prefix}/opt/%{name}/sql
%{_prefix}/opt/%{name}/var
%{_prefix}/opt/%{name}/etc/
%{_prefix}/etc/profile.d
%{_prefix}/tmp
/tmp/createDhsConfigDirs.sh
%attr(755, root, root) /etc/rsyslog.d/dhs.conf
%{_unitdir}/dhs.service
%config(noreplace) %{_sysconfdir}/sysconfig/dhs

%files libs
%defattr(-,root,root,-)
# libqldp.so and pkgIndex.tcl come from qldp/ and BOTH ship in the deployed
# dhs-libs (checked against `rpm -ql dhs-libs` on mkodhs1). dhs-libs is a
# shared package -- the workstations have one too, and QlTools loads that
# plugin -- so omitting them would make an upgrade silently break QlTools.
%{_prefix}/opt/%{name}/lib/*.so
%{_prefix}/opt/%{name}/lib/*.tcl
%{_prefix}/etc/ld.so.conf.d

%files QlServer
%attr(755, root, root) %{_prefix}/opt/%{name}/bin/dhsQlServer

%files devel
%defattr(-,root,root,-)
%{_prefix}/opt/%{name}/include
%{_prefix}/opt/%{name}/lib/*.a

%changelog
* Fri Aug 28 2026 Hawi Stecher <hawi.stecher@noirlab.edu> - 1.9.0-1
- Migrated from SVN to git; ported to the gemini-rtsw-ci pipeline.
- Build the 32-bit stack from pinned i686 RPMs via custom-repo-setup.sh.
- Link with ld.gold: ld.bfd on EL9 segfaults on the legacy i686 libraries.
- Replaced the sysvinit script and chkconfig with a systemd unit, and the
  hand-rolled rsyslog/logrotate edits in %%post with packaged config.
- Scoped to the core server: QlTools and Console are workstation packages
  built from their own line, so %%build passes DHS_GUI=NO. That also drops
  the skycat, itk, iwidgets, tclx and ocswish dependencies. See MIGRATION.md.
* Fri Apr 17 2015 Javier Luhrs <jluhrs@gemini.edu> 1.8.5-4
- Fixed problem with log output not following file rotation.
* Wed Sep 10 2008 Felix Kraemer <fkraemer@gemini.edu> 4.0
- seperated out QlTools and Console (ever needed??) in extra rpms
* Tue Jun 17 2008 Matthieu Bec <mbec@gemini.edu> 4.0
- removed EPICS dependencies
* Tue Jun 17 2008 Matthieu Bec <mbec@gemini.edu> 3.0
- added sample config that will work for a hostname called localhost
* Mon Jun 16 2008 Vasu Upadhya<vupadhya@gemini.edu> 2.0
- second rpm
* Wed Apr 2 2008 Matthieu Bec <mbec@gemini.edu> 1.0
- first rpm, removed SYBASE dependencies
