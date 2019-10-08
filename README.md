LiveRender
==========
LiveRender is an open source cloud gaming system.

Set up
------

### Prerequisites

Microsoft Visual Studio (c++) 2010 (If you want to quick start, you can install Microsoft Visual C++ 2010 Redistributable Package, http://www.microsoft.com/en-us/download/details.aspx?id=5555)

DirectX SDK 9.0 c (Recommended: DXSDK_Jun10 version, the version released in June 2010, http://www.microsoft.com/en-us/download/details.aspx?id=6812)

### Build

### Downloading the source code
The project includes four subproject: game_server for server side, game_client for client side, loader for initializing game process and network, utility as a set of utility classes. Dependencies: game_client and game_server depend on D3D library and utility, and loader depends on utility. 
### Properties Configuration
Adding include directories:

Selecting game_client project, "Properties"-->"Configuration Properties" --> "C/C++" --> "General" --> "Additional Include Directories", add "DirectX SDK Directory\Include", and the same for game_server. Add "\utility" to game_client, game_server and loader include directories.

Adding linker library:

Selecting game_client project, "Properties"-->"Configuration Properties" --> "Linker" --> "General Additional Library Directories", add "DirectX SDK Directory\Lib\x86"(target to 32bit program). The same for game_server.

Configuration Type. 

"Properties"-->"Configuration Properties" --> "General" --> "Configuration Type". Select .exe for game_client and loader, and .dll for game_server.

Quick Start
-----------
You can start with our pre-compiled exe files for Windows x86(with Visual Studio C++ 2010).
### Server Side
Copy files: "game_server.conf", "game_server.dll", "loader.exe" and "SDL2.dll" from "Release" directory to the game directory. Make sure these files are standing in the same directory of the game exe file. Run the "loader.exe".
### Client Side
Copy files: "game_client.conf", "game_client.exe", "SDL2.dll" to a folder. Configurate "game_client.conf", set ip address to the server's ip. Then shit + right mouse-clicking, choose "Open a command window here". Use commands: game_client game_name, here game_name refers to the game(name of game exe) you want to start in the server side. For example, game_client trine.

Game test cases
---------------

SprillRitchie: http://sprill-ritchie-abenteuerliche-zeitreise.software.informer.com/1.0/

Shadowrun returns: http://harebrained-schemes.com/shadowrun/

CastleStorm: http://www.castlestormgame.com/

Trine: http://trine-thegame.com/site/

Unity Angry Bots: http://unity3d.com/showcase/live-demos#angrybots

About us
--------
This project is developed by Huazhong University of Science and Technology, Service Computing Technology and System Lab, and Cluster and Grid Computing Lab. Our lab website is http://grid.hust.edu.cn/. If you have any questions, pls contact with Li Lin (llfjfz@163.com).
For more details, you can find them in "Li Lin, Xiaofei Liao, Guang Tan, Hai Jin, Xiaobin Yang, Wei Zhang, Bo Li. LiveRender: A Cloud Gaming System Based on Compressed Graphics Streaming. In Proc. of the 22nd ACM International Conference on Multimedia(MM'14), ACM, 2014: 347-356."
