#!/bin/sh

export CDHSQLTOOL_PLUGIN_PATH="${DHS_BASE}/plugins/guideCameraPlugin/"
export CDHSQLTOOL_PLUGIN="${CDHSQLTOOL_PLUGIN_PATH}/qlToolInstPlugin.tcl"

echo "==> starting instrument version of dhsQlTool..."
echo ""

dhsQlTool `hostname`:0.0 ${CDHSQLTOOL_PLUGIN}
