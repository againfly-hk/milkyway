# 更换清华源 https://mirrors.tuna.tsinghua.edu.cn/raspbian
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
sudo apt update
sudo apt upgrade

# 安装编译内核所需的环境
sudo apt install git bc bison flex libssl-dev make
sudo apt install libncurses5-dev
sudo apt install raspberrypi-kernel-headers

cd ~/linux
head -n 10 Makefile

xz -d patch-6.1.77-rt24.patch.xz

patch -p1 < ~/kernel/patch-6.1.77-rt24.patch


# opencv优化策略
# 1、安装实时补丁；
# 2、
