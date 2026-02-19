# NotSergey
Project NotSergey: Behavioral Analysis of Ransomware-Disguised Wipers
Overview
NotSergey is a controlled Proof of Concept (PoC) developed for educational purposes to demonstrate the mechanics of destructive malware ("wipers") that masquerade as ransomware. This project explores how certain threat actors use the guise of financial extortion to delay incident response while performing irreversible data destruction.

This repository serves as a technical case study in malware behavior, persistence mechanisms, and user-interface manipulation within the Windows environment.

Educational Objectives
The primary goals of this project are:

Behavioral Analysis: To study how malware interacts with the Windows File System and Known Folder IDs.

Persistence Mechanisms: Demonstrating the use of the Windows Registry and Startup folders for maintaining presence.

Social Engineering: Illustrating how fake payment interfaces are used to deceive victims.

Resource Exhaustion: Analyzing the impact of rapid disk-writing and system noise on system stability.

Technical Features
The PoC implements several common malware techniques:

Persistence: Utilizes IShellLinkW for startup link creation and HKEY_CURRENT_USER registry keys.

File Manipulation: Uses the <filesystem> library to iterate through user directories and WriteFile to overwrite headers with junk data.

Multithreading: Implements std::thread to run simultaneous background tasks (Audio, Disk I/O, and GUI).

GUI Simulation: A Win32 API-based interface that simulates a decryption status timer.

Implementation Details
Compilation Requirements
To compile the project for analysis, the following environment is required:

Compiler: MSVC (cl.exe) or GCC (g++) with C++17 support.

Linked Libraries: user32, gdi32, winmm, ole32, advapi32, shell32, shlwapi.

Command Line:

Bash

g++ NotSergey.cpp -o NotSergey.exe -luser32 -lgdi32 -lwinmm -lole32 -ladvapi32 -lshell32 -mwindows -std=c++17
Security Warning & Ethical Use
[!CAUTION]
This software is for research and educational purposes only. The execution of this code results in irreversible data loss within the targeted directories.

Do not execute this binary on any production system.

Only run this in a strictly isolated virtual environment (Sandbox).

The author assumes no liability for misuse or damage caused by this code.

Disclaimer
This project was created as part of a cybersecurity curriculum to better understand defensive strategies against destructive threats. It is not intended for malicious use.
