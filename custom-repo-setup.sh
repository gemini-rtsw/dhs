#!/bin/bash
# Assemble the legacy 32-bit /gemsoft build stack before `dnf builddep` runs.
#
# Scope: the core DHS server. The Tcl/Tk GUI components (dhs-QlTools,
# dhs-Console) are built elsewhere, so itk, iwidgets, tclx, blt and ocswish
# are deliberately NOT installed here.
#
# skycat and itcl ARE installed, but only because dhsClient Requires skycat
# and skycat Requires itcl(x86-32) -- the server build itself never touches
# a skycat header. qldp, the one thing in the tree that does, is skipped by
# DHS_GUI=NO.
#
# DHS still compiles with -m32 (conf/cc_*.mk, conf/cxx_*.mk) and every server
# binary links tcl/tk 8.5 via TCLTK_LDLIBS, so the 32-bit stack is unavoidable.
# It cannot be resolved by BuildRequires alone, for two reasons:
#
#  1. Rocky 9 ships tcl/tk 8.6 as x86_64. Multilib requires the i686 and
#     x86_64 halves to be the SAME version, so pulling tcl-8.5.13-8.el9.i686
#     alongside base tcl 8.6 makes dnf report "does not belong to a
#     distupgrade repository" and give up. Excluding tcl*/tk*/itcl* from
#     baseos/appstream/epel removes the x86_64 competitor. itcl matters here
#     too: skycat needs itcl(x86-32) specifically, and the flat rpm-repo
#     otherwise resolves the name to itcl-3.4-10.el9.gemini.x86_64.
#
#  2. The rpm-repo is flat and holds several arches of the same NVR, so an
#     unqualified name resolves to the x86_64 build. Everything 32-bit is
#     therefore pinned by full NEVRA.
#
# Verified against ghcr.io/gemini-rtsw/rpm-repo:latest on rockylinux:9.

set -e

# Host-arch build tools MUST be installed here, not left to `dnf builddep`.
# builddep runs under `setarch i686`, and with an i686 personality dnf
# resolves unqualified names to i686 builds that do not exist on Rocky 9.
# This bites noarch packages too: autoconf is noarch, but it requires m4,
# which is arch-specific and has no i686 build --
#   nothing provides m4 >= 1.4.14 needed by autoconf-2.69-41.el9.noarch
dnf install -y --setopt=install_weak_deps=False \
    gcc gcc-c++ make autoconf m4 imake byacc flex binutils-gold

dnf config-manager \
    --setopt=baseos.exclude=tcl*,tk*,itcl* \
    --setopt=appstream.exclude=tcl*,tk*,itcl* \
    --setopt=epel.exclude=tcl*,tk*,itcl* \
    --save

dnf install -y --setopt=install_weak_deps=False \
    glibc-devel.i686 libstdc++-devel.i686 binutils-gold \
    tcl-8.5.13-8.el9.i686    tcl-devel-8.5.13-8.el9.i686 \
    tk-8.5.13-6.el9.i686     tk-devel-8.5.13-6.el9.i686 \
    cfitsio-4.1.0-1.el9.i686 cfitsio-devel-4.1.0-1.el9.i686 \
    itcl-3.4-10.el7.centos.i686 \
    drama dhsClient skycat \
    epics-base-3.14.12-8.el9.gemini.i686 \
    epics-base-devel-3.14.12-8.el9.gemini.i686 \
    gemini-top gemini-setup

# ld.bfd 2.35.2-72.el9 segfaults linking several of the i686 shared libraries
# in this stack (libtcl8.5, libtk8.5, libcfitsio). ld.gold links all of them;
# the spec passes -fuse-ld=gold through LDFLAGS. Fail loudly here rather than
# at link time if it is not installed.
command -v ld.gold >/dev/null || { echo "ERROR: ld.gold missing (binutils-gold)" >&2; exit 1; }

# Pre-install the cross-repo build dependencies here, at NATIVE arch.
# `dnf builddep` runs under `setarch i686` (so rpm accepts BuildArch: i686),
# and an i686 personality makes base arch i386 -- dnf will not consider ANY
# x86_64 package installable. dhsClient-devel needs dhsClient -> drama ->
# chkconfig/gemini-top, all x86_64-only, so it cannot be installed from
# inside the setarch'd builddep:
#   nothing provides chkconfig needed by drama-2.0-5...i686
# Installed at native arch first, builddep only has to VERIFY them.
# `|| true` deliberately: if this fails, the check below reports WHICH header
# is missing, which is far more useful than dnf's "No match for argument".
dnf install -y --nobest --setopt=install_weak_deps=False \
    dhsClient-devel skycat-devel || true

# Verify explicitly. build_rpm.sh calls this script WITHOUT checking its exit
# status -- it sits in a non-final position of an && chain, where `set -e` does
# not apply -- so a failure here is swallowed and the build carries on with the
# dependency missing. It then dies thousands of lines later on an unrelated
# compile error (gen_types.h: No such file or directory). Fail here, where the
# message is useful.
for h in /gemsoft/opt/dhsClient/include/dhs.h \
         /gemsoft/opt/skycat/include/astrotcl/Fits_IO.h; do
    [ -f "$h" ] || { echo "ERROR: $h missing -- the -devel package it comes from did not install." >&2; exit 1; }
done
echo "Cross-repo -devel headers present."
