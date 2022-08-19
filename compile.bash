#!/bin/bash
/opt/gcc-12/bin/g++ -O2 -std=c++20 -o main.out main.cpp 
/opt/gcc-12/bin/g++ -O2 -std=c++20 -o hardcode.out testHardcodedText.cpp 
/opt/gcc-12/bin/g++ -O2 -std=c++20 -o escapeAllCtrlChar.out EscapeAllControlChar.cpp 