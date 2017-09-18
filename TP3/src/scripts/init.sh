sudo apt-get install lighttpd
chown www-data /var/www/html
echo "www-data ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers
sudo cp -r /home/pi/GIT/SOII/src/html /var/www/html
cd /var/www/html/cgi-bin
make
cd ..
sudo lighty-enable-mode cgi