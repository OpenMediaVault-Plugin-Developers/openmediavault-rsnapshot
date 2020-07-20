<?php
/**
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author    Julian Kalinowski <julakali@gmail.com>
 * @copyright Copyright (c) 2013-2014 Julian Kalinowski
 * @copyright Copyright (c) 2015-2020 OpenMediaVault Plugin Developers
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
require_once('openmediavault/globals.inc');
require_once('openmediavault/functions.inc');

class OMVRpcServiceRsnapshot extends \OMV\Rpc\ServiceAbstract {
    /**
     * Get the RPC service name.
     */
    public function getName()
    {
        return 'RSnapshot';
    }

    /**
     * Initialize the RPC service.
     */
    public function initialize()
    {
        $this->registerMethod('getList');
        $this->registerMethod('get');
        $this->registerMethod('set');
        $this->registerMethod('delete');
        $this->registerMethod('execute');
    }

    /**
     * Get list of RSnapshot backup job configuration objects.
     * @param data An array containing the following fields:
     *   \em start The index where to start.
     *   \em limit The number of objects to process.
     *   \em sortfield The name of the column used to sort.
     *   \em sortdir The sort direction, ASC or DESC.
     * @return An array containing the requested objects. The field \em total
     *   contains the total number of objects, \em data contains the object
     *   array. An exception will be thrown in case of an error.
     * @throw E_CONFIG_GET_OBJECT_FAILED
     */
    public function getList($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ['role' => OMV_ROLE_ADMINISTRATOR]);
        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, 'rpc.common.getlist');
        // Get configuration objects.
        $db = \OMV\Config\Database::getInstance();
        $objects = $db->get('conf.service.rsnapshot');
        // Add additional information.
        $objectsAssoc = [];
        foreach ($objects as $objectk => &$objectv) {
            // Add the new property 'sharedfoldername'.
            $objectv->add('sourcefoldername', 'string', gettext("n/a"));
            $objectv->add('targetfoldername', 'string', gettext("n/a"));
            $sfObject = $db->get('conf.system.sharedfolder', $objectv->get('src.sharedfolderref'));
            $objectv->set('sourcefoldername', $sfObject->get('name'));
            $sfObject = $db->get('conf.system.sharedfolder', $objectv->get('dest.sharedfolderref'));
            $objectv->set('targetfoldername', $sfObject->get('name'));
            $objectv->add('running', file_exists(sprintf('/var/run/rsnapshot-%s', $objectv->get('uuid'))));
            $objectsAssoc[] = $objectv->getAssoc();
        }

        // Filter the result.
        return $this->applyFilter($objectsAssoc, $params['start'], $params['limit'],
            $params['sortfield'], $params['sortdir']);
    }

    /**
     * Get a RSnapshot backup job configuration object.
     * @param params An array containing the following fields:
     *   \em uuid The UUID of the configuration object.
     * @param context The context of the caller.
     * @return The requested configuration object.
     * @throw E_CONFIG_GET_OBJECT_FAILED
     */
    function get($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ['role' => OMV_ROLE_ADMINISTRATOR]);
        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, 'rpc.common.objectuuid');
        // Get the configuration object.
        $db = \OMV\Config\Database::getInstance();
        $object = $db->get('conf.service.rsnapshot', $params['uuid']);
        $object->copy('src.sharedfolderref', 'srcsharedfolderref');
        $object->copy('dest.sharedfolderref', 'destsharedfolderref');
        return $object->getAssoc();
    }

    /**
     * Set a RSnapshot backup job configuration object.
     * @param params The method parameters.
     * @param context The context of the caller.
     * @return The stored configuration object.
     * @throw E_CONFIG_OBJECT_UNIQUENESS
     * @throw E_CONFIG_SET_OBJECT_FAILED
     */
    function set($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ['role' => OMV_ROLE_ADMINISTRATOR]);
        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, 'rpc.rsnapshot.set');
        // Prepare the configuration data.
        $object = new \OMV\Config\ConfigObject('conf.service.rsnapshot');
        $object->setAssoc($params, TRUE, TRUE);
        $object->set('src.sharedfolderref', $params['srcsharedfolderref']);
        $object->set('dest.sharedfolderref', $params['destsharedfolderref']);
        // Set the configuration object.
        $db = \OMV\Config\Database::getInstance();
        $db->set($object);
        // Return the configuration object.
        return $object->getAssoc();
    }

    /**
     * Delete a backup job configuration object.
     * @param params An array containing the following fields:
     *   \em uuid The UUID of the configuration object.
     * @param context The context of the caller.
     * @return The deleted configuration object.
     */
    public function delete($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ['role' => OMV_ROLE_ADMINISTRATOR]);
        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, 'rpc.common.objectuuid');
        // Delete the configuration object.
        $db = \OMV\Config\Database::getInstance();
        $object = $db->get('conf.service.rsnapshot', $params['uuid']);
        $db->delete($object);
        // Return the deleted configuration object.
        return $object->getAssoc();
    }

    /**
     * Execute a backup job.
     * @param params An array containing the following fields:
     *   \em uuid The UUID of the cron job to execute.
     *   \em level The backup level to execute.
     * @param context The context of the caller.
     * @return The name of the background process status file.
     * @throw E_CONFIG_DIRTY
     */
    public function execute($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ['role' => OMV_ROLE_ADMINISTRATOR]);
        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, 'rpc.common.objectuuid');
        // Check if the module is marked as dirty. This is an indication
        // that the rsync cron script has not been created or updated
        // until now.
        if ($this->isModuleDirty('rsnapshot'))
            throw new \OMV\Config\ConfigDirtyException();
        // Get the configuration object.
        $db = \OMV\Config\Database::getInstance();
        $object = $db->get('conf.service.rsnapshot', $params['uuid']);
        // Determine backup level.
        // Will use the smallest backup cycle upon manual execution.
        $level = 'unknown';
        if ($object->get('hourly') != 0) {
            $level = 'hourly';
        } elseif ($object->get('daily') != 0) {
            $level = 'daily';
        } elseif ($object->get('weekly') != 0) {
            $level = 'weekly';
        } elseif ($object->get('monthly') != 0) {
            $level = 'monthly';
        } elseif ($object->get('yearly') != 0) {
            $level = 'yearly';
        }
        // Create the background process.
        return $this->execBgProc(function($bgStatusFilename, $bgOutputFilename)
            use ($params, $level) {
                $cmdArgs = [];
                $cmdArgs[] = $GLOBALS['OMV_RSNAPSHOT_CRONSCRIPT'];
                $cmdArgs[] = $level;
                $cmdArgs[] = $params['uuid'];
                $cmd = new \OMV\System\Process('/bin/sh', $cmdArgs);
                $cmd->setRedirect2to1();
                $cmdLine = $cmd->getCommandLine();
                if (0 !== $this->exec($cmdLine, $output, $bgOutputFilename))
                    throw new \OMV\ExecException($cmdLine, $output);
                return $output;
            }
        );
    }
}
