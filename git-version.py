# motorctrl, a generic motor control application.
#
# Copyright (C) 2021 Julian Friedrich
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>. 
#
# You can file issues at https://github.com/fjulian79/motorctrl/issues
 
import subprocess

# git config --get remote.origin.url           
ret = subprocess.run(["git", "config", "--get", "remote.origin.url"], stdout=subprocess.PIPE, text=True)
git_remote_origin_url = ret.stdout.strip()

# git branch --show-current                                                   
ret = subprocess.run(["git", "branch", "--show-current"], stdout=subprocess.PIPE, text=True)
git_branch = ret.stdout.strip()

# git describe --abbrev=0 --tags 
ret = subprocess.run(["git", "describe", "--abbrev=0", "--tags"], stdout=subprocess.PIPE, text=True)
git_version_short = ret.stdout.strip()

# git describe --abbrev --dirty --always --tags
ret = subprocess.run(["git", "describe", "--abbrev", "--dirty", "--always", "--tags"], stdout=subprocess.PIPE, text=True)
git_version_long = ret.stdout.strip()

f = open("./include/git_version.h", "w")
f.write("""/*
 * motorctrl, a generic motor control application.
 *
 * Copyright (C) 2021 Julian Friedrich
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>. 
 *
 * You can file issues at https://github.com/fjulian79/motorctrl/issues
 */
 
/****************************************************************************** 
 *********** WARNING: AUTO GENERATED FILE, DO NOT EDIT IT MANUALLY! ***********
 *****************************************************************************/

#ifndef GIT_VERSION_HPP_
#define GIT_VERSION_HPP_""")

f.write("""

/**
 * @brief The remote origin url defined when starting the build
 */
""")
f.write("#define GIT_REMOTE_ORIGIN_URL       \"" + git_remote_origin_url + "\"")

f.write("""

/**
 * @brief The branch selected when starting the build
 */
""")
f.write("#define GIT_BRANCH                  \"" + git_branch + "\"")


f.write("""

/**
* @brief The latest git tag when starting the build.
*
* WARNING: Would be a empty string if there is not at least one tag on the 
*          current branch. In this case GIT_VERSION_SHORT is defined as 
*          GIT_VERSION_LONG to avoid compiler errors.
*/
""")
if git_version_short:
  f.write("#define GIT_VERSION_SHORT           \"" + git_version_short + "\"")
else:
  f.write("#define GIT_VERSION_SHORT           GIT_VERSION_LONG")

f.write("""

/**
 * @brief The latest git tag including offset and short hash when starting the 
 * build.
 *
 * If this is equal to GIT_VERSION_SHORT the build is based on a clean tag 
 * without any changes.
 */
""")
f.write("#define GIT_VERSION_LONG            \"" + git_version_long + "\"")

f.write("""

#endif /* GIT_VERSION_HPP_ */

""")

f.close()