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
// require("js/omv/workspace/grid/Panel.js")
// require("js/omv/workspace/window/Form.js")
// require("js/omv/workspace/window/plugin/ConfigObject.js")
// require("js/omv/Rpc.js")
// require("js/omv/data/Store.js")
// require("js/omv/data/Model.js")
// require("js/omv/data/proxy/Rpc.js")
// require("js/omv/window/Execute.js")
// require("js/omv/form/field/GroupComboBox.js")
// require("js/omv/form/field/SharedFolderComboBox.js")

/**
 * @class OMV.module.admin.service.rsnapshot.Job
 * @derived OMV.workspace.window.Form
 */
Ext.define("OMV.module.admin.service.rsnapshot.Job", {
    extend   : "OMV.workspace.window.Form",
    requires : [
        "OMV.data.Store",
        "OMV.data.Model",
        "OMV.data.proxy.Rpc",
        "OMV.form.field.SharedFolderComboBox",
        "OMV.form.field.GroupComboBox"
    ],
    uses     : [
        "OMV.workspace.window.plugin.ConfigObject"
    ],

    rpcService   : "RSnapshot",
    rpcGetMethod : "get",
    rpcSetMethod : "set",
    plugins      : [{
        ptype: "configobject"
    }],
    width: 570,
    height: 400,

    /**
     * The class constructor.
     * @fn constructor
     * @param uuid The UUID of the database/configuration object. Required.
     */

    getFormItems : function() {
        var me = this;
        return [{
            xtype      : "checkbox",
            name       : "enable",
            fieldLabel : _("Enable"),
            checked    : true
        },{
            xtype      : "textfield",
            name       : "comment",
            fieldLabel : _("Comment"),
            allowBlank : true,
            vtype      : "comment"
        },{
            xtype      : "sharedfoldercombo",
            name       : "sourcefolderref",
            fieldLabel : _("Source folder"),
            plugins    : [{
                ptype : "fieldinfo",
                text  : _("The shared folder to Backup.")
            }]
        },{
            xtype      : "checkbox",
            name       : "onefs",
            fieldLabel : "&nbsp",
            checked    : false,
            inputValue : 1,
            boxLabel   : _("One filesystem only"),
            plugins    : [{
                ptype : "fieldinfo",
                text  : _("If this is enabled, rsync won't span filesystem partitions within a backup point.")
            }]
        },{
            xtype      : "sharedfoldercombo",
            name       : "targetfolderref",
            fieldLabel : _("Target folder"),
            plugins    : [{
                ptype : "fieldinfo",
                text  : _("The Folder used as target for backups. Backups will go into a sub-folder <em>localhost/&lt;label of source filesystem&gt;/&lt;source folder relative path&gt;</em>")
            }]
        },{
            xtype         : "numberfield",
            name          : "hourly",
            fieldLabel    : _("Retain hourly"),
            minValue      : 0,
            allowDecimals : false,
            allowBlank    : false,
            value         : 0,
            plugins       : [{
                ptype : "fieldinfo",
                text  : _("Number of hourly backups to keep. After n backups are created, the first one will be overwritten.")
            }]
        },{
            xtype         : "numberfield",
            name          : "daily",
            fieldLabel    : _("Retain daily"),
            minValue      : 0,
            allowDecimals : false,
            allowBlank    : false,
            value         : 7,
            plugins       : [{
                ptype : "fieldinfo",
                text  : _("Number of daily backups to keep. After n backups are created, the first one will be overwritten.")
            }]
        },{
            xtype         : "numberfield",
            name          : "weekly",
            fieldLabel    : _("Retain weekly"),
            minValue      : 0,
            allowDecimals : false,
            allowBlank    : false,
            value         : 4,
            plugins       : [{
                ptype : "fieldinfo",
                text  : _("Number of weekly backups to keep. After n backups are created, the first one will be overwritten.")
            }]
        },{
            xtype         : "numberfield",
            name          : "monthly",
            fieldLabel    : _("Retain monthly"),
            minValue      : 0,
            allowDecimals : false,
            allowBlank    : false,
            value         : 3,
            plugins       : [{
                ptype : "fieldinfo",
                text  : _("Number of monthly backups to keep. After n backups are created, the first one will be overwritten.")
            }]
        },{
            xtype         : "numberfield",
            name          : "yearly",
            fieldLabel    : _("Retain yearly"),
            minValue      : 0,
            allowDecimals : false,
            allowBlank    : false,
            value         : 0,
            plugins       : [{
                ptype : "fieldinfo",
                text  : _("Number of yearly backups to keep. After n backups are created, the first one will be overwritten.")
            }]
        },{
            xtype         : "numberfield",
            name          : "numtries",
            fieldLabel    : _("Rsync tries"),
            minValue      : 1,
            allowDecimals : false,
            allowBlank    : false,
            value         : 1,
            plugins       : [{
                ptype : "fieldinfo",
                text  : _("Number of rsync tries. If you experience any network problems or network card issues that tend to cause ssh to crap-out with 'Corrupted MAC on input' errors, for example, set this to a higher value to have the rsync operation re-tried")
            }]
        },{
            xtype      : "groupcombo",
            name       : "gid",
            fieldLabel : _("Group"),
            value      : "users",
            plugins    : [{
                ptype : "fieldinfo",
                text  : _("This option specifies the group that the backup directories will belong to. Note: This will only change the group of additionally created dirs. The backed-up files and directories will mirror the original permissions.")
            }]
        },{
            xtype      : "textfield",
            name       : "rsyncargs",
            fieldLabel : _("Rsync long arguments"),
            allowBlank : true,
            plugins    : [{
                ptype : "fieldinfo",
                text  : _("Default rsync long args. All rsync commands have at least these options set. Default is '--delete --numeric-ids --delete-excluded'. Please check the <a href='http://www.samba.org/ftp/rsync/rsync.html' target='_blank'>manual page</a> for more details.")
            }]
        }];
    }
});

