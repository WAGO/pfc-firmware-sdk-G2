# Visual Studio Code Development container for PTXDIST
A **development container** is a running [Docker](https://www.docker.com) container with a well-defined tool/runtime stack and its prerequisites. You can try out development containers with **[GitHub Codespaces](https://github.com/features/codespaces)** or **[Visual Studio Code Remote - Containers](https://aka.ms/vscode-remote/containers)**.

## Setting up the development container

### VS Code Remote - Containers
Follow these steps to open this ptxdist environment in a container using the VS Code Remote - Containers extension:

1. If this is your first time using a development container, please ensure your system meets the pre-reqs (i.e. have Docker installed) in the [getting started steps](https://aka.ms/vscode-remote/containers/getting-started).

2. For this container to work properly, it is necessary to limit the memory for WSL 2. Open Windows Terminal/CMD/PowerShell and run the commands below:
    ```
    # turn off all wsl instances such as docker-desktop
    wsl --shutdown
    notepad "$env:USERPROFILE/.wslconfig"
    ```

    Edit .wslconfig file with notepad and write down these settings:
    ```
    [wsl2]
    memory=10GB   # Limits VM memory in WSL 2 up to 10GB
    processors=4 # Makes the WSL2 VM use four virtual processors
    Save the file and restart the docker-desktop service!
    ```
    See [here](https://docs.microsoft.com/en-us/windows/wsl/wsl-config#wsl-2-settings) for more information about configuring of WSL 2.

3. - Clone this repository to your local filesystem.
   - Press <kbd>F1</kbd> and select the **Remote-Containers: Open Folder in Container...** command.
   - Select the cloned copy of this folder and wait for the container to start.


## Things to try

Once you have this container running, you'll be able to work with **ptxdist** like you would locally.

Some things to try:

1. **Build firmware image:**  
Included is a build command  which you can call instead of *"ptxdistimages -q"* to start a "brute force" parallel build that repeats the compile process until the dependencies are all met. This is a workaround for the packages which have incomplete dependeny informations.
    ```
    build
    ```


For more information on how to develop with ptxdist please refer to one of the firmware source repos e.g.:  
https://github.com/WAGO/pfc-firmware-sdk  
and the PFC Howtos:  
https://github.com/WAGO/pfc-howtos
