# DHS: SVN -> git + gemini-rtsw-ci

Status of the port, what was verified, and what is still blocked.

Source: `http://source.gemini.edu/software/dhs/trunk` @ r71186
(last changed r70223, 2022-03-14). The working copy was clean — no local
modifications — so nothing is lost by importing it as-is.

## Scope: the core server only

`mkodhs1` (the DHS server) carries `dhs`, `dhs-libs`, `dhs-QlServer` and
`dhsClient`. The Tcl/Tk GUI packages — `dhs-QlTools`, `dhs-Console` — are
installed on *workstations* (`hbftelops-ld3`) and come from a different build
line: `1.8.7-55792` against the server's `1.8.5-16`. So this repo builds the
server and nothing else.

`dhs/Makefile` now gates the GUI subdirs (`arrayMath`, `qldp`, `dhsConsole`,
`dhsQlTool`, `dhsQlToolDS9`) behind `DHS_GUI`, which defaults to `YES` so an
existing GUI build is unchanged. The spec's `%build` passes `DHS_GUI=NO`.

Packages produced (`setarch i686 rpmspec -q dhs.spec`, verified):

    dhs-1.9.0-1.git<hash>.el9.i686
    dhs-devel-1.9.0-1.git<hash>.el9.i686
    dhs-libs-1.9.0-1.git<hash>.el9.i686
    dhs-QlServer-1.9.0-1.git<hash>.el9.i686

Out of scope, still on SVN: `dhs-QlTools`, `dhs-Console`, and the separately
packaged `dhs-config-*`, `dhs-ws`, `dhs-monitor`, `dhs-http-config`,
`DhsTranslator`.

## What DHS actually is

A 32-bit build. `dhs/conf/cc_bin.mk`, `cc_lib.mk`, `cxx_bin.mk`, `cxx_lib.mk`
and `conf/include.mk.in` all hard-code `gcc -m32` / `g++ -m32`. Everything
below follows from that.

The server's five daemons — `dhsSim4Data`, `dhsStatus`, `dhsData`,
`dhsQlServer`, `dhsCommand`, the ones `GemBootStart server` launches — link
drama (libimp/libsds/libers), dhsClient (libdhs/libgen), cfitsio and, via
`TCLTK_LDLIBS`, tcl/tk 8.5. `dhsStatus` additionally links EPICS
(`libca`/`libCom`) for `cadef.h`/`tsDefs.h` in `staChannel.H`.

Only `qldp` ever needed skycat, and `DHS_GUI=NO` skips it — but skycat still
gets *installed* in the build environment, because `dhsClient` declares
`Requires: skycat`, which in turn pulls `itcl`, `itk` and `tclx`. Heavy for a
client library, but harmless here.

## Build status: green

The server builds end to end on `rockylinux:9` and produces all four RPMs:

    dhs-1.9.0-1.git<hash>.el9.i686          211 files, 6.5M
    dhs-libs-1.9.0-1.git<hash>.el9.i686      30 files, 2.2M
    dhs-QlServer-1.9.0-1.git<hash>.el9.i686   4 files, 2.9M
    dhs-devel-1.9.0-1.git<hash>.el9.i686     41 files, 3.2M

Checked, not assumed: all six binaries are `ELF 32-bit LSB executable, Intel
80386`; `dhsStatus` carries 59 `cStaChannel::` symbols and links
`libca.so.3.14`/`libCom.so.3.14`; no unresolved shared-library dependencies.

**Checked file-for-file against production.** `rpm -ql dhs dhs-libs
dhs-QlServer` on mkodhs1 versus the four built RPMs: every path matches
except `/etc/init.d/dhs`, which the systemd unit deliberately replaces. That
diff is what caught defect #8.

The one thing that is not yet self-sufficient: the dhsClient **and** skycat
headers are hand-copied into the build container. See Blocked #2.

### What actually broke, and why none of it was visible from the spec

Seven defects, each found by compiling. Two would have shipped silently.

