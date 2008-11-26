#!/bin/sh

export CDHSQLTOOL_PLUGIN_PATH="${DHS_BASE}/plugins/guideCameraPlugin/"
export CDHSQLTOOL_PLUGIN="${CDHSQLTOOL_PLUGIN_PATH}/qlToolWfsPlugin.tcl"

echo "==> starting WFS version of dhsQlTool..."
echo ""

dhsQlTool `hostname`:0.0 ${CDHSQLTOOL_PLUGIN}
