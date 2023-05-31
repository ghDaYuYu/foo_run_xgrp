/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "stdafx.h"

// version info
DECLARE_COMPONENT_VERSION(PLUGIN_NAME,
// component version
FOO_RUN_XGRP_VERSION,
"Released by: Dayuyu (v1.0.5)\n"
"Version: "FOO_RUN_XGRP_VERSION"\n"
"Compiled: "__DATE__ "\n"
"fb2k SDK: "PLUGIN_FB2K_SDK"\n"
"\n"
PLUGIN_NAME":\n\n Executes external applications per group\n\n"
"\n"
"Inspired by Run services (foo_run.dll) developed by Florian\n\n"
"[2023 - 05 - 30 v1.0.5]\n\n"
"x32 & x64 Installer (Dayuyu)\n\n"
"[2015 - 12 - 23 v1.00_beta] to [2020 - 01 - 11 v1.04]\n\n"
"See https://github.com/ghDaYuYu/foo_run_group\n\n"
)

// This will prevent users from renaming your component around (important for proper troubleshooter behaviors) or loading multiple instances of it.
VALIDATE_COMPONENT_FILENAME(PLUGIN_DLLFILENAME);