| # | symptom | cause | fix |
|---|---|---|---|
| 1 | `-I/opt/dhsClient/include` | `GEMINI_TOP` unset; rpmbuild runs a non-login shell so the gemsoft profile never loads | `export GEMINI_TOP=/gemsoft` in `%build` |
| 2 | **`SIZEOF_LONG=8` on a 32-bit build** | `include.mk.in` appends `-m32` *after* configure has run its probes | `./configure CC="gcc -m32" CXX="g++ -m32"` |
| 3 | `multiple definition of 'dbuf'` | `press/gzip.h:48` has a tentative definition; GCC 10 made `-fno-common` the default | `CFLAGS=-fcommon` |
| 4 | `invalid conversion from 'boolean' to 'cSig*'` | `genListOf.H:614` did `ret = FALSE`; `(char)0` was a valid null pointer constant in C++03, not since C++11 | `ret = NULL` |
| 5 | `No rule to make target ../arrayMath/libarrayMath.a` | stale comment said "only for qldp", but `include.mk.in:127` puts it in `LOCAL_LIBS`, which every binary depends on | build `arrayMath` unconditionally |
| 6 | **`undefined reference to cStaChannel::*`** | `dhsStatus/Makefile` gates `staChannel.C` behind `ifneq ($(EPICS_BASE),)` *above* its `include`, so make evaluates it before `EPICS_BASE` exists | `export EPICS_BASE=/gemsoft/opt/epics/base` |
| 7 | would fail on first pipeline run | `%setup -n %name` expects `dhs/`, but `build_rpm.sh` builds `dhs-<version>/` | plain `%setup -q` |
| 8 | **`dhs-libs` missing `libqldp.so` + `pkgIndex.tcl`** | both come from `qldp/`, which `DHS_GUI=NO` skipped -- but `dhs-libs` is a *shared* package and QlTools loads that plugin | build `qldp` unconditionally |
| 9 | build directory baked into RPATH | `--prefix` feeds only `-Wl,-rpath=$(INSTALL_DIR)/lib`; the legacy spec pointed it at the build tree | `--prefix=/gemsoft/opt/dhs` |

**#2, #6 and #8 are the dangerous ones.** Neither fails loudly. #2 yields a binary
with 64-bit `long` assumptions in a 32-bit build -- wrong struct layouts, wire
formats off by four bytes. #6 silently drops two source files from `SOURCES`
and only surfaces as a link error much later; had `dhsStatus` not referenced
those symbols externally, it would have built and shipped without EPICS
support at all. #8 builds and installs perfectly and only breaks a *different*
machine: `dhs-libs` is installed on the DHS server **and** on every
workstation, so publishing a 1.9.0 without the qldp plugin would upgrade over
the workstations' 1.8.5-6 and delete the library QlTools loads.

#9 was pre-existing -- the legacy spec passes the same `--prefix=$PWD/../release`,
so the deployed binaries carry a build path too. Harmless (the loader skips a
path that does not exist, and `/gemsoft/etc/ld.so.conf.d/dhs.so.conf` covers
the real location) but wrong, and free to fix: `--prefix` feeds nothing but
the RPATH, since `RELEASE_DIR` is hardcoded to `$(TOP)/release`.

Both #1 and #6 have the same root cause: the historical build inherited the
gemsoft environment from an interactive profile, and `rpmbuild` does not.

## Verified working

Tested locally on `rockylinux:9` against `ghcr.io/gemini-rtsw/rpm-repo:latest`.

- **The legacy i686 stack installs.** `custom-repo-setup.sh` does it: exclude
  `tcl*`/`tk*`/`itcl*` from baseos/appstream/epel so the x86_64 counterparts
  stop winning the multilib resolution, then install each legacy package by
  full NEVRA. Without the exclusions dnf reports *"does not belong to a
  distupgrade repository"* and gives up.
- **EPICS lines up exactly.** `configure.in` hard-codes `EPICS=/gemsoft/opt/epics`,
  and `epics-base-devel-3.14.12-8.el9.gemini.i686` installs `cadef.h` and
  `tsDefs.h` to `/gemsoft/opt/epics/base/include/`. `dhsStatus/staChannel.H`
  needs them.
- **The server's link line works** — drama + dhsClient + tcl/tk + cfitsio +
  EPICS `libca`/`libCom` in one `gcc -m32` command — *but only with `ld.gold`*,
  see below.
