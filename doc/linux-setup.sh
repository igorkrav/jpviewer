sudo apt update
sudo apt-get install libjxl-dev
sudo apt-get install libturbojpeg libturbojpeg0-dev
sudo apt-get install libglfw3-dev
sudo apt-get install libglew-dev

-------
install on linux
vi ~/.local/share/applications/my-viewer.desktop
update-desktop-database ~/.local/share/applications/
xdg-mime default my-viewer.desktop image/jpeg