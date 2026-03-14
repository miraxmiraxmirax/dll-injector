# DLL Injector + Auto-Updating DLL Downloader

A simple **C++ DLL injector** that automatically **downloads a DLL from a URL and injects it into a running process**.

The program also **auto-updates the DLL** by downloading the latest version from the provided link each time it runs.

---

## Features

* Automatic **DLL download from URL**
* **Auto-update system** (re-downloads DLL each run)
* Waits for a **target process** to start
* Injects the DLL using **LoadLibrary + CreateRemoteThread**
* Lightweight and simple implementation
* Written in **C++**

---

## How It Works

1. The program gets the user's **AppData folder**
2. Creates a working directory
3. Downloads the DLL from the specified URL
4. Waits for the target process to start
5. Allocates memory inside the process
6. Writes the DLL path into the process memory
7. Calls **LoadLibraryA** using a remote thread
8. The DLL is loaded into the target process

---

## Requirements

* Windows
* Visual Studio (recommended)
* Windows SDK

Libraries used:

* `Windows.h`
* `TlHelp32.h`
* `urlmon.lib`

---

## Building

1. Create a **Console Application** in Visual Studio
2. Add the source file to the project
3. Make sure the following line exists in the code:

```cpp
#pragma comment(lib, "urlmon.lib")
```

4. Build the project

---

## Configuration

### Changing the DLL Download Link

Find this line in the code:

```cpp
const char* lpDownloadUrl = "dll link here";
```

Replace it with your own URL:

```cpp
const char* lpDownloadUrl = "https://yourdomain.com/file.dll";
```

Each time the program runs it will **download the DLL again**, allowing you to update the DLL remotely.

---

### Changing the Target Game / Process

Find this line:

```cpp
const char* lpProcessName = "gmod.exe";
```

Replace it with the process name of the program you want to inject into.

Example:

```cpp
const char* lpProcessName = "game.exe";
```

The injector will wait until this process is running before injecting the DLL.

---

### Changing the AppData Directory

The program stores the downloaded DLL inside a directory in **AppData**.

By default the folder structure is:

```
%APPDATA%\misaki\gmod\
```

This is created with the following code:

```cpp
sprintf_s(misakiDir, "%s\\misaki", appDataPath);
sprintf_s(gmodDir, "%s\\misaki\\gmod", appDataPath);
```

#### Changing the Folder Name

To change the directory, edit these lines.

Example:

```cpp
sprintf_s(misakiDir, "%s\\myfolder", appDataPath);
sprintf_s(gmodDir, "%s\\myfolder\\dll", appDataPath);
```

New result:

```
%APPDATA%\myfolder\dll\
```

---

#### Changing the DLL Name

You can also change the downloaded DLL name.

Find this line:

```cpp
sprintf_s(dllPath, "%s\\eb.dll", gmodDir);
```

Example change:

```cpp
sprintf_s(dllPath, "%s\\module.dll", gmodDir);
```

Result:

```
%APPDATA%\myfolder\dll\module.dll
```

---

## Usage

1. Run the injector
2. Start the target application
3. The program will detect the process automatically
4. The DLL will be downloaded and injected

Example console output:

```
Waiting for game.exe to start...

game.exe detected!

[DLL Injector]
Process : game.exe
Process ID : 1234

Injected!
```

---

## Disclaimer

This project is intended for **educational purposes only**.

Make sure you comply with the **software's terms of service** and applicable laws when using DLL injection techniques.
