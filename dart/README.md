sudo apt update
sudo apt upgrade
sudo apt autoremove

sudo apt install cmake git gedit
sudo apt install libopencv-dev

sudo raspi-config

git clone https://github.com/cedricve/raspicam.git
cd raspicam
mkdir build
cd build
cmake ..
sudo make install
sudo ldconfig