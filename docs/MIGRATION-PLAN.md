# DHS to GitHub + gemini-rtsw-ci — migration plan

Four repos, GitLab -> GitHub, each on the pipeline, each publishing its own
`-devel`. Ordered by dependency: nothing here can be done out of sequence.

```
dhs  (the server)
├── dhsClient-devel   dhs.h, gen_*.h
│   └── drama-devel   sds.h, Ers.h, imp.h, drama.h
└── skycat-devel      Fits_IO.h, Mem.h  (qldp -> libqldp.so, ships in dhs-libs)
```

All three `-devel` subpackages **already exist in the specs**. They were never
published to rpm-repo — only the main packages were uploaded. So this is a
packaging-and-publishing job, not new packaging work.

---

## Phase 0 — prerequisites (Hawi)

- [ ] Create GitHub repos: `gemini-rtsw/drama`, `gemini-rtsw/dhsClient`,
      `gemini-rtsw/skycat`. (`gemini-rtsw/dhs` exists and is empty.)
- [ ] Grant each repo **Write** access to the `rpm-repo` package —
      github.com/orgs/gemini-rtsw/packages/container/rpm-repo/settings.
      Already done for `dhs`. Without it: `denied: permission_denied:
      write_package` at publish.
- [ ] **Decide the shared-package question.** `dhs-libs` and `dhs-QlServer`
      are built by `dhs` *and* deployed on workstations from the older line
      (`1.8.5-6.el7`, `1.8.7-55792.el9`). A 1.9.0 publish upgrades them. Take
      the line over, rename the server-side packages, or do not publish those
      two. **Nothing publishes until this is answered.**
- [ ] Confirm the `dhs` version number. MK runs `1.8.5-16`; CP unchecked
      (`ssh cpodhs03 'rpm -qa | grep -i "^dhs"'`). 1.9.0 is a guess that
      clears MK.
- [ ] Heads-up to whoever owns `skycat` — it is in the `operation-tools`
      GitLab group, not `dhs-related`.

## Phase 1 — teach the pipeline to build 32-bit (blocks everything)

Every one of these four packages is i686. `rpmbuild` refuses `BuildArch: i686`
on an x86_64 host: *"No compatible architectures found for build"*.

- [ ] Apply `docs/gemini-rtsw-ci-32bit.patch` to `gemini-rtsw-ci/build_rpm.sh`
      — ~6 lines: derive `setarch i686` from the spec's `BuildArch`, prefix
      `dnf builddep` and `rpmbuild` with it, add `--nobest`, and pin repo
      metadata to x86_64 so EPEL does not 404 on `arch=i386`.
      Verified working locally.

## Phase 2 — drama (no dependencies) — BUILDS, devel verified

Status: builds on rockylinux:9 and produces `drama-2.0-5.git<hash>.el9.i686`
plus `drama-devel`, and **drama-devel contains sds.h, Ers.h, imp.h, drama.h
and arg.h** -- the assumption the rest of the chain rested on, now checked.

**Build from `6ff091a` (2024-05-03), not HEAD.** Production is
`drama-2.0-4.el9.i686`, Build Date 2024-05-28, Build Host
sbfswgbld9-lv1.cl.gemini.edu -- so it came from the May-2024 tree, NOT from
tag `2.0-4` (commit 7bee9e5, 2020-09-17), which is older than production. The
release number was carried forward.

Between `6ff091a` and HEAD: **zero .h and zero source changes** -- only
`etc/` (the systemd migration) and `drama.spec`. So:

- `drama-devel` is identical either way; dhs is unblocked regardless.
- The main `drama` package is the only thing at risk, and building from HEAD
  would **break the DHS server**: HEAD deletes `etc/drama.init.d` and
  `etc/scripts/imp_master.sh` in favour of `etc/imp_master.service`, but
  `dhs/scripts/GemBootStart` lines 34 and 39 call
  `/etc/init.d/imp_master status|start`. The workstations do not run
  GemBootStart, which is why the systemd migration was safe for them.

Ship the systemd migration as its own release, paired with a GemBootStart
change. Do not inherit it by building from `main`.

### Version: anchor to production, and do NOT reset the release to 1

`drama-2.0-5.git<hash>` — version `2.0` unchanged (it is the upstream AAO
version and what production runs); release `4` -> `5`. The pipeline's usual
`Release: 1.git<hash>` sorts BELOW the deployed `-4` and could never install
as an upgrade. Verified with `rpm.labelCompare`.

### The i686 build-tool trap (applies to all four repos)

`flex`, `byacc`, `gcc` and `make` have **no i686 build** on Rocky 9. Under
`setarch i686`, `dnf builddep` resolves an unqualified `BuildRequires: flex`
to `flex.i686` and dies with "No matching package to install". Install
host-arch build tools in `custom-repo-setup.sh`, which runs before builddep
at native arch; they are then already satisfied when builddep looks.

dhs did not hit this only because `byacc` happened to be pre-installed in the
test container. Every one of these specs is exposed to it.

## Phase 2 detail — drama

- [ ] Mirror GitLab -> GitHub preserving all branches and tags
      (`git push --mirror`).
- [ ] Port `drama.spec` to the pipeline: `%{auto_version}`/`%{auto_release}`
      -> `%global specver` + `1.git%{git_hash}%{?dist}`; `BuildArch: %{arch}`
      -> `i686`; `gmake` -> `make`; drop the gemini-build macros.
- [ ] Add `.github/workflows/ci.yml`, the `gemini-rtsw-ci` submodule, and a
      `custom-repo-setup.sh` if it needs pinned 32-bit deps.
