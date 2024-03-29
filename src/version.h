/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once
#define PLUGIN_NAME "Run Service x Group"

#define COMPONENT_VERSION_MAJOR 1
#define COMPONENT_VERSION_MINOR 1
#define COMPONENT_VERSION_PATCH 2
#define COMPONENT_VERSION_SUB_PATCH 0

#define MAKE_STRING(text) #text
#define MAKE_COMPONENT_VERSION(major,minor,patch) MAKE_STRING(major) "." MAKE_STRING(minor) "." MAKE_STRING(patch)
#define MAKE_DLL_VERSION(major,minor,patch,subpatch) MAKE_STRING(major) "." MAKE_STRING(minor) "." MAKE_STRING(patch) "." MAKE_STRING(subpatch)
#define MAKE_API_SDK_VERSION(sdk_ver, target_ver) MAKE_STRING(sdk_ver) " " MAKE_STRING(target_ver)

//"0.1.2"
#define FOO_RUN_XGRP_VERSION MAKE_COMPONENT_VERSION(COMPONENT_VERSION_MAJOR,COMPONENT_VERSION_MINOR,COMPONENT_VERSION_PATCH)

//0.1.2.3 & "0.1.2.3"
#define DLL_VERSION_NUMERIC COMPONENT_VERSION_MAJOR, COMPONENT_VERSION_MINOR, COMPONENT_VERSION_PATCH, COMPONENT_VERSION_SUB_PATCH
#define DLL_VERSION_STRING MAKE_DLL_VERSION(COMPONENT_VERSION_MAJOR,COMPONENT_VERSION_MINOR,COMPONENT_VERSION_PATCH,COMPONENT_VERSION_SUB_PATCH)
//fb2k ver
#define PLUGIN_FB2K_SDK MAKE_API_SDK_VERSION(FOOBAR2000_SDK_VERSION, FOOBAR2000_TARGET_VERSION)

#define COMPONENT_NAME "foo_run_xgrp"
#define COMPONENT_YEAR "2023"

#define PLUGIN_FILENAME COMPONENT_NAME
#define PLUGIN_DLLFILENAME PLUGIN_FILENAME ".dll"
#define PLUGIN_VERSION FOO_RUN_XGRP_VERSION


// To support correct upgrade from the 3rd version of the plugin.
#define PLUGIN_CFG_GLOBAL_V4_19 0x0040
#define PLUGIN_CFG_GLOBAL_V4_19_FLDS 6
#define PLUGIN_CFG_GLOBAL_VERSION 0x0041
#define PLUGIN_CFG_GLOBAL_VERSION_FLDS 7

#define PLUGIN_ABOUT "about"
