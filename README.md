# HTTPD

Metamod module for serve static files for fastdl.

## Install
Install as metamod plugins. Example `cstrike\addons\httpd\httpd.dll`
Create a configuration file with paths listed: `cstrike\addons\httpd\httpd.cfg`
```
 ./cstrike
 ./cstrike_addon
 ./cstrike_downloads
```

Searching for files in directories depends on their order in the settings

Set `sv_downloadurl "http://localhost/"`  or your public ip address for working fastdl.

WARNING!!! 

The plugin was developed only for local play. public use is fraught with vulnerabilities and server hijacking. 
Directories and files specified in the config will be available without checking access rights!!!


## Build instructions
### Checking requirements
There are several software requirements for building HTTPD:

#### Windows
<pre>
Visual Studio 2015 (C++14 standard) and later
</pre>

#### Linux
<pre>
git >= 1.8.5
cmake >= 3.10
GCC >= 4.9.2 (Optional)
ICC >= 15.0.1 20141023 (Optional)
LLVM (Clang) >= 6.0 (Optional)
</pre>

### Building

#### Windows
Use `Visual Studio` to build, open `httpd.sln` and just select from the solution configurations list `Release` or `Debug` and platform x86

#### Linux

* Optional options using `build.sh --compiler=[gcc] --jobs=[N] -D[option]=[ON or OFF]` (without square brackets)

<pre>
-c=|--compiler=[icc|gcc|clang]  - Select preferred C/C++ compiler to build
-j=|--jobs=[N]                  - Specifies the number of jobs (commands) to run simultaneously (For faster building)

<sub>Definitions (-D)</sub>
DEBUG                           - Enables debugging mode
USE_STATIC_LIBSTDC              - Enables static linking library libstdc++
</pre>

* ICC          <pre>./build.sh --compiler=intel</pre>
* LLVM (Clang) <pre>./build.sh --compiler=clang</pre>
* GCC          <pre>./build.sh --compiler=gcc</pre>

##### Checking build environment (Debian / Ubuntu)

<details>
<summary>Click to expand</summary>

<ul>
<li>
Installing required packages
<pre>
sudo dpkg --add-architecture i386
sudo apt-get update
sudo apt-get install -y gcc-multilib g++-multilib
sudo apt-get install -y build-essential
sudo apt-get install -y libc6-dev libc6-dev-i386
</pre>
</li>

<li>
Select the preferred C/C++ Compiler installation
<pre>
1) sudo apt-get install -y gcc g++
2) sudo apt-get install -y clang
</pre>
</li>
</ul>

</details>
