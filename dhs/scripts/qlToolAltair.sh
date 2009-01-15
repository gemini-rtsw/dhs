#!/bin/sh

export CDHSQLTOOL_PLUGIN_PATH="${DHS_BASE}/plugins/guideCameraPlugin/"
export CDHSQLTOOL_PLUGIN="${CDHSQLTOOL_PLUGIN_PATH}/qlToolAltairPlugin.tcl"

echo "==> starting Altair version of dhsQlTool..."
echo ""

dhsQlTool `hostname`:0.0 ${CDHSQLTOOL_PLUGIN}
