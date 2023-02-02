# GTProxy
A Growtopia Proxy to get packet from Client and Server
# Build
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
