# HallC_NPS

Hall_C NPS Geant4 Simulation 

## Developer notes

In this section we document useful information for application developers, namely: how to install the Geant4 on local computer and how to setup the project in IDE.

### Install the Geant4 on your computer

Before installing Geant make sure following prerequisites are satisfied: `build essentials`, `Qt5`, `libX11`, `libXmu`, `Motif` libraries. On Fedora linux prerequisities can be installed via following commands:
```
sudo dnf groupinstall "C Development Tools and Libraries"
sudo dnf install mesa-libGL-devel qt5* libX11* libXmu* motif*
```
Create a folder for the source code location. It is reasonable to store source code in a dedicated `~/Source` folder in the home folder. Geant4 source code must remain on the filesystem in order to provide enchanced debugging capcbilities in Eclipse IDE.
Download Geant4 source code from [https://geant4.web.cern.ch/support/download](https://geant4.web.cern.ch/support/download) to `~/Source`.

Navigate to the `~/Source` folder, unpack the Geant4 code and create the directory for the build. In this example we will use Geant4 version `10.06.p02`. Please replace this with the corresponding version that you have downloaded.
```
mkdir -p ~/Source && cd ~/Source
tar -xzf geant4.10.06.p02.tar.gz
mkdir geant-build && cd geant-build
```
Next we will assign some CMake variables. Specify the source location for the build, turn on some fundamental Geant4 options and most importantly, tell CMake to carry out the compilation with the debug symbols. This feature will enable step-by-step debugging of the program code. It is reasonable to follow the macOS pattern and keep the compiled binaries in `~/Applications` folder.
```
mkdir -p ~/Applications
cmake -DCMAKE_INSTALL_PREFIX=$HOME/Applications/geant4.10.06.p02/ ./../geant4.10.06.p02
cmake -DGEANT4_INSTALL_DATA=ON .
cmake -DGEANT4_USE_OPENGL_X11=ON .
cmake -DGEANT4_USE_QT=ON .
cmake -DGEANT4_USE_XM=ON .
cmake -DCMAKE_BUILD_TYPE=Debug .
```
After the CMake configuration was specified, the compilation process is started. Depending on the performance of your computer this process can take about 30 minutes.
```
make -j$(nproc)
```
Finally the Geant4 code is compiled. Now we install in `~/Applications` folder:
```
make install
```
After the installation is completed it is necesary to source the Geant4 environment variables. In order to do it automatically with the start of the user session, source the Geant4 script in your `~/.bashrc`:
```
echo ‘source $HOME/Applications/geant4/bin/geant4.sh’ >> ~/.bashrc
```
Now that the setup of the Geant4 with debug symbols is complete, restart your session (logout and login back) and proceed with the Eclipse project setup.

### Setup the Glass Prototype Project in Eclipse

First, install Eclipse IDE. This is documented in the (Chapter 6 of my dissertation).Check out the repository into the desired location on your computer:

```
git clone https://github.com/petrstepanov/HallC_NPS && cd HallC_NPS
```
Create a directory for the Eclipse project. For demonstration purposes we willto store the Eclipse project
Thankfully CMake has a straightforward way of generating the Eclipse project. This saves quite a time for application developers.

## User notes

This section is designed for the end users and describes the process of running the simulation and obtaining the experimental results on the Computating Farm.
There is no need to install the Geant4 toolkit on the farm. It comes preinstalled. Howefer, after user logs in to the Computing Farm it is necessary to source the latest environment. In order to see the available versions of the environment run the following command:
```
csh /site/12gev_phys/softenv.csh
```
The output of the above command will provide the avalable versions of the software environments. It is recommended to use the latest environment unless the software requires some specific version of it. Currently (Dec 5, 2020) the latest version of the environment is `2.4`. Therefore one will execute:
```
source /site/12gev_phys/softenv.csh 2.4
```
The shortcut to source always the latest possible environment is following:
```
source /site/12gev_phys/softenv.csh `grep -oP "\d\.\d(?= \(production)" /site/12gev_phys/softenv.csh`
```

## Notes from the former developers

For Hall C DVCS (DVCS_evt_gen/), see https://wiki.jlab.org/cuawiki/images/f/fa/User_Guide.pdf for a short description on how to run on JLab/ifarm
