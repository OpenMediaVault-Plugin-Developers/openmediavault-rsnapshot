#!/bin/sh
#
# @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
# @author    Volker Theile <volker.theile@openmediavault.org>
# @author    OpenMediaVault Plugin Developers <plugins@omv-extras.org>
# @copyright Copyright (c) 2009-2013 Volker Theile
# @copyright Copyright (c) 2013-2026 openmediavault plugin developers
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

set -e

. /usr/share/openmediavault/scripts/helper-functions

XPATH="/config/services/rsnapshot/jobs/job"

# Modify the model of the rsync jobs.
xmlstarlet sel -t -m "${XPATH}" \
    -v "uuid" -n \
    ${OMV_CONFIG_FILE} | xmlstarlet unesc |
    while read uuid; do
        # cmd_preexec
        if ! omv_config_exists "${XPATH}[uuid='${uuid}']/cmd_preexec"; then
            omv_config_add_key "${XPATH}[uuid='${uuid}']" "cmd_preexec"
        fi
        # cmd_postexec
        if ! omv_config_exists "${XPATH}[uuid='${uuid}']/cmd_postexec"; then
            omv_config_add_key "${XPATH}[uuid='${uuid}']" "cmd_postexec"
        fi
    done

exit 0
