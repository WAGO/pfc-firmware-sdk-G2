***The open source software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.***


# FIRMWARE FOR PFC200-G2
Find the [table](#supported-devices) of supported devices at the end of the document


# ..:: Attention ::.. 
## Starting with firmware 23 only Codesys 3 will be supported. The last version for eCockpit will be FW22.x.


# Install WAGO-PFC-SDK using docker

This HowTo describes how to use the Software-Development-Kit (SDK) for PFC's based on docker.

All steps up to 3.1) need a connection to the internet. The build can be done without a network connection.

The pfc-builder image comes with a ready to use toolchain and the build tool ptxdist. In order to simplify the interaction with docker and ptxdist we provide a make [wrapper](##make-wrapper) including all steps to create PFC firmware images and beyond.

The start into the embedded linux world requires substantial technical know-how to have success. 
Therefore WAGO recommends familiarity with the following topics for the future embedded linux developer:
- Linux-OS
/- working on command line interface(CLI)
- writing Makefile, C or C++ code, shell-scripts.
- Cross-Platform-Development.

Helpful when heard about:
- Realtime operating systems
- PTXdist,
- ARM architecture
- Bootloader


## PREREQUISITES

### 1.) Download and install GIT
Make sure that you install GIT version >= 1.8.2

    sudo apt install git

### 2.) Install git-lfs (large file support)
Due to the fact that the repository contains files over 50MB you need the **GIT large file support** extension for GIT before you clone the repository.

    sudo apt install git-lfs && git lfs install

### 3.) Install docker 
Make sure that docker and make are installed on the host system.  
To install docker, please refer to the instructions depending on your host system, e.g for Ubuntu use [https://docs.docker.com/install/linux/docker-ce/ubuntu/](https://docs.docker.com/install/linux/docker-ce/ubuntu/).

Make sure docker can be run without root privileged. Refer to [https://docs.docker.com/engine/install/linux-postinstall/](https://docs.docker.com/engine/install/linux-postinstall/) for further information.

### 4.) Install make

    sudo apt install make

### 5.) Optional: Build the sdk-builder image manually.

The pfc-builder image bases on the sdk-builder image defined in [https://github.com/WAGO/sdk-builder](https://github.com/WAGO/sdk-builder). Docker will pull the corresponding base image during the build process. 

If you prefer to build the sdk-builder image manually follow instruction provided in the [repository](https://github.com/WAGO/sdk-builder) description. Pay attention to the sdk-builder version as each fw release is bound to exactly one sdk-builder release. 

## Installation and building STEP-BY-STEP:

### 1.) Check out the correct release
We provide one tag for each firmware(FW) release. You can download specific FW versions in the release section. Alternatively you may use git to clone the repository.  

    git clone git@github.com:WAGO/pfc-firmware-sdk-G2.git && cd pfc-firmware-sdk-G2

Afterward you can checkout a specific release.

    git checkout <fw release>    

### 2.) Create docker image pfc-builder

    make builder

### 3.) Initialize PTXdist project in ptxproj directory 

    make init

Alternatively you can also run pfc-builder in bash mode (```make bash```) and enter the following commands.

    ptxdist select configs/wago-pfcXXX/ptxconfig_pfc_g2 && \ 
    ptxdist platform configs/wago-pfcXXX/platformconfig && \      
    ptxdist toolchain /opt/gcc-Toolchain-2022.08-wago.1/LINARO.Toolchain-2022.08-wago.1/arm-linux-gnueabihf/bin/ && \
    ptxdist clean -q
    
#### 3.1) Get and download all packages

This step is optional, step 4.) includes this one. But using this step, everything that follows can be done without a connection to the internet.

    make get
    
Alternatively you can also run pfc-builder in bash mode (```make bash```) and enter the following command.

    ptxdist get -q

### 4.) Build all packages

    make build

Alternatively you can also run pfc-builder in bash mode (```make bash```) and enter the following command.

    ptxdist go -q

```
    started : ethtool.get
    finished: ethtool.get
    started : ethtool.extract
    finished: ethtool.extract
    started : base.get
    finished: base.get
    started : base.extract
    finished: base.extract
    started : kernel-header.get
    finished: kernel-header.get
    started : kernel-header.extract
    ...
```
>__Now it's probably time to go home for the day.
>Even on reasonably fast machines the time to build the PFC 
>firmware takes from 30 minutes up to a few hours.
>Another possibility is to read "How to become a PTXdist Guru"
>http://public.pengutronix.de/software/ptxdist/appnotes/
>in the meantime.__

### 5.) Create SD card image
Note that the SD card image is also provided in the release section for each FW. To generate those images run:

    make images

   Alternatively you can also run pfc-builder in bash (```make bash```) mode and enter the following command.
   
    ptxdist images -q

Afterwards you should find the firmware image "sd.hdimg" in folder
   **~/ptxproj/platform-wago-pfcXXX/images/ .**

#### 5.1) Create Wago Update Package (WUP)
It is possible to create the WUP for an easier update of the firmware via ethernet. To generate the WUP run:

    make wup

   Alternatively you can also run pfc-builder in bash (```make bash```) mode and enter the following command.
   
    make wup
    
Documentation on how to use the WUP can be found in the pfc manual: [https://www.wago.com/de/sps/controller-pfc200/p/750-8212#downloads](https://www.wago.com/de/sps/controller-pfc200/p/750-8212#downloads).

RAUC uses a certificate based approach to authenticate the origin of a bundle. To ease creating of WUP files during development, the BSP contains test certificates. In order to use RAUC's authentication mechanism to ensure the origin of a bundle, it is stricly recommended to to use custom certificates. A HowTo for this can be found here: [https://github.com/WAGO/pfc-howtos/tree/master/HowTo_GenerateWUPFile](https://github.com/WAGO/pfc-howtos/tree/master/HowTo_GenerateWUPFile).

### 6.) Write the binary image file "sd.hdimg" to SD-Card

#### 6.1) Virtual machine on a windows host

If you are working in a virtual machine on a windows host and you feel
unsafe using dd on the virtual machine just follow the follwing steps:
- Copy firmware image "sd.hdimg" to your windows host
- Download, install and use "Win32 Disk imager.exe"...

#### 6.2) For physical Ubuntu hosts you may use tools like "balena-etcher" or follow the next steps to copy the image manually.

- Disable desktops "automount-open" feature
```
    gsettings set org.gnome.desktop.media-handling automount true && \
    gsettings set org.gnome.desktop.media-handling automount-open false
```

- Identify SD-Card \
In this example we will identify **/dev/sde** as out sd card device.
```
    >mount
    /dev/sda1 on / type ext4 (rw,errors=remount-ro)
    proc on /proc type proc (rw,noexec,nosuid,nodev)
    sysfs on /sys type sysfs (rw,noexec,nosuid,nodev)
    ....
    /dev/sde1 on /media/BOOT type vfat (rw,nosuid,nodev,uid=1000,gid=1000,shortname=mixed,dmask=0077,utf8=1,showexec,flush,uhelper=udisks)
    /dev/sde2 on /media/disk type ext3 (rw,nosuid,nodev,uhelper=udisks)

    >umount /dev/sde1
    >umount /dev/sde2
```
- Copy "sd.hdimg" to SD-Card
```
    cd <workspace>/ptxproj/platform-wago-pfcXXX/images
```
Identify your sd card device, this will be needed as parameter for dd. 'of=[/dev/[sd card device].`\
We will use /dev/sde for example.
```
    >sudo dd if=sd.hdimg of=/dev/sde 
    [sudo] password for <user>:
    399360+0 Datasets in
    399360+0 Datasets out
    204427320 Bytes (204 MB) copied, 73.5553 s, 2.8 MB/s
```

### 7.) Boot PFC with custom image "sd.hdimg" on SD-Card
PFC boot device order checks SD-Card slot before internal NAND.\
By default, PFC tries to find a DHCP-Server to retrieve an IP-Address.

1. PowerOff PFC
2. Insert SD-Card with custom-image
3. PowerOn PFC

### 8.) Default settings for PFC

#### 8.1) User accounts (usr/pwd):

| User  | Password |
|-------|----------|
| root  | wago |
| admin | wago |
| user  | user |

#### 8.2) Hostname 
__"PFCx00-uvwxyz"__ \
Where 'uvwxyz' is the last part of eth0's MAC-ID in capital letters.

#### 8.3) Network Interfaces X1, X2: "br0"(Default)
- Operate in "switched-mode"
- DHCP-Client request dynamic Ip-Address from DHCP Server.
- SSH-Daemon is enabled.
- https-Server is enabled.

#### 8.4) Service Interface
Prepared to dial with:
- "WAGO IO-Check.exe" version 5.3.1.17 or higher 
- "WAGO EthernetSettings.exe" version 5.4.1.3 or higher
REQUIRE: WAGO-Service-Cable 750-920 or 750-923

#### 8.5) PFC Onboard Serial Interface X3 "/dev/ttyO0"(9p SubD):
Is no more owned by operating system Linux, so it has to be configured to be the console output.\
This can be easily be done via the web-confuguration.

#### 8.6) Get in touch with your PFC the first time

- Variant A: Use tool WAGO-IO-Check to set ip address or obtain actual\
A.1) Connect PFC-Service-Interface and PC with Service-Cable 750-920 or 750-923.\
A.2) Start windows tool "WAGO-IO-Check.exe".\
A.3) Configure local serial interface to use\
A.4) Press button [Identify]\
A.5) Move to register card "Network"\

