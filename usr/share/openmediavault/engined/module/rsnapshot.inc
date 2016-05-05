<?php
/**
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author    OpenMediaVault Plugin Developers <plugins@omv-extras.org>
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
class OMVModuleRSnapshot extends \OMV\Engine\Module\ServiceAbstract
    implements \OMV\Engine\Notify\IListener {
    /**
     * Get the module name.
     */
    public function getName()
    {
        return "rsnapshot";
    }

    /**
     * Generate the configuration.
     * @throw E_EXEC_FAILED
     */
    public function applyConfig()
    {
        $cmd = new \OMV\System\Process("omv-mkconf", "rsnapshot");
        $cmd->setRedirect2to1(TRUE);
        $cmd->execute();
    }

    /**
     * Helper function to find out whether the given shared folder
     * configuration object is used. If it is used, then mark the
     * module as dirty.
     * @param type The event message type.
     * @param path The event message path.
     * @param object The configuration object.
     */
    final public function onModifySharedFolder($type, $path, $object)
    {
        $db = \OMV\Config\Database::getInstance();
        $object = $db->get("conf.service.rsnapshot.job");
        if (TRUE === $object->isEqual("sourcefolderref", $object['uuid']))
            $this->setDirty();
        if (TRUE === $object->isEqual("targetfolderref", $object['uuid']))
            $this->setDirty();
    }

    /**
     * Bind listeners.
     */
    function bindListeners(\OMV\Engine\Notify\Dispatcher $dispatcher) {
        $dispatcher->addListener(
            OMV_NOTIFY_CREATE | OMV_NOTIFY_MODIFY | OMV_NOTIFY_DELETE,
            "org.openmediavault.conf.service.rsnapshot.job",
            [ $this, "setDirty" ]
        );
        $dispatcher->addListener(
            OMV_NOTIFY_MODIFY,
            "org.openmediavault.conf.system.sharedfolder",
            [ $this, "onModifySharedFolder" ]
        );
    }
}
