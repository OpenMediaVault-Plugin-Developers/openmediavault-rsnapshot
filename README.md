openmediavault-rsnapshot
========================

Lesen:

http://wiki.ubuntuusers.de/Grundlagen_der_Paketerstellung

http://en.wikipedia.org/wiki/Debian_build_toolchain

Installieren:

sudo apt-get install build-essential debhelper dh-make quilt fakeroot lintian 
sudo apt-get install python-pip devscripts
pip install transifex-client

Dann dieses repo clonen.

Im Modulverzeichnis aufrufen:

dpkg-buildpackage -us -uc 

Plugin installieren!

Installation fehlgeschlagen/hängt? vermutlich konnte der omv-engined nicht starten:

/usr/sbin/omv-engined -fd


Debian Package tools
======