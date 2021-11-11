# HexCoder
<p align="center">
<img src="https://b.radikal.ru/b06/2111/c1/6f6a16b5f8f3.png">
<p align="center">

XOR-based encryption processor with multithread support.

# Features:

- Encrypt via password
- Encrypt via logical and arithmetic operators
- Both ways
- Settable read file block size
- Settable hash algorithm for password (MD5, SHA256, SHA512)

# How to use:

```
Usage: HexCoder-MSVC.exe {-m {p|a|b}} {-d {e|d}} [--hash {None|MD5|SHA256|SHA512}] [-b blockSize] {-f filePath} [-a actionsFilePath] [-h] | -c
-m, --method            p - Password, a - Actions, b - Both. Default Password
-d, --direction         e - Encrypt, d - Decrypt. Default Encrypt
    --hash              Hash algorithm for password. Default is SHA256
-b,                     Block size as power of two. Default 28 (256Mb)
-f, --file              File path
-a, --actions           Actions file path
-c, --create            Open menu for creating and save actions
                        Other parameters will be ignore
-h, --help              Print this message
```
  
# Examples

Encrypt file via password and actions with block size 128Mb with SHA512
  
`.\HexCoder-MSVC.exe -m b -b 27 -a C:\..\..\actions.hca --hash SHA512 -f C:\..\..\file.ext`

Or using relative paths
  
`.\HexCoder-MSVC.exe -m b -b 27 -a actions.hca --hash SHA512 -f file.ext`

Decrypt file
  
`.\HexCoder-MSVC.exe -d d -m b -b 27 -a actions.hca --hash SHA512 -f file.ext`

Create actions

`.\HexCoder-MSVC.exe -c`
  
1. Enter **1** to open editor
2. Follow the instruction
3. Enter **2** to save
4. Enter filename
 
