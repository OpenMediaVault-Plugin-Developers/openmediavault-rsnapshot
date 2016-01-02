<?php
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
require_once("openmediavault/object.inc");
require_once("openmediavault/config.inc");
require_once("openmediavault/error.inc");
require_once("openmediavault/rpcservice.inc");
require_once("openmediavault/notify.inc");

/**
* So this RPC service gets registered upon startup of openmediavault-engined.
* Any errors while starting engined originate from this file :(
*/
class OMVRpcServiceRSnapshot extends OMVRpcServiceAbstract {
    /**
     * Get the RPC service name.
     */
    public function getName() {
        return "RSnapshot";
    }

    /**
     * Initialize the RPC service.
     */
    public function initialize() {
        $this->registerMethod("getList");
        $this->registerMethod("get");
        $this->registerMethod("set");
        $this->registerMethod("delete");
        $this->registerMethod("execute");
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
    public function getList($params, $context) {
        global $xmlConfig;
        // Validate the RPC caller context.
        $this->validateMethodContext($context, array(
              "role" => OMV_ROLE_ADMINISTRATOR
          ));
        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "start"     : { "type" : "integer" },
                "limit"     : { '.$GLOBALS['OMV_JSONSCHEMA_COUNTFIELD'].' },
                "sortfield" : { '.$GLOBALS['OMV_JSONSCHEMA_SORTFIELD'].' },
                "sortdir"   : { '.$GLOBALS['OMV_JSONSCHEMA_SORTDIR'].' }
            }
        }');

