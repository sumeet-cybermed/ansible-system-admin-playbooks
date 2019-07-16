cd /var/downloads
mkdir -p xeams
cd xeams
wget http://www.xeams.com/files/XeamsLinux64.tar
tar -xf XeamsLinux64.tar
chmod +x ./Install.sh
./Install.sh

service xeams start
systemctl enable xeams
service xeams start
service xeams status


firewall-cmd --zone=public --add-port=5272/tcp --permanent

cd /tmp
wget http://localhost:5272