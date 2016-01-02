/**
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author    Julian Kalinowski <julakali@gmail.com>
 * @copyright Copyright (c) 2013-2014 Julian Kalinowski
 * @copyright Copyright (c) 2015-2016 OpenMediaVault Plugin Developers
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
// require("js/omv/WorkspaceManager.js")

OMV.WorkspaceManager.registerNode({
    id      : "rsnapshot",
    path    : "/service",
    text    : _("Rsnapshot"),
    icon16  : "images/rsnapshot.png",
    iconSvg : "images/rsnapshot.svg"
});