/**
 * @class OMV.module.admin.service.rsnapshot.Jobs
 * @derived OMV.workspace.grid.Panel
 */
Ext.define("OMV.module.admin.service.rsnapshot.Jobs", {
    extend   : "OMV.workspace.grid.Panel",
    requires : [
        "OMV.Rpc",
        "OMV.data.Store",
        "OMV.data.Model",
        "OMV.data.proxy.Rpc",
        "OMV.window.Execute"
    ],
    uses     : [
        "OMV.module.admin.service.rsnapshot.Job"
    ],

    hidePagingToolbar : false,
    stateful          : true,
    stateId           : "693bddb2-7765-11e2-8c62-00221568ca88",
    columns           : [{
        xtype     : "booleaniconcolumn",
        text      : _("Enabled"),
        sortable  : true,
        dataIndex : "enable",
        stateId   : "enable",
        align     : "center",
        width     : 80,
        resizable : false,
        trueIcon  : "switch_on.png",
        falseIcon : "switch_off.png"
    },{
        text      : _("Source"),
        sortable  : true,
        dataIndex : "sourcefoldername",
        stateId   : "sourcefoldername"
    },{
        text      : _("Target"),
        sortable  : true,
        dataIndex : "targetfoldername",
        stateId   : "targetfoldername"
    },{
        text      : _("Comment"),
        sortable  : true,
        dataIndex : "comment",
        stateId   : "comment",
        flex      : 1
    }],

    initComponent : function() {
        var me = this;
        Ext.apply(me, {
            store : Ext.create("OMV.data.Store", {
                autoLoad : true,
                model    : OMV.data.Model.createImplicit({
                    idProperty  : "uuid",
                    fields      : [
                        { name : "uuid", type : "string" },
                        { name : "enable", type : "boolean" },
                        { name : "sourcefoldername", type : "string" },
                        { name : "targetfoldername", type : "string" },
                        { name : "comment", type : "string" },
                        { name : "running", type : "boolean" }
                    ]
                }),
                proxy : {
                    type    : "rpc",
                    rpcData : {
                        service : "RSnapshot",
                        method  : "getList"
                    }
                }
            })
        });
        me.callParent(arguments);
    },

    getTopToolbarItems : function() {
        var me = this;
        var items = me.callParent(arguments);
        // Add 'Run' button to top toolbar
        Ext.Array.insert(items, 2, [{
            id       : me.getId() + "-run",
            xtype    : "button",
            text     : _("Run"),
            icon     : "images/play.png",
            iconCls  : Ext.baseCSSPrefix + "btn-icon-16x16",
            handler  : Ext.Function.bind(me.onRunButton, me, [ me ]),
            scope    : me,
            disabled : true
        }]);
        return items;
    },

    onSelectionChange : function(model, records) {
        var me = this;
        me.callParent(arguments);
		// Process additional buttons.
		var tbarBtnDisabled = {
			"run" : true
		};
        if(records.length == 1) {
            tbarBtnDisabled["run"] = false;
        }
		// Update the button controls.
		Ext.Object.each(tbarBtnDisabled, function(key, value) {
			this.setToolbarButtonDisabled(key, value);
		}, me);
    },

    onAddButton : function() {
        var me = this;
        var record = me.getSelected();
        Ext.create("OMV.module.admin.service.rsnapshot.Job", {
            title     : _("Add backup job"),
            uuid      : OMV.UUID_UNDEFINED,
            listeners : {
                scope  : me,
                submit : function() {
                    this.doReload();
                }
            }
        }).show();
    },

    onEditButton : function() {
        var me = this;
        var record = me.getSelected();
        Ext.create("OMV.module.admin.service.rsnapshot.Job", {
            title     : _("Edit backup job"),
            uuid      : record.get("uuid"),
            listeners : {
                scope  : me,
                submit : function() {
                    this.doReload();
                }
            }
        }).show();
    },

    doDeletion : function(record) {
        var me = this;
        OMV.Rpc.request({
            scope    : me,
            callback : me.onDeletion,
            rpcData  : {
                service : "RSnapshot",
                method  : "delete",
                params  : {
                    uuid : record.get("uuid")
                }
            }
        });
    },

    onRunButton : function() {
        var me = this;
        var record = me.getSelected();

        Ext.create("OMV.window.Execute", {
            title      : _("Execute backup job"),
            rpcService : "RSnapshot",
            rpcMethod  : "execute",
            rpcParams  : {
                uuid : record.get("uuid")
            },
            listeners  : {
                scope     : me,
                exception : function(wnd, error) {
                    OMV.MessageBox.error(null, error);
                }
            }
        }).show();
    }
});

OMV.WorkspaceManager.registerPanel({
    id        : "jobs",
    path      : "/service/rsnapshot",
    text      : _("Backup jobs"),
    position  : 10,
    className : "OMV.module.admin.service.rsnapshot.Jobs"
});