- Variant B: Web-Based-Management(WBM) via https\
B.0) Obtain actual IP address\
B.1) Open WBM in browser(https://<ip or hostname>)\
B.2) Ignore x509 certificate warning and proceed.\
B.3) Now you should be able to view and change settings.\

- Variant C: Command-Line-Interface(CLI) via ssh\
C.0) Obtain actual IP address\
C.1) Start ssh-client, such as putty\
C.2) Open ssh session to PFC <ip or hostname>)\
C.3) Ignore 'PuTTY Security Alert' and proceed.\
C.4) Login as user/password (Default: "root"/"wago")\
C.5) Start WAGO config tool "/usr/sbin/cbm"\


## 9.) Learn more about "ptxdist"
Read "How to become a PTXdist Guru" and other ressources at http://public.pengutronix.de/software/ptxdist/appnotes/


## 10.) Aspects of the daily work
Building a complete firmware image is necessary exactly one time, to extract
and build the whole system. For your daily work, you usually will work on package level.
After entering the pfc-builder (```make bash```), you can:
- Rebuild a package
```
    >ptxdist clean <pkg-name> && ptxdist targetinstall <pkg-name>
```
- Copy "binaries" somehow by hand into PFC's file system, and make them executable.
- Transfer install-package "<pkg-name>.ipk" somehow into PFC file system and call
```
    >opkg install <pkg-name>.ipk
```
- Utilize Web-Based-Management(WBM) feature "Software-Upload" to upload and apply "<pkg-name>.ipk".

