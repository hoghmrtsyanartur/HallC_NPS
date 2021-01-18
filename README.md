# HallC_NPS

Repository containing the code for simulation of the energy deposition profiles and energy resolution in the Glass Prototype NPS experiment. Table of contents:

- [Developer notes](#developer-notes)
    - [Installing the Geant4 on your computer](#installing-the-geant4-on-your-computer)
    - [Setting up the Glass Prototype Project in Eclipse](#setting-up-the-glass-prototype-project-in-eclipse)
- [User notes](#user-notes)
    - [Logging to the Computing Farm](#logging-to-the-computing-farm)
    - [Compilation of the Executable Program](#compilation-of-the-executable-program)
    - [Analysis of the Output Files](#analysis-of-the-outputfiles)
    - [Analysis of a Single File](#analysis-of-a-single-file)
    - [Plotting Energy Resolution for Multiple Energies](#plotting-energy-resolution-for-multiple-energies)
    - [Plotting Energy Resolution for Multiple Geometries](#plotting-energy-resolution-for-multiple-geometries)
    - [Copying the Results to Local Computer](#copying-the-results-to-local-computer)
    - [Notes from the Former Developers](#notes-from-the-former-developers)

## Developer notes

In this section we document useful information for application developers, namely: how to install the Geant4 on local computer and how to setup the project in IDE.

### Installing the Geant4 on your computer

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
cmake -DGEANT4_BUILD_MULTITHREADED=ON .
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

### Setting up the Glass Prototype Project in Eclipse

First, install Eclipse IDE. This process is documented in the [Chapter 6 of my dissertation](https://petrstepanov.com/static/petr-stepanov-dissertation-latest.pdf). Next, check out the Git repository into the desired location on your computer. I usually keep most of the Git repositories in `~/Development` folder.
```
mkdir -p ~/Development && cd ~/Development
git clone https://github.com/petrstepanov/HallC_NPS
```
In order to access the Geant4 source and header files in Eclipse we need to create a symlink to the Geant4 source code in the Glass Prototype source folder:
```
cd HallC_NPS/glass_prototype/source/
ln -s ~/Source/geant4.10.06.p02/source ./geant4_source
```
The name of the stmbolic link `geant4_source` is excluded from the version control. Therefore the Geant4 source files will not be checked in the Git tree. folder name is excluded in the `.gitignore`

Next we need to set up an Eclipse project. Thankfully, CMake has an automated way of generating the Eclipse project. This saves quite a time for application developers. The general CMake command is `cmake -G"Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ../project/source`, where the `../project/source` path must be relative or absolute path of the project source folder that contains Cmake cache file `CMakeLists.txt`.  Generally speaking there are two options:
1. **In-source build**. Eclipse project folder files are located the source program folder next to the `CMakeLists.txt` file. It is not favorable because project files will interfere with the Git tree and multiple excludes in `.gitignore` will be required. Also I've experienced Eclipse indexer issued using this method.
2. **Out-of-source build**. Eclipse project is located outside of the Git repository. This option is preferrable because the project, build and executable files are separated trom the Git project tree. However, in this option I ran into a problem, namely the Makefile targets were not generated automatically.

So, there seem to be problems with each type of build. However, it turned out that the out-of-source project build _located inside the Git tree_ works without any issues. Also the project folder can be simply excluded in `.gitignore` with one line. We will follow this option now and create a directory for the Eclipse project.
```
cd ~/Development/HallC_NPS/glass_prototype/
mkdir eclipse && cd eclipse
```
Note, `eclipse` folder is already added to the `.gitignore`. The out-of-source project generator build is initiated via following command:
```
cmake -G"Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ../source
```
At this point the project folder is ready to be imported in Eclipse. Open Eclipse and go to File → Open Projects from File System... Specify the project location in the modal dialog by clicking the "Directory..." button. Locate the `~/Development/HallC_NPS/glass_prototype/eclipse` project folder. Click "Finish".
Eclipse will import the project and start indexing source files and headers. This process may take up to 15 minutes.

More information regarding the CMake Eclipse generator can be found: [on Mantid project page](https://www.mantidproject.org/Setting_up_Eclipse_projects_with_CMake), [on Javier V. Gómez website](https://jvgomez.github.io/pages/how-to-configure-a-cc-project-with-eclipse-and-cmake.html), [on Cmake Wiki](https://gitlab.kitware.com/cmake/community/-/wikis/home), [again on Cmake Wiki](https://gitlab.kitware.com/cmake/community/-/wikis/doc/editors/Eclipse-CDT4-Generator) and [again on Cmake Wiki](https://gitlab.kitware.com/cmake/community/-/wikis/doc/editors/Eclipse-UNIX-Tutorial).

## User notes

This section is designed for the end users and describes the process of running the simulation and obtaining the experimental results on the Computing Farm. There is no need to install the Geant4 toolkit on the farm. It comes preinstalled. 

### Logging to the Computing Farm

Program code supports the running the executable in the Interactive simulation mode. However in order to forward the graphical output from the Computing Farm via the ssh protocol, it is necessary to pass the `-Y` parameter to the `ssh` command:
```
ssh -Y <your-username>@login.jalb.org
ssh ifarm
```
After user logs in to the Computing Farm it is necessary to source the latest environment or the Geant4 and ROOT toolkits. In order to see the available versions of the environment run the following command:
```
csh /site/12gev_phys/softenv.csh
```
The output of the above command will provide the avalable versions of the software environments. It is recommended to use the latest environment unless the software requires some specific version of it. Currently (Dec 5, 2020) the latest version of the environment is `2.4`. Therefore one will execute:
```
source /site/12gev_phys/softenv.csh 2.4
```
The shortcut to source always the latest possible environment is following (grep's regexp lookahead):
```
source /site/12gev_phys/softenv.csh `grep -oP "\d\.\d(?= \(prod)" /site/12gev_phys/softenv.csh`
```

### Compilation of the Executable Program

Next we check out this GitHub repository under the `Downloads` folder:
```
mkdir -p ~/Downloads && cd ~/Downloads/
rm -rf ./HallC_NPS
git clone https://github.com/petrstepanov/HallC_NPS
```
In order to not interfere with the original program code we will create a dedicated build directory outside of the Git repository tree.
```
mkdir -p ./HallC_NPS_glass_bulid && cd ./HallC_NPS_glass_bulid
cmake -DGeant4_DIR=$G4LIB/Geant4-$G4DATA_VERSION/ ./../HallC_NPS/glass_prototype/source/
make -j`nproc`
```

### Running the Simulation

Now the program is compiled and ready to launch. However, we need to specify the detector geometry, incident particle properties and other minor in the macro file.
```
nano ./macros/farm.mac
```
Adjust values of the following macro commands, that correspond to a variety of simulation properties:
```
# Crystal size along X, Y and Z axis
/detector/setCrystalSize 20 20 200 mm
# Crystal material - currently supported values are BaGdSiO, BaSi2O5, PbWO4
/detector/setCrystalMaterial BaGdSiO
# Number of crystals in the assembly along X and Y axis
/detector/setCrystalNumberX 3
/detector/setCrystalNumberY 3

# Output of the locations, energies and particle types that escape the World boundary
# turn this setting off when performing a simulation with number of events > 100
/histoManager/writeWorldEscape true

# Incident particle type and energy
/gps/particle e-
/gps/ene/mono 5.2 GeV

# Number of events for the visualization run
/run/beamOn 15

# Uncomment lines below in order to draw the energy deposition projections for crystals and PMTs
/score/drawProjection crystalsMesh eneDepCrystal myColorMap
/score/drawProjection pmtsMesh eneDepPMT myColorMap

# Number of events in the main run (for energy resolution)
/run/beamOn 5000
```
Please note that there are two `/run/beamOn` comands. First command defines the number of the events for the visualization run. Usually 10-20 events are enough to obtain an energy deposition shower profile. Second `/run/beamOn` command generates output ROOT file with a larger number of events for obtaining the energy resolution. Please use about 10000 events for the energy resolution.

Once the above parameter values are set, hit `CTRL+X` to close nano editor and save the macro file. Program can be launched via following command:
```
glass
```
The Geant4 user interface window is presented to the user. Click on the "Open" bitton on the toolbar and locate the `./macros/farm.mac` macro file containing the above commands that control the simulation. 

<p align="center">
 <img src="https://raw.githubusercontent.com/petrstepanov/HallC_NPS/master/glass_prototype/resources/Screenshot%20from%202020-12-21%2020-42-48.png" alt="Specify the Geant4 Macro File" /><br/>
 <i>Click the `Open` button on the left of the toolbar and locate the `./mcaros/farm.mac` macro file.</i>
</p>

After the program run finishes the visualization window will be demonstarted to the user. Second run Visualization accumulates particle trajectories from a single event. However, the energy profiles correspond to the total number of the events specified in the macro file.

<p align="center">
 <img src="https://raw.githubusercontent.com/petrstepanov/HallC_NPS/master/glass_prototype/resources/Screenshot%20from%202020-12-21%2020-45-41.png" alt="Visualization Image of the Energy Deposition in the Crystals and PMT assembly" /><br/>
 <i>Visualization Image of the Energy Deposition in the Crystals and PMT assembly is automatically saved in `./output` folder</i>.
</p> 

### Analysis of the Output Files
Simulation results, namely the visualization image in EPS format as well as two output ROOT files are automatically saved under the `./output` folder.

The glass prototype Geant4 program code generated two ROOT files for every simulation. First ROOT file contains events from 15 injected particles by default and is used to visualize the shower profile. Additionally this file contains information about the particles escaped the simulation: location, energy and particle type.

Second ROOT file contains considerably more number of events (10k by default) and is used to calculate the energy resolution of the detector.

A special ROOT script is designed to analyze the output files. Command to launch the script is following:
```
root ./draw/energy-deposition/energyDeposition.cpp
```
Script can process both types of output files.

#### Analysis of a single file

Select a desired ROOT data file to be processed by the program. The ROOT file containing smaller number of the events (15 by default) stores information about the escape locations and energies of the particles escaping the world, as well as the particle types. This is important to ensure the energy balance in the simulation is correct.

<p align="center">
 <img src="https://raw.githubusercontent.com/petrstepanov/HallC_NPS/master/glass_prototype/resources/Screenshot%20from%202020-12-22%2000-12-55.png" alt="Visualization of the particles escaping the world." /><br/>
 <i>Visualization of the particles escaping the world.</i>
</p> 

Another ROOT file containing condiderably larger number of events (10000 by default) must be used for plotting the energy resolution of the assembly.

<p align="center">
 <img src="https://raw.githubusercontent.com/petrstepanov/HallC_NPS/master/glass_prototype/resources/Screenshot%20from%202020-12-22%2000-43-16.png?123" alt="Energy deposition in the crystals and Energy Resolution calculation" /><br/>
 <i>Energy deposition in the crystals and Energy Resolution calculation.</i>
</p> 

The graph with total energy deposition in the crystal assembly per event is fitted with the Crystal Ball function. Energy resolution of the detector is calculated as the ratio of the Crystal Ball functino mean to the sigma. 

#### Plotting Energy Resolution for Multiple Energies

Script `energyDeposition.cpp` will plot the energy resolution graph for a set of energies if multiple input ROOT files with large statistics are selected. Locate the `.root` output files that correspond to the simulations of the same detector geometry for different energies. It is possible to select multiple files in ROOT dialog box by checking the `Multiple files` checkbox on the top right of the dialog box and holding the `CTRL` or `CMD` key on the keyboard:

<p align="center">
 <img src="https://raw.githubusercontent.com/petrstepanov/HallC_NPS/master/glass_prototype/resources/Screencast_01-12-2021_09 33 55 AM.gif" alt="Selecting multiple files with ROOT File dialog." /><br/>
 <i>Selecting multiple files with ROOT File dialog.</i>
</p>

The ASCII data file with energy resolution values for every energy of the incident particle is created. Additionally the energy resolution graph is plotted.

<p align="center">
 <img src="https://raw.githubusercontent.com/petrstepanov/HallC_NPS/master/glass_prototype/resources/Screenshot from 2021-01-12 21-38-45.png" alt="Energy resolution plot for multiple incident particle energies" /><br/>
 <i>Energy resolution plot for multiple incident particle energies.</i>
</p>

All above graphs are automatically saved in the same folder where the input data files are located.

#### Plotting Energy Resolution for Multiple Geometries

A special Gnuplot script `./glass_prototype/draw/energy-resolution/energyResolution.gp` can plot series of the energy resolutions for various detector geometries. open the script with a text editor of your choice:

```
nano ./draw/energy-resolution/energyResolution.gp
```
Update the data file names in the Gnuplot script to correspond to the energy resolution data points output by the `energyResolution.cpp` script. Then the energy resolution series can can be plotted with the following command:
```

```

### Copying the Results to Local Computer

In order to copy the results from the Computing Farm to the local computer the `scp` command can be used. On Windows one must install the PuTTY software to execute the below command. Native Terminal applications can be used on MacOS and Linux. Use following command to copy the Geant4 `output` folder to the local machine. Replace `<your-username>` with your JLab username:
```
scp -r <your-username>@login.jlab.org:/home/<your-username>/Downloads/HallC_NPS_glass_bulid/output ~/
```
In order to copy a single file, remove the `-r` flag from the above command.

## Notes from the Former Developers

For Hall C DVCS (DVCS_evt_gen/), see https://wiki.jlab.org/cuawiki/images/f/fa/User_Guide.pdf for a short description on how to run on JLab/ifarm
