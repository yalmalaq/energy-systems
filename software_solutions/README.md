# software_solutions

Our C application "all_software_measuring" contains the three purposed software based energy methods and three workloads scenarios (games, web, social media).

# compile our C/C++ application and execute it on an Android phone:
1. Install android-ndk
2. Install platform-tools >> for "adb"
3. In terminal: 
- #CFLAGS=-static-libstdc++ /Path_to/android-ndk/toolchains/llvm/prebuilt/darwin-x86_64/bin/armv7a-linux-androideabi33-clang $CFLAGS -o app_name c_app.c
- ./adb push app_name /data
- ./adb shell chmod u+x /data/app_name
- ./adb shell /data/app_name 



- Alternatively: Using Termux on Android phone to compile and execute C/C++ :
1. install termux app on your mobile
2. install the needed pkgs such as: pkg install libllvm, apt-get install libllvm
- ./adb push c_app.c /data/data/com.termux/files/home
- ./adb shell
- export PATH=/data/data/com.termux/files/usr/bin:${PATH}
- cd /data/data/com.termux/files/home
- gcc -o app_name c_app.c
- ./app_name

# Or simply just run the script in the terminal: 
/run_apps.sh