**CAUTION:\
Every binary or installation package(.ipk) is generated for a specific firmware version,
do not mix up different versions.**

### 10.1) Projects/packages can be managed from command line interface as follows:
Open 4 shell sessions on your development host.
- First shell used to open source code files within an editor like vim or gedit
```
    cd /wago/ptxproj/src/kbusdemo && gedit kbusdemo1.c
```
- Second shell inside pfc-builder (```make bash```)used to “clean” and “build” the ptxdist package
```
    ptxdist clean kbusdemo && ptxdist targetinstall kbusdemo
    
```
- Third shell used to transfer executable to PFC.
```
    scp kbusdemo1 root@<ip or hostname>:/usr/bin/kbusdemo1
```
- Fourth shell used to manage and control the PFC via ssh
```
    >ssh root@<ip or hostname>

    >cd /usr/bin
    >chmod +x kbusdemo1
    >./kbusdemo1
```
For the next round trip:
        1. Switch to the first shell and use the cursor-up-key to call the last command…
        2. Modify source code and save changes
        3. Switch to the second shell and use the cursor-up-key to call the last command…
        4. Rebuild executables
        5. Switch to the third shell and use the cursor-up-key to call the last command…
        6. Transfer executables to PFC 
        7. Switch to the fourth shell and use the cursor-up-key to call the last command…
        8. Start executable on PFC

