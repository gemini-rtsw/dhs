#!/bin/bash
# Gather what the DHS git/CI port is missing. Read-only: copies and queries
# only, changes nothing. Run on hbfswgbld-lv1 first (a build machine is most
# likely to have the headers); if it comes up empty, run it on mkodhs1 too.
#
#   scp docs/collect-dhs-deps.sh <host>:/tmp/
#   ssh <host> 'bash /tmp/collect-dhs-deps.sh'
#   scp <host>:/tmp/dhs-deps-*.tar.gz .
#
# Result: /tmp/dhs-deps-<host>.tar.gz

set -u
OUT=/tmp/dhs-deps-$(hostname -s)
rm -rf "$OUT"; mkdir -p "$OUT"/{headers,manifests,info}

say() { echo "== $*"; }

# ---------------------------------------------------------------------------
# 1. THE BLOCKER: dhsClient headers.
#    The el9 RPM in the rpm-repo ships libdhs/libgen and nothing else -- no
#    include/ directory at all. DHS cannot compile without these.
#    Wanted: dhs.h dhs++.H localDhs++.H genMutex.H
#            gen_config.h gen_eptr.h gen_file.h gen_msg.h gen_str.h
#            gen_types.h gen_util.h
#    Copy the WHOLE include tree, not those names -- they include each other
#    and probably reach into DRAMA (sds.h, arg.h), and I need to follow that.
# ---------------------------------------------------------------------------
say "dhsClient include tree"
for d in /gemsoft/opt/dhsClient/include \
         /gemsoft/opt/dhsClient \
         /opt/dhsClient/include; do
    if [ -d "$d" ]; then
        echo "   found $d"
        mkdir -p "$OUT/headers/dhsClient"
        find "$d" \( -name '*.h' -o -name '*.H' \) 2>/dev/null | while read -r f; do
            cp -a --parents "$f" "$OUT/headers/dhsClient/" 2>/dev/null
        done
    fi
done
# Catch-all: wherever they actually live on this box.
say "hunting gen_msg.h / dhs++.H anywhere on local filesystems"
find / -xdev \( -name 'gen_msg.h' -o -name 'dhs++.H' -o -name 'localDhs++.H' \
                -o -name 'gen_types.h' -o -name 'genMutex.H' \) \
     2>/dev/null | tee "$OUT/info/header-locations.txt"

# If a dhsClient SOURCE tree is here, that is the real prize -- it lets us
# build a proper dhsClient-devel RPM instead of vendoring headers.
say "dhsClient source tree?"
find / -xdev -maxdepth 6 -type d -name 'dhsClient*' 2>/dev/null \
     | tee "$OUT/info/dhsClient-source-dirs.txt"

# ---------------------------------------------------------------------------
# 2. DRAMA headers. The el9 drama RPM is libimp.a/libsds.a/libers.a only.
#    If dhsClient's headers include sds.h or arg.h, I need these too.
# ---------------------------------------------------------------------------
say "DRAMA include tree"
if [ -d /gemsoft/opt/drama ]; then
    find /gemsoft/opt/drama \( -name '*.h' -o -name '*.H' \) 2>/dev/null | while read -r f; do
        cp -a --parents "$f" "$OUT/headers/" 2>/dev/null
    done
    find /gemsoft/opt/drama -maxdepth 4 -type d > "$OUT/info/drama-layout.txt" 2>/dev/null
fi

# ---------------------------------------------------------------------------
# 3. What the deployed packages actually contain -- so I can check my %files
#    against reality instead of against the 2015 spec.
# ---------------------------------------------------------------------------
say "deployed package manifests"
for p in dhs dhs-libs dhs-QlServer dhs-devel dhsClient drama skycat; do
    rpm -q "$p" >/dev/null 2>&1 || continue
    rpm -q  "$p"                   >  "$OUT/manifests/$p.nvr"      2>&1
    rpm -ql "$p"                   >  "$OUT/manifests/$p.files"    2>&1
    rpm -q --requires "$p"         >  "$OUT/manifests/$p.requires" 2>&1
    rpm -q --provides "$p"         >  "$OUT/manifests/$p.provides" 2>&1
done
rpm -qa | grep -iE 'dhs|drama|skycat|^tcl|^tk-|itcl|itk|cfitsio|epics|ocswish' \
    | sort > "$OUT/manifests/installed-related.txt" 2>&1

# The actual RPMs, if this box has them cached or staged anywhere.
say "locating built dhs RPMs"
find / -xdev \( -name 'dhs-1.8*.rpm' -o -name 'dhsClient-*.rpm' \) 2>/dev/null \
    | head -40 | tee "$OUT/info/rpm-locations.txt"

# ---------------------------------------------------------------------------
# 4. How the legacy build derived Version/Release. The old spec said
#    `%define version __auto__`; the deployed RPMs are 1.8.5-16 (server) and
#    1.8.7-55792 (QlTools, an SVN revision). I need to know what substituted
#    those so the new 1.9.0 does not sort below what is already installed.
# ---------------------------------------------------------------------------
say "gemini-build / version substitution"
rpm -q gemini-build > "$OUT/info/gemini-build.nvr" 2>&1
rpm -ql gemini-build > "$OUT/info/gemini-build.files" 2>&1
for f in $(rpm -ql gemini-build 2>/dev/null | grep -iE '\.(mk|sh|spec|m4)$|macros'); do
    [ -f "$f" ] && cp -a --parents "$f" "$OUT/info/" 2>/dev/null
done

echo
say "environment"
{ uname -a; cat /etc/redhat-release 2>/dev/null; gcc --version 2>&1 | head -1; } \
    > "$OUT/info/uname.txt" 2>&1

tar -czf "$OUT.tar.gz" -C /tmp "$(basename "$OUT")" 2>/dev/null
echo
echo "DONE -> $OUT.tar.gz  ($(du -h "$OUT.tar.gz" 2>/dev/null | cut -f1))"
echo "Headers found: $(find "$OUT/headers" -name '*.h' -o -name '*.H' 2>/dev/null | wc -l)"
