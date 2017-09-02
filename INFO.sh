#!/bin/bash

source /pkgscripts/include/pkg_util.sh

package="WORM"
version="1.0"
displayname="WORM"
maintainer="Marc GUICHARD"
arch="$(pkg_get_unified_platform)"
description="WORM-FS is WORM (Write Once Read Many) compliant file system. Once a file is created, you cannot delete or update it, until the retention period has expired."
maintainer_url="https://github.com/dfgs/WORM-FS"
[ "$(caller)" != "0 NULL" ] && return 0
pkg_dump_info