- **`dnf builddep` resolves** against the real rpm-repo, under
  `setarch i686 ... --nobest`.
- `imake`, `byacc`, `autoconf` 2.69 and the `.i686` devel packages are all in
  Rocky 9 appstream/baseos/epel/crb. `configure.in` uses pre-2.70 constructs
  (`AC_INIT(file)`, `AC_CANONICAL_SYSTEM`, `AC_TRY_COMPILE`) which 2.69 accepts.

## Resolved

All three missing -devel packages now exist and are published: drama-devel
2.0-5, dhsClient-devel 0.7-2, skycat-devel 3.1-3. Each was already defined in
its spec and had simply never been built. dhs builds against them with no
hand-copied headers anywhere.

Two further traps found while doing it, both consequences of `setarch i686`:

- **dnf will not install ANY x86_64 package under an i686 personality.**
  Base arch becomes i386 and x86_64 is not a candidate. Anything a 32-bit
  build needs that pulls in x86_64 packages -- build tools, cross-repo
  -devel packages -- must be installed in custom-repo-setup.sh, which runs
  at native arch BEFORE the setarch'd builddep. Being noarch is not enough:
  autoconf is noarch but requires m4, which is not.
- **A failing custom-repo-setup.sh does not fail the build.** build_rpm.sh
  calls it in a non-final position of an && chain, where `set -e` does not
  apply, so the failure is swallowed and the build dies much later on an
  unrelated compile error. build_rpm.sh already guards `dnf builddep` with an
  explicit `if !` check for exactly this reason; the setup-script call needs
  the same. Worth a PR to gemini-rtsw-ci.

## Formerly blocked (kept for the record)

### 1. `ld.bfd` on EL9 segfaults on these libraries

`GNU ld 2.35.2-72.el9` dies with SIGSEGV linking any of `libtcl8.5.so`,
`libtk8.5.so`, `libitk3.4.so`, `libastrotcl2.0.so`, `libcfitsio.so` (i686):

    collect2: fatal error: ld terminated with signal 11 [Segmentation fault]

`ld.gold` links all of them. The spec therefore passes `-fuse-ld=gold` through
`LDFLAGS`, which `conf/cc_bin.mk` does use on its link line. This is a
workaround, not a fix — the real fix is rebuilding those i686 RPMs on EL9.

### 2. dhsClient ships no headers — the only thing left blocking the pipeline

`dhsClient-0.7-1.el9.gemini.i686` contains `libdhs.{a,so}`, `libgen.{a,so}`,
two binaries and man pages. **No `include/` directory at all.** DHS needs
`dhs.h`, `dhs++.H`, `localDhs++.H`, `genMutex.H`, `gen_config.h`, `gen_eptr.h`,
`gen_file.h`, `gen_msg.h`, `gen_str.h`, `gen_types.h`, `gen_util.h` from it.
**Nothing compiles without these.** They were recovered from
`hbfswgbld-lv1:/gemsoft/opt/dhsClient/include` — where `rpm -ql dhsClient`
shows **no file under `include/` at all**, so they are unpackaged leftovers
that nothing would reproduce on a fresh machine.

The local build works only because those 11 files are copied into the
container by hand. For CI to run unattended they must come from an RPM:
a `dhsClient-devel` package, published once to rpm-repo. That is the last
thing standing between a green local build and a green pipeline.

Related, not blocking: the el9 rebuild of dhsClient gained
`Requires: skycat`, `libtcl8.5.so`, `libtk8.5.so` and `libnsl.so.3`, none of
which the el7 build declares. That is what drags skycat, itcl and itk into
the build container. Worth cleaning up whenever dhsClient is migrated
properly; harmless for now, since none of it reaches the DHS RPMs.

### 3. skycat ships no headers either

Identical to dhsClient: `skycat-3.1-2.el9.gemini.i686` ships libraries only,
and `/gemsoft/opt/skycat/include/` on hbfswgbld-lv1 belongs to no RPM.
`qldp/image.{C,H}` needs `Fits_IO.h` (astrotcl) and `Mem.h` (tclutil), and
qldp has to be built because `libqldp.so` ships in the shared `dhs-libs`.

