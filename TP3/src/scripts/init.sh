sudo apt-get install lighttpd
chown www-data /var/html
echo "www-data ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers
sudo cp -r /home/pi/GIT/SOII/src/html /var/html
cd /var/html/cgi-bin
make
cd ..
sudo lighty-enable-mode cgi