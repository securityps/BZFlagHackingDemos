# BZFlag Hacking Demos
BZFlag is a free and open-source multiplayer tank battle game. Here at Security PS, we love playing BZFlag during our team-building events. Since we also enjoy application security, it was only a matter of time before we created hacks for the game. While we do not condone the act of hacking on public servers, we think game hacking is a great introduction to the field of application security for interested students. As part of our Cybersecurity Student Education Program, we present a wallhack and movement hack and discuss with students the concepts behind them. For any students that are interested, we want to make the code available for self-study. We encourage you to learn from this code and not use it in a way that harms the BZFlag community. 

This repository contains a VS2019 solution with three projects:
- A DLL injector, which will load a user-defined dll into a process' memory
- A wallhack that will disable depth testing in the game and allow players to see other players through walls, and 
- A movement hack that will allow the player to move faster, through walls, and in different directions from the normal game 

The wallhack and movement hack are written specifically for the Windows 2.4.18 version of BZFlag, but can be easily modified to work with other versions. The solution and project files are targetted toward the community edition of Visual Studio 2019, which can be downloaded for free [here.](https://visualstudio.microsoft.com/vs/)

## Usage
To use the hacks, they must be injected into the BZFlag process. The steps to do this are outlined below:
1. Build the project
2. Start BZFlag and join a game
3. Open Command Prompt or Powershell and navigate to the Debug directory
4. Run `./Injector.exe bzflag Wallhack.dll` or `./Injector.exe bzflag MovementHack.dll`
5. If there are no errors, the DLL has been injected successfully. If there are errors, run both BZFlag and the Injector with Administrator privileges.
