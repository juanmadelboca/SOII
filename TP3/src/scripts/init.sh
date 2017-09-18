sudo apt-get install lighttpd
chown -R www-data /var/www/html
echo "www-data ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers
sudo cp -r /home/pi/GIT/SOII/TP3/src/html /var/www/
cd /var/www/html/cgi-bin
make
cd ..
sudo lighty-enable-mod cgi