### 10.2) Using Web-Based-Management(WBM) feature "Software-Upload" for upload and applying IPK packages
Tool "ptxdist" automatically generates an ".ipk" file for each package during build process.
#### 10.2.1) Start your local browser, and navigate to PFC200's default homepage(WBM)
```
https://ip.address.assigned.by.dhcp
```
Ignore Cert-Warning ...
#### 10.2.2) Select "Software-Upload" in left hand "Navigation" pane, You will be requested to authenticate!
```
Login as "admin" with password "wago" (default)
```
#### 10.2.3) Click on button [Browse...] and open the local file dialog.
Browse to folder "~/wago/ptxproj/platform-wago-pfcXXX/packages/"\
Select package to install or update, here "kbusdemo_0.3_arm.ipk".

#### 10.2.4) Click on button [Start Upload].
Transfers selected file into PFC file system and show button [Activate].

#### 10.2.5) In newly shown section "Activate new software", click on button [Activate] install package.
Internally WBM just calls:
```
    cd /home/ && opkg install kbusdemo_0.3_arm.ipk
```
Depending on type of package a restart of PFC may required.


**It may a good idea to setup a build server for a nightly build to check dependencies 
and consistency and do some unit testing in an automated way.**

## Make wrapper
| Command       | Description |
| ------------- | ----------- |
| builder       | create docker image *pfc-builder* |
| init          | initialize PTXdist project in *ptxproj* directory |
| get           | download all OSS packages |
| offline-get   | verify that all OSS packages are downloaded |
| build         | build all packages |
| offline-build | build all packages without network access |
| images        | create SD card image |
| wup           | create WUP file |
| bash           | runs *pfc-builder* in bash mode |

## Supported devices
| Article Number   | Designation |
|------------------|-------------|
| 750-8210 | PFC200 G2 4ETH |
| 750-8210/025-000 | PFC200 G2 4ETH T |
| 750-8210/040-000 | PFC200 G2 4ETH XTR |
| 750-8211 | PFC200 G2 2ETH 2SFP |
| 750-8211/040-000 | PFC200 G2 2ETH 2SFP T |
| 750-8211/040-001 | PFC200 G2 2ETH 2SFP Tele T |
| 750-8212 | PFC200 G2 2ETH RS |
| 750-8212/000-100 | PFC200 G2 2ETH RS BACnet/IP |
| 750-8212/025-000 | PFC200 G2 2ETH RS T |
| 750-8212/025-001 | PFC200 G2 2ETH RS Tele T |
| 750-8212/025-002 | PFC200 G2 2ETH RS ECO Tele T |
| 750-8212/040-010 | PFC200 G2 2ETH M12 RS XTR |
| 750-8213 | PFC200 G2 2ETH CAN |
| 750-8213/040-010 | PFC200 G2 2ETH M12 CAN XTR |
| 750-8214 | PFC200 G2 2ETH RS CAN |
| 750-8215 | PFC200 G2 4ETH CAN USB |
| 750-8216 | PFC200 G2 2ETH RS CAN DPS |
| 750-8216/025-000 | PFC200 G2 2ETH RS CAN DPS T |
| 750-8216/025-001 | PFC200 G2 2ETH RS CAN DPS Tele T |
| 750-8217 | PFC200 G2 2ETH RS 4G EU |
| 750-8217/025-000 | PFC200 G2 2ETH RS 4G EU T |
| 750-8217/600-000 | PFC200 G2 2ETH RS 4G Global |
| 750-8217/625-000 | PFC200 G2 2ETH RS 4G Global T |
| 750-8110 | PFC100 G2 2ETH ECO |
| 750-8111 | PFC100 G2 2ETH |
| 750-8112 | PFC100 G2 2ETH RS |
| 750-8112/025-000 | PFC100 G2 2ETH RS T |
