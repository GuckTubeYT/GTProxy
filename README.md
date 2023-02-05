# GTProxy
A Growtopia Proxy to get packet from Client and Server, no shadowban i hope<br>
# Build
- [CodeBlocks (Windows Version)](https://github.com/GuckTubeYT/GTProxy/#codeblocks-windows-version)
- [Manual Build (GCC Windows Version)](https://github.com/GuckTubeYT/GTProxy/#manual-build-gcc-windows-version)
## CodeBlocks (Windows Version)
1. Download CodeBlocks from http://www.codeblocks.org/downloads/binaries/ and download MINGW Version
2. Download this project
3. Open `GTProxy.cbp`
4. Before build this project, Press `Settings --> Compiler...`<br><br>
![image](https://user-images.githubusercontent.com/56192597/216290797-e8e78f02-d576-4235-86c8-53b28dc61813.png)
5. And then go to `Linker settings`<br><br>
![image](https://user-images.githubusercontent.com/56192597/216291324-0becc6b8-b575-4554-994a-32475d316451.png)
6. And then, put `-lws2_32 -lwinmm` on `Other linker options`. after put that, press OK <br><br>
![image](https://user-images.githubusercontent.com/56192597/216291666-1b87ddfa-2938-4c20-96d8-a6a828c36e77.png)
7. and now, you are ready to build the project, to build the project, you can press ![image](https://user-images.githubusercontent.com/56192597/216292002-8de7d903-c974-4a37-bda4-e0f8917342e3.png)
## Manual Build (GCC Windows Version)
If you have `gcc is not recognize` issue, please read 1 to 5
1. Make sure, you installed GCC for Windows (you can use CodeBlocks MINGW Version for manual build)
2. to making environment, goto `Control Panel` and type `environment` and then press `Edit the system environment variables`<br><br>
![image](https://user-images.githubusercontent.com/56192597/216309023-f02bbaa0-5c0c-499e-96e6-3ee622c66d1c.png)
3. press `Environment Variables...`<br><br>
![image](https://user-images.githubusercontent.com/56192597/216309414-ee4ff480-716a-4a12-ad5c-1b32eae45018.png)
4. press PATH on System Variables<br><br>
![image](https://user-images.githubusercontent.com/56192597/216310063-9661f608-f338-4e88-8614-5a1754b7bfec.png)
5. Add your GCC path to system variables (because i am using CodeBlocks MINGW, so i must search CodeBlocks Path) and then press OK <br><br>
![image](https://user-images.githubusercontent.com/56192597/216310430-c2d1c2b7-ea67-431d-9ff5-48c273523890.png)
6. after you put GCC Path to system variables, now open Command Prompt, and cd to project path, and type `build`<br><br>
![image](https://user-images.githubusercontent.com/56192597/216310843-8442f295-5769-42e7-86f6-ce74f6ff4dc8.png)
## Manual Build (Linux Version)
1. first, you need build-essential, to install build-essential type `sudo apt install build-essential`
2. after install build-essential, do `git clone https://github.com/GuckTubeYT/GTProxy` (if you get error message like this `git: command not found`, you must install git, to install git, type `sudo apt install git`)
3. goto the project folder, and then type `bash build.sh`
4. to run App, type `./proxy`