skycat is in SVN, so a real `skycat-devel` is possible. Only the headers are
needed here, not the history -- 106 of them, recovered the same way.

### 4. Shared package names collide with the workstation build line

`dhs-libs` and `dhs-QlServer` are built by this repo *and* deployed on
workstations from a different, older source line:

| package | workstation (hbftelops-ld3) | this repo |
|---|---|---|
| `dhs-libs` | 1.8.5-6.el7 | 1.9.0 |
| `dhs-QlServer` | 1.8.7-55792.el9 | 1.9.0 |

1.9.0 sorts above both, so publishing to rpm-repo upgrades the workstations
whether or not that was intended. The file lists now match, so it would no
longer *break* them -- but it is still one build line silently replacing
another, and `1.8.7-55792` comes from SVN **r55792**, older than this
checkout's r71186, so the version numbers already run backwards against the
revisions. Decide before publishing: keep the names and take the line over,
rename the server-side packages, or do not publish these two at all.

### 5. The pipeline cannot build a 32-bit package yet

`rpmbuild` refuses `BuildArch: i686` while running as x86_64. See
[docs/gemini-rtsw-ci-32bit.patch](docs/gemini-rtsw-ci-32bit.patch) — about six
lines in `build_rpm.sh`, verified.

### 6. No SVN history

`source.gemini.edu` is unreachable from hstecher-ld1 (ICMP answers, every TCP
port is filtered), so the conversion cannot run there. Both `dhs` (661
revisions, from r54/2002-11-24) and `dhsClient` (220 revisions, from
r41/2002-09-30) are being dumped with `svnrdump` from hbfswgbld-lv1, which
can reach it. `svnrdump` replays every revision in the range, not just the
ones that touch the project, so ~71,000 revisions at ~1.7/sec is roughly
11 hours. `svnadmin`, `svnrdump` and `git svn` are all available locally for
the conversion once the dumps land.

## Deliberate changes from the legacy packaging

- `%define version __auto__` / `release __auto__` / `_prefix __auto__` were
  placeholders the old gemini-build system substituted. `gemini-build` is not
  in the rpm-repo. Replaced with `_prefix /gemsoft` and the pipeline's
  `1.git<hash>%{?dist}` release.
- Version bumped 1.8.5 -> 1.9.0. `1.git<hash>` sorts *below* the legacy
  release `4`, so `dhs-1.8.5-1.git<hash>.el9` would look older than the
  `dhs-1.8.5-4.el9.gemini` already deployed. **Confirm this against what is
  actually installed at MK/CP before the first publish.**
- `gmake` -> `make`: EL9 has no `gmake`.
- sysvinit -> systemd. `deploy/dhs.service` is a *minimal faithful*
  translation: `GemBootStart server` launches five daemons into the background
  and returns, so the unit is `Type=oneshot` / `RemainAfterExit=yes` and
  **systemd does not supervise the individual daemons**. Doing that properly
  means one unit per daemon and a decision about drama's own
  `/etc/init.d/imp_master`, which `GemBootStart` starts first. That is a design
  change, not a mechanical port.
- The old `%post` edited `/etc/rsyslog.conf` with `sed`, generated
  `/etc/logrotate.d/dhs` line by line with `echo -e`, and ran `chkconfig` and
  `service dhs start`. The account/group creation (`gemdhs`:`gemini`, uid 5052,
  gid 2000), `createDhsConfigDirs.sh` and the `/staging` setup are kept; the
  rest is gone. **The logrotate config is not yet replaced** — it was per-host
  (`$logfolder` is derived from the hostname) and needs a packaged equivalent.
- Runtime `Requires` on `dhs-config-server`, `qlplugins`, `ds9` and `xpa-tcl`
  dropped: none exist in the rpm-repo, and all were QlTools/Console deps.
- The main package still owns `%{_prefix}/opt/dhs/var` wholesale, which
  contains `dhsQls.config`, `dhsQlt.config` and `dhsConsole.config`. The
  legacy packaging had the same overlap with `dhs-QlTools`/`dhs-Console`, so
  this is not a regression — but if the server and workstation packages ever
  land on one host, that is where they will collide.
