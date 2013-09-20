openmediavault-rsnapshot
========================

Lesen:

http://wiki.ubuntuusers.de/Grundlagen_der_Paketerstellung

http://en.wikipedia.org/wiki/Debian_build_toolchain

Installieren:

sudo apt-get install build-essential debhelper dh-make quilt fakeroot lintian 
sudo apt-get install python-pip
pip install transifex-client

Auschecken:

svn checkout svn://svn.code.sf.net/p/openmediavault/code/trunk openmediavault-code

Dann dieses repo clonen nach openmediavault-code.

Im Modulverzeichnis aufrufen:

dpkg-buildpackage -us -uc 