        // Get the configuration objects.
        $xpath = "/config/services/rsnapshot/jobs/job";
        $objects = $xmlConfig->getList($xpath);
        if(is_null($objects)) {
            throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
              $xpath);
        }
        // Add additional source/target informations.
        foreach($objects as $objectk => &$objectv) {
            $objectv['sourcefoldername'] = gettext("n/a");
            $objectv['targetfoldername'] = gettext("n/a");
            // Get the source folder configuration object.
            $sfobject = $xmlConfig->get("//system/shares/sharedfolder[" .
              "uuid='{$objectv['sourcefolderref']}']");
            if(!is_null($sfobject) && !empty($sfobject)) {
                $objectv['sourcefoldername'] = $sfobject['name'];
            }
            // Get the target folder configuration object
            $sfobject = $xmlConfig->get("//system/shares/sharedfolder[" .
              "uuid='{$objectv['targetfolderref']}']");
            if(!is_null($sfobject) && !empty($sfobject)) {
                $objectv['targetfoldername'] = $sfobject['name'];
            }
            // Get job status.
            $objectv['running'] = file_exists(sprintf("/var/run/rsnapshot-%s",
              $objectv['uuid']));
            // Modify result object.
            $objectv['enable']   = boolval($objectv['enable']);
            $objectv['numtries'] = intval($objectv['numtries']);
            $objectv['onefs']    = boolval($objectv['onefs']);
        }
        // Filter the result.
        return $this->applyFilter($objects, $params['start'],
          $params['limit'], $params['sortfield'], $params['sortdir']);
    }

    /**
     * Get a RSnapshot backup job configuration object.
     * @param params An array containing the following fields:
     *   \em uuid The UUID of the configuration object.
     * @param context The context of the caller.
     * @return The requested configuration object.
     * @throw E_CONFIG_GET_OBJECT_FAILED
     */
    function get($params, $context) {
        global $xmlConfig;
        // Validate the RPC caller context.
        $this->validateMethodContext($context, array(
              "role" => OMV_ROLE_ADMINISTRATOR
          ));
        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "uuid" : { '.$GLOBALS['OMV_JSONSCHEMA_UUID'].' }
            }
        }');
        // Get the configuration object.
        $xpath = sprintf("//services/rsnapshot/jobs/job[uuid='%s']",
          $params['uuid']);
        $object = $xmlConfig->get($xpath);
        if(is_null($object)) {
            throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
              $xpath);
        }
        // Modify result object.
        $object['enable']   = boolval($object['enable']);
        $object['hourly']   = intval($object['hourly']);
        $object['daily']    = intval($object['daily']);
        $object['weekly']   = intval($object['weekly']);
        $object['monthly']  = intval($object['monthly']);
        $object['yearly']   = intval($object['yearly']);
        $object['numtries'] = intval($object['numtries']);
        $object['onefs']    = boolval($object['onefs']);

        return $object;
    }

    /**
     * Set a RSnapshot backup job configuration object.
     * @param params The method parameters.
     * @param context The context of the caller.
     * @return The stored configuration object.
     * @throw E_CONFIG_OBJECT_UNIQUENESS
     * @throw E_CONFIG_SET_OBJECT_FAILED
     */
    function set($params, $context) {
        global $xmlConfig;
        // Validate the RPC caller context.
        $this->validateMethodContext($context, array(
              "role" => OMV_ROLE_ADMINISTRATOR
          ));
        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type":"object",
            "properties":{
                "uuid"            : { '.$GLOBALS['OMV_JSONSCHEMA_UUID_UNDEFINED'].' },
                "enable"          : { "type" : "boolean" },
                "comment"         : { "type" : "string" },
                "targetfolderref" : { '.$GLOBALS['OMV_JSONSCHEMA_UUID'].' },
                "onefs"           : { "type" : "boolean" },
                "sourcefolderref" : { '.$GLOBALS['OMV_JSONSCHEMA_UUID'].' },
                "hourly"          : { "type" : "integer", "minimum" : 0, "maximum" : 50 },
                "daily"           : { "type" : "integer", "minimum" : 0, "maximum" : 50 },
                "weekly"          : { "type" : "integer", "minimum" : 0, "maximum" : 50 },
                "monthly"         : { "type" : "integer", "minimum" : 0, "maximum" : 50 },
                "yearly"          : { "type" : "integer", "minimum" : 0, "maximum" : 50 },
                "numtries"        : { "type" : "integer", "minimum" : 1, "maximum" : 65535 },
                "gid"             : { "type" : "string"},
                "rsyncargs"       : { "type" : "string"}
            }
        }');
        // Prepare the configuration object.
        $object = array(
            "uuid"            => ($params['uuid'] == $GLOBALS['OMV_UUID_UNDEFINED']) ?
              OMVUtil::uuid() : $params['uuid'],
            "enable"          => array_boolval($params, 'enable'),
            "comment"         => $params['comment'],
            "targetfolderref" => $params['targetfolderref'],
            "onefs"           => array_boolval($params, 'onefs'),
            "sourcefolderref" => $params['sourcefolderref'],
            "hourly"          => $params['hourly'],
            "daily"           => $params['daily'],
            "weekly"          => $params['weekly'],
            "monthly"         => $params['monthly'],
            "yearly"          => $params['yearly'],
            "numtries"        => $params['numtries'],
            "gid"             => $params['gid'],
            "rsyncargs"       => $params['rsyncargs']
        );
        // Set the configuration object.
        $success = FALSE;
        if($params['uuid'] == $GLOBALS['OMV_UUID_UNDEFINED']) {
            // Append object to configuration.
            $success = $xmlConfig->set("//services/rsnapshot/jobs",
              array("job" => $object));
        } else {
            // Update existing configuration object.
            $success = $xmlConfig->replace("//services/rsnapshot/jobs/job[" .
              "uuid='{$params['uuid']}']", $object);
        }
        if(FALSE === $success) {
            throw new OMVException(OMVErrorMsg::E_CONFIG_SET_OBJECT_FAILED);
        }
        // Notify configuration changes.
        $dispatcher = &OMVNotifyDispatcher::getInstance();
        $dispatcher->notify(($params['uuid'] == $GLOBALS['OMV_UUID_UNDEFINED']) ?
          OMV_NOTIFY_CREATE : OMV_NOTIFY_MODIFY,
          "org.openmediavault.services.rsnapshot.jobs.job", $object);
        // Return the configuration object.
        return $object;
    }

    /**
     * Delete a backup job configuration object.
     * @param params An array containing the following fields:
     *   \em uuid The UUID of the configuration object.
     * @param context The context of the caller.
     * @return The deleted configuration object.
     */
    public function delete($params, $context) {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, array(
              "role" => OMV_ROLE_ADMINISTRATOR
          ));
        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "uuid" : { '.$GLOBALS['OMV_JSONSCHEMA_UUID'].' }
            }
        }');
        // Delete the configuration object.
        $xpath = sprintf("//services/rsnapshot/jobs/job[uuid='%s']",
          $params['uuid']);
        return $this->deleteConfigObjectByPath($xpath,
          "org.openmediavault.services.rsnapshot.jobs.job");
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
    public function execute($params, $context) {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, array(
              "role" => OMV_ROLE_ADMINISTRATOR
          ));
        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "uuid" : { '.$GLOBALS['OMV_JSONSCHEMA_UUID'].' }
            }
        }');
        // Check if the module is marked as dirty. This is an indication
        // that the rsync cron script has not been created or updated
        // until now.
        if($this->isModuleDirty("rsnapshot")) {
            throw new OMVException(OMVErrorMsg::E_CONFIG_DIRTY);
        }

        // get object
        $object = $this->get($params, $context);

        // Create a background process.
        $bgStatusFilename = $this->createBgProcStatus();
        $pid = $this->fork();
        if($pid > 0) { // Parent process.
            $this->initializeBgProcStatus($bgStatusFilename, $pid);
            return $bgStatusFilename;
        }

        // Determine backup level.
        // Will use the smallest backup cycle upon manual execution.
        $level = "unknown";
        if ($object['hourly'] != 0) {
            $level = "hourly";
        } elseif ($object['daily'] != 0) {
            $level = "daily";
        } elseif ($object['weekly'] != 0) {
            $level = "weekly";
        } elseif ($object['monthly'] != 0) {
            $level = "monthly";
        } elseif ($object['yearly'] != 0) {
            $level = "yearly";
        }

        // Child process.
        try {
            $bgOutputFilename = $this->createBgProcOutput();
            $this->updateBgProcStatus($bgStatusFilename, "outputfilename",
              $bgOutputFilename);
            // Execute the backup script.
            $cmd = "/bin/sh";
            $cmdArgs = array($GLOBALS['OMV_RSNAPSHOT_CRONSCRIPT'], $level, $params['uuid']);
            if(FALSE === $this->execve($cmd, $cmdArgs, $bgOutputFilename)) {
                throw new OMVException(OMVErrorMsg::E_EXEC_FAILED,
                  $cmd." ".implode(" ", $cmdArgs), "");
            }
        } catch(Exception $e) {
            $this->finalizeBgProcStatus($bgStatusFilename, "", $e);
            exit(1);
        }
    }
}

// Register the RPC service.
$rpcServiceMgr = &OMVRpcServiceMgr::getInstance();
$rpcServiceMgr->registerService(new OMVRpcServiceRSnapshot());
