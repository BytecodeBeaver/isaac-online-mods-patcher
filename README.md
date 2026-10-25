## Warning
```Keep in mind that using this program might lead to getting you banned from online mode and bad actors will have possiblity to execute malicious code on your device, such as viruses, btc miners, etc. I'm not taking any resposibility for whatever happens due to building or executing this program.```

**You've been warned! You are on your own now. ** 

# Issac online mod check bypass patcher

## About
This project is a patcher, which modifies **The Binding of Isaac: Repentence plus** game executable in a way, that allows modded clients to play together via online mode, despite having mods enabled.


## Building & running
The patcher is a standard project built with C++ and CMake. To built it first install CMake and inside project root run:

Windows build:
```shell
cmake -S . -B build-win -DTARGET_WINDOWS=ON && cmake --build build-win
```

Linux build:
```shell
cmake -S . -B build-linux && cmake --build build-linux
```

**I've built it with:**
- GCC 16.2.1-2
- CMake 4.3.0


After build, copy the compiled binary into the main game directory and run the patcher. 




## Limitations
Running mods in online session has potential issues, causing desynchronization(which is quite common even in vanilla due to networking implementation). Mods might make It much common, possibly due to many mods modifying the internal RNG object of the game in hard to predict ways.

Additionally, with every build the assembly code of the binary changes in hard to predict ways, meaning that this tool will need to be patched after update, specifically the instruction bytes to look for in assembly.