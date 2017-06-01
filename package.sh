set -e
rm -rf  package
source version.sh

#AMD64
mkdir -p package/riaps-timesync-amd64/DEBIAN
mkdir -p package/riaps-timesync-amd64/opt/riaps/armhf/lib/
cp DEBIAN/riaps-timesync-amd64.control package/riaps-timesync-amd64/DEBIAN/control
cp DEBIAN/postinst package/riaps-timesync-amd64/DEBIAN/postinst
cp DEBIAN/postrm package/riaps-timesync-amd64/DEBIAN/postrm
cp -r amd64-opt/* package/riaps-timesync-amd64/.
cp -r armhf-opt/opt/riaps/armhf/lib/* package/riaps-timesync-amd64/opt/riaps/armhf/lib/.
sed s/@version@/$timesyncversion/g -i package/riaps-timesync-amd64/DEBIAN/control
fakeroot dpkg-deb --build package/riaps-timesync-amd64/

#ARMHF
mkdir -p package/riaps-timesync-armhf/DEBIAN
mkdir -p package/riaps-timesync-armhf/opt/riaps/armhf/lib/
cp DEBIAN/riaps-timesync-armhf.control package/riaps-timesync-armhf/DEBIAN/control
cp DEBIAN/postinst package/riaps-timesync-armhf/DEBIAN/postinst
cp DEBIAN/postrm package/riaps-timesync-armhf/DEBIAN/postrm
cp -r armhf-opt/* package/riaps-timesync-armhf/.
sed s/@version@/$timesyncversion/g -i package/riaps-timesync-armhf/DEBIAN/control
fakeroot dpkg-deb --build package/riaps-timesync-armhf/