- [ ] Verify **`drama-devel` actually contains the headers.** Its `%files
      devel` uses `-f rpm_files_devel`, a list generated during the build —
      confirm `sds.h`, `Ers.h`, `imp.h`, `drama.h` land in it.
- [ ] Green build publishes `drama` + `drama-devel` to rpm-repo.

## Phase 3 — dhsClient and skycat (parallel; dhsClient needs Phase 2)

**dhsClient** — GitLab HEAD is *"Changes for EL9 build script"* (Mar 2025).

- [ ] Mirror to GitHub with branches and tags (51 commits, 32 tags, back to
      2002-09-30; already git-svn converted).
- [ ] Port the spec. Same macro work, plus two bugs worth fixing while there:
      - `Requires: ... dhsClient` — the package requires **itself**.
      - `Requires: skycat` and `BuildRequires: skycat-devel` — dhsClient
        includes no skycat header. This is what drags skycat/itcl/itk into
        every build that installs dhsClient.
- [ ] Confirm `dhsClient-devel` ships `/gemsoft/opt/dhsClient/include`
      (`Makefile:54` does `cp $(INCLUDES) release/include`, so it should).

**skycat** — GitLab HEAD is *"Changes for EL9"* (Apr 2025).

- [ ] Mirror to GitHub, port spec, add pipeline files.
- [ ] `skycat-devel` already lists `include/{tclutil,astrotcl,rtd,cat,skycat}`.

## Phase 4 — dhs (needs Phases 1-3)

- [ ] Mirror GitLab -> GitHub. **GitLab `main` is the base, not SVN**: its
      newest `git-svn-id` is `trunk@70223`, matching the SVN working copy's
      last-changed revision, and it carries 2024-25 EL9 work on top.
      265 commits, 100 tags, branches incl. `yoyo`.
- [ ] Rebase the port in this working tree onto GitLab `main`. What still
      applies, and what does not:

  | change | status against GitLab main |
  |---|---|
  | `export GEMINI_TOP=/gemsoft` in `%build` | **still needed** |
  | `export EPICS_BASE=...` (else `staChannel.C` is silently dropped) | **still needed** |
  | `%setup -q` (theirs is `-n %name`; pipeline builds `<name>-<version>/`) | **still needed** |
  | `--prefix=%{_prefix}/opt/%{name}` (RPATH had the build dir) | **still needed** |
  | `CC="gcc -m32"` for configure probes | still worth it, see note |
  | pipeline port: `BuildArch: i686`, git-hash release, `make`, systemd unit, `custom-repo-setup.sh`, `ci.yml` | **still needed** |
  | `-fcommon` for `press/gzip.h` | **drop** — they fixed it properly in source with an `extern` guard |
  | `genListOf.H` `ret = FALSE` -> `NULL` | **drop** — they added global `-fpermissive` in `include.mk.in` |
  | `DHS_GUI` gate + `arrayMath`/`qldp` ungating | reconsider — their tree builds everything; the gate only matters if the server build should skip the GUI |

  Note on the configure probes: `include.mk.in` appends `-m32` *after*
  configure runs, so every probe measures the 64-bit ABI. The visible symptom
  was `SIZEOF_LONG=8`, which is **defined but never read** — so it is hygiene,
  not a live bug. Other probes could differ under `-m32`.

- [ ] Re-verify against production: `rpm -ql dhs dhs-libs dhs-QlServer` on
      mkodhs1 vs the built RPMs. This is what caught `libqldp.so` missing
      from `dhs-libs`.
- [ ] Green build publishes `dhs`, `dhs-libs`, `dhs-QlServer`, `dhs-devel`.

## Phase 5 — deployment (not started, decisions needed)

- [ ] **mkodhs1/mkodhs2 are EL6 i386.** The pipeline builds EL9 i686. Moving
      the DHS servers to EL9 is a platform migration, not a rebuild — the
      workstations went to EL9, the servers did not.
- [ ] **The systemd unit does not supervise the daemons.** `GemBootStart
      server` launches five processes and returns, so `deploy/dhs.service` is
      `Type=oneshot`/`RemainAfterExit=yes`. Real supervision means one unit per
      daemon plus a decision about drama's `/etc/init.d/imp_master`, which
      `GemBootStart` starts first.
- [ ] **logrotate is not replaced.** The old `%post` generated
      `/etc/logrotate.d/dhs` line by line with a hostname-derived path. Needs
      a packaged equivalent.
- [ ] Test on mkodhsx before production.

---

## Done

- DHS server builds on EL9 and produces all four RPMs, 32-bit, EPICS linked.
- Verified file-for-file against mkodhs1 (only `/etc/init.d/dhs` differs, by
  design).
- The legacy 32-bit stack installs from rpm-repo (`custom-repo-setup.sh`).
- `ld.gold` works around `ld.bfd` 2.35.2-72.el9 segfaulting on the i686
  libraries; see MIGRATION.md.
- Nine issues found and fixed; see MIGRATION.md for the full table.

## Dead ends, recorded so nobody repeats them

- **SVN dumps are unnecessary.** `dhs`, `dhsClient` and `drama` were all
  git-svn converted to GitLab years ago, and GitLab is current with SVN.
  ~11 hours of `svnrdump` for nothing. Check GitLab first.
- `svnrdump` 1.7 writes a range-start revision with paths relative to the
  dumped URL and every later revision with paths absolute from the repo root,
  so `svnadmin load` fails on the second revision. If anyone ever does need a
  subtree dump, that is the trap.
