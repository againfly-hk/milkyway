# 更换清华源 mirrors.tuna.tsinghua.edu.cn
sudo apt update
sudo apt upgrade
sudo apt autoremove

# 安装必要的驱动
sudo apt install cmake git gedit
sudo apt install libopencv-dev

# 设置cam raspicam支持
sudo raspi-config

# 安装raspicam c++ api
git clone https://github.com/cedricve/raspicam.git
cd raspicam
mkdir build
cd build
cmake ..
sudo make install
sudo ldconfig

# 安装实时补丁

# opencv优化策略
# 1、安装实时补丁；
# 2、
