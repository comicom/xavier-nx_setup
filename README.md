# xavier-nx_setup

https://medium.com/may-i-lab/nvidia-jetson-xavier-nx-%EC%82%AC%EC%9A%A9-%EC%A4%80%EB%B9%84%ED%95%98%EA%B8%B0-d62cfea15165

```
sudo apt update && sudo apt upgrade
```

## (optional)INSTALL JTOP: https://github.com/rbonghi/jetson_stats

```
sudo -H pip install -U jetson-stats
```

## BUILD OPENCV & INSTALL TENSORFLOW: https://qiita.com/misoragod/items/c36c1ffd682b2a5b6e6f 

https://www.jetsonhacks.com/2018/11/08/build-opencv-3-4-on-nvidia-jetson-agx-xavier-developer-kit/

```
sudo apt install python3-pip

mkdir tf
cd tf
sudo python3 -m pip install virtualenv
virtualenv env
```

Add line 2 of env/bin/activate
```
OLD_PYTHONPATH=$PYTHONPATH
export PYTHONPATH="/usr/local/lib/python3.6/dist-packages:/usr/lib/python3/dist-packages:/usr/lib/python3.6/dist-packages"
```

Add line 2 of env/bin/postdeactivate
```
export PYTHONPATH=$OLD_PYTHONPATH
unset OLD_PYTHONPATH
```

Install openCV
```
git clone https://github.com/jetsonhacks/buildOpenCVXavier.git
cd buildOpenCVXavier
git checkout v1.0
./buildOpenCV.sh
```

Build example
```
g++ -o gstreamer_view -Wall -std=c++11 gstreamer_view.cpp $(pkg-config –libs opencv)

g++ -o gstreamer_view -Wall -std=c++11 gstreamer_view.cpp `pkg-config --cflags --libs opencv`
sudo ldconfig 
./gstreamer_view
```

Install tensorflow
```
### Install in tf folder (virtual environment)
sudo apt-get install -y libhdf5-dev libc-ares-dev libeigen3-dev
sudo apt-get install -y libhdf5-serial-dev hdf5-tools zlib1g-dev zip libjpeg8-dev liblapack-dev libblas-dev gfortran
sudo apt-get install -y openmpi-bin libopenmpi-dev
sudo apt-get install -y libatlas-base-dev
python3 -m pip uninstall tensorflow
pip3 install -U numpy==1.16.1 future==0.17.1 grpcio absl-py py-cpuinfo psutil portpicker six mock==3.0.5 requests gast==0.2.2 h5py==2.9.0 astor termcolor protobuf keras_preprocessing==1.0.5 keras_applications==1.0.8 wrapt google-pasta setuptools testresources scipy==1.4.1

###JP_VERSION
The major and minor version of JetPack you are using, such as 42 for JetPack 4.2.2 or 33 for JetPack 3.3.1.

###TF_VERSION
The released version of TensorFlow, for example, 1.13.1.

###NV_VERSION
The monthly NVIDIA container version of TensorFlow, for example, 19.01.

$ pip3 install --extra-index-url https://developer.download.nvidia.com/compute/redist/jp/v44 tensorflow==$TF_VERSION+nv$NV_VERSION


e.g
### version 2
pip3 install --extra-index-url https://developer.download.nvidia.com/compute/redist/jp/v44 tensorflow==2.1.0+nv20.4 --user
### version 1
pip3 install --extra-index-url https://developer.download.nvidia.com/compute/redist/jp/v44 tensorflow==1.51.2+nv20.4 --user

```

```
pip3 install --upgrade pip sudo apt-get install zlib1g-dev zip libjpeg8-dev libhdf5-dev sudo pip3 install -U numpy grpcio absl-py py-cpuinfo psutil portpicker grpcio six mock requests gast h5py astor termcolor
sudo apt-get install cmake python-catkin-pkg python-empy python-nose python-setuptools libgtest-dev python-rosinstall python-rosinstall-generator python-wstool build-essential git
```

darknet setup
```
#nvcc setup
export PATH=/usr/local/cuda-10.2/bin${PATH:+:${PATH}}
export LD_LIBRARY_PATH=/usr/local/cuda-10.2/lib64${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}
```

install sklearn
```
pip install --upgrade setuptools
sudo pip install -U setuptools
sudo apt-get install libpcap-dev libpq-dev
sudo pip install cython

sudo pip install git+https://github.com/scikit-learn/scikit-learn.git
```
module Error: No module named 'sklearn.utils.linear_assignment_'

from sklearn.utils.linear_assignment_ import linear_assignment

->

from scipy.optimize import linear_sum_assignment as linear_assignment

```
sudo apt-get install python-scipy
```

others

http://www.neko.ne.jp/~freewing/raspberry_pi/nvidia_jetson_xavier_nx_2020_initialize/

## SORT SETUP ##

```
sudo apt-get install python-matplotlib python-numpy python-pil python-scipy
sudo apt-get install build-essential cython
sudo apt-get install python-skimage

sudo apt-get install python3-module-filterpy
```

## GPU monitoring ##

https://www.jetsonhacks.com/2018/05/29/gpu-activity-monitor-nvidia-jetson-tx-dev-kit/
