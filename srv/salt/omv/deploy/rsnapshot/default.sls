# @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
# @author    OpenMediaVault Plugin Developers <plugins@omv-extras.org>
# @copyright Copyright (c) 2019-2026 openmediavault plugin developers
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

{% set config = salt['omv_conf.get']('conf.service.rsnapshot') %}

{% set confs_dir = salt['pillar.get']('default:OMV_RSNAPSHOT_CONFS_DIR', '/var/lib/openmediavault/rsnapshot.d') %}
{% set cronscript = salt['pillar.get']('default:OMV_RSNAPSHOT_CRONSCRIPT', '/var/lib/openmediavault/cron.d/rsnapshot') %}
{% set scripts_mask = salt['pillar.get']('default:OMV_RSNAPSHOT_SCRIPTS_MASK', '755') %}
{% set dirs_mask = salt['pillar.get']('default:OMV_RSNAPSHOT_DIRS_MASK', '2775') %}
{% set start_hour = salt['pillar.get']('default:OMV_RSNAPSHOT_STARTING_HOUR', '2') %}
{% set disable_cron = salt['pillar.get']('default:OMV_RSNAPSHOT_DISABLE_CRON', '0') | to_bool %}
{% set logfile = salt['pillar.get']('default:OMV_RSNAPSHOT_LOGFILE', '/var/log/rsnapshot.log') %}
{% set verbose_lo = salt['pillar.get']('default:OMV_RSNAPSHOT_VERBOSE_LO', '2') %}
{% set verbose_hi = salt['pillar.get']('default:OMV_RSNAPSHOT_VERBOSE_HI', '3') %}
{% set log_level = salt['pillar.get']('default:OMV_RSNAPSHOT_LOG_LEVEL', '2') %}

rsnapshot_confs_dir:
  file.directory:
    - name: {{ confs_dir }}
    - user: root
    - group: root
    - dir_mode: 0755
    - makedirs: True

{% if not disable_cron %}
openmediavault_rsnapshot_cron:
  file.managed:
    - name: /etc/cron.d/openmediavault-rsnapshot
    - user: root
    - group: root
    - mode: 0644
    - contents: |
        {{ pillar['headers']['auto_generated'] }}
        {{ pillar['headers']['warning'] }}
        # m h dom mon dow user  command
        5 * * * * root  {{ cronscript }} hourly
        30 {{ (start_hour|int + 1) }} * * * root  {{ cronscript }} daily
        0 {{ (start_hour|int + 1) }} * * 1 root  {{ cronscript }} weekly
        30 {{ start_hour|int }} 1 * * root  {{ cronscript }} monthly
        0 {{ start_hour|int }} 1 1 * root  {{ cronscript }} yearly
{% else %}
openmediavault_rsnapshot_cron_absent:
  file.absent:
    - name: /etc/cron.d/openmediavault-rsnapshot
{% endif %}

{% set keep_confs = [] %}
{% for job in config.jobs.job %}
  {% set uuid = job.uuid %}
  {% if not uuid %}{% continue %}{% endif %}
rsnapshot_job_conf_{{ uuid|replace('-', '_') }}:
  file.managed:
    - name: {{ confs_dir }}/rsnapshot-{{ uuid }}.conf
    - user: root
    - group: root
    - mode: {{ scripts_mask }}
    - template: jinja
    - source: salt://omv/deploy/rsnapshot/files/rsnapshot-job_conf.j2
    - context:
        job: {{ job|json }}
        uuid: {{ uuid }}
        logfile: {{ logfile }}
        verbose_lo: {{ verbose_lo }}
        verbose_hi: {{ verbose_hi }}
        log_level: {{ log_level }}
    - require:
      - file: rsnapshot_confs_dir
  {% do keep_confs.append('rsnapshot-' ~ uuid ~ '.conf') %}
{% endfor %}

rsnapshot_confs_tidied:
  file.tidied:
    - name: {{ confs_dir }}
    - matches:
      - "rsnapshot-.*\\.conf"
    - exclude:
{%- for f in keep_confs %}
      - "{{ f }}"
{%- endfor %}
    - rmdirs: False
    - rmlinks: True

# The single cron dispatch script
rsnapshot_cronscript:
  file.managed:
    - name: {{ cronscript }}
    - user: root
    - group: root
    - mode: {{ scripts_mask }}
    - template: jinja
    - source: salt://omv/deploy/rsnapshot/files/rsnapshot-cron_sh.j2
    - context:
        config: {{ config | json }}
        confs_dir: {{ confs_dir }}
        dirs_mask: {{ dirs_mask }}
        logfile: {{ logfile }}
        verbose_lo: {{ verbose_lo }}
        verbose_hi: {{ verbose_hi }}
        log_level: {{ log_level }}
    - require:
      - file: rsnapshot_confs_dir
