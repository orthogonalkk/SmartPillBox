# A smart pill box that support face detection

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)

This is my project for **Embedded System Programming and Practices in autumn 2023**. It utilizes PaddlePi-K210 to implement a smart pill box that supports face detection and authentication.

## Implemented functions
- face detection, feature extraction and use face information for authentication
- data transmission via UART (Drug information, how and when to take it)
- Use LCD flashing as a medication reminder
- a timer

## How to use



To compile this code, you need to utilize SDK fot Kendryte K210.
The followings are how to use this SDK.

### Usage

If you want to start a new project, for instance, `hello_world`, you only need to:

- Linux and OSX

`mkdir` your project in `src/`, `cd src && mkdir hello_world`, then put your codes in it, enter SDK root directory and build it.

```bash
mkdir build && cd build
cmake .. -DPROJ=<ProjectName> -DTOOLCHAIN=/opt/riscv-toolchain/bin && make
```

- Windows

Download and install latest CMake.

[Download cmake-3.14.1-win64-x64.msi](https://github.com/Kitware/CMake/releases/download/v3.14.1/cmake-3.14.1-win64-x64.msi)

Download and install latest toolchain.

[Download kendryte-toolchain-win-i386-8.2.0-20190409.tar.xz](https://github.com/kendryte/kendryte-gnu-toolchain/releases/download/v8.2.0-20190409/kendryte-toolchain-win-i386-8.2.0-20190409.tar.xz)

Open a Windows Powershell, cd to Project directory.

`mkdir` your project in `src/`, `cd src && mkdir hello_world`, then put your codes in it, and build it.

```powershell
$env:Path="E:\kendryte-toolchain\bin;C:\Program Files\CMak
e\bin" +  $env:Path

mkdir build && cd build
cmake -G "MinGW Makefiles" ../../../..
make
```

You will get 2 key files, `hello_world` and `hello_world.bin`.

1. If you are using JLink to run or debug your program, use `hello_world`
2. If you want to flash it in UOG, using `hello_world.bin`, then using flash-tool(s) burn <ProjectName>.bin to your flash.

This is very important, don't make a mistake in files.
