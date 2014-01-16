/**
*
* @license http://www.gnu.org/licenses/gpl.html GPL Version 3
* @author    Julian Kalinowski <julakali@gmail.com>
* @copyright Copyright (c) 2014 Julian Kalinowski
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
// require("js/omv/PluginManager.js")
// require("js/omv/module/admin/diagnostic/log/plugin/Plugin.js")
// require("js/omv/util/Format.js")

Ext.define("OMV.module.admin.diagnostic.log.plugin.Rsnapshot", {
    extend : "OMV.module.admin.diagnostic.log.plugin.Plugin",

    id : "rsnapshot",
    text : _("Rsnapshot (Backup)"),
    stateful : true,
    stateId : "e61b7810-7ee4-11e3-baa7-0800200c9a66",
    columns : [{
        text : _("Date & Time"),
        sortable : true,
        dataIndex : "date",
        id : "date",
        renderer : OMV.util.Format.localeTimeRenderer()
    },{
        text : _("Message"),
        sortable : true,
        dataIndex : "message",
        id : "message"
    }],
    rpcParams : {
        id: "rsnapshot"
    },
    rpcFields : [
        { name : "date", type: "string" },
        { name : "message", type: "string" }
    ]
});

OMV.PluginManager.register({
    ptype: "diagnostic",
    id: "log",
    className: "OMV.module.admin.diagnostic.log.plugin.Rsnapshot"
});