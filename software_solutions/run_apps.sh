#!/bin/bash

# ./adb shell mkdir /mnt/yazeed
# ./adb shell mount -t tmpfs none /mnt/yazeed


# This script will run Android Benchmarks multiple times, and after each time, it will close all the opened apps and pages.

# Also, it will measure energy from rails, and measure QoS.

# to store results in txt file
OUTPUT_FILE=output/output_$(date '+%y-%m-%d_%H-%M')_apps.txt



run_benchmark() {
    benchmark=$1
    
 CFLAGS=-static-libstdc++
 /Users/yazeedyousefalmalaq/android-ndk/toolchains/llvm/prebuilt/darwin-x86_64/bin/armv7a-linux-androideabi21-clang++ $CFLAGS "$benchmark"
 ./adb push a.out /mnt/yazeed
./adb shell chmod u+x /mnt/yazeed/a.out
./adb shell /mnt/yazeed/a.out
}


    ##### Turn on the screen func ###
 unlock_screen() {
    ./adb shell input keyevent KEYCODE_WAKEUP
    sleep 2
    ./adb shell input swipe 500 1200 500 500
    sleep 2
}


    ##### close all apps func ###
close_all_apps() {
    ./adb shell input keyevent KEYCODE_APP_SWITCH
    sleep 2
    ./adb shell input swipe 100 500 1000 500
    sleep 2
    ./adb shell input swipe 100 500 1000 500
    sleep 2
    ./adb shell input swipe 100 500 1000 500
    sleep 2
    ./adb shell input touchscreen tap 200 1200
    sleep 2
}

# energy value from CH3= S2M_VDD_CPUCL2 (energy of large cores)
rails_energy_ch3() {
 ./adb shell cat /sys/bus/iio/devices/iio:device0/energy_value | grep 'CH3' | awk '{print $2}'
}
# energy value from CH4= S3M_VDD_CPUCL1 (energy of med cores)
rails_energy_ch4() {
 ./adb shell cat /sys/bus/iio/devices/iio:device0/energy_value | grep 'CH4' | awk '{print $2}'
}
# energy value from CH5= S4M_VDD_CPUCL0 (energy of small cores)
rails_energy_ch5() {
 ./adb shell cat /sys/bus/iio/devices/iio:device0/energy_value | grep 'CH5' | awk '{print $2}'
}

# to get the time from rails
rails_read_time() {
 ./adb shell cat /sys/bus/iio/devices/iio:device0/energy_value | grep 't' | sed 's/t=//'
}




######################## youtube benchmark #############
for i in {1..1}; do

# Lunch the apps:


# Lunch the web
   ./adb shell am start -n org.lineageos.jelly/.MainActivity
   sleep 1
   
# Lunch the game app
   ./adb shell am start -n com.JindoBlu.OfflineGames/com.unity3d.player.UnityPlayerActivity
      sleep 1
      
# Lunch the social media app
   ./adb shell am start -n com.twitter.android/com.twitter.android.StartActivity
   sleep 1

# Start measuremnets of energy and time from rails:
    start_energy1_value=$(rails_energy_ch3)
    start_energy2_value=$(rails_energy_ch4)
    start_energy3_value=$(rails_energy_ch5)
    start_time=$(rails_read_time)

# Run the benchmark:
   run_benchmark all_software_measuring.c >> $OUTPUT_FILE


# End measuremnets of energy and time from rails:
  end_energy1_value=$(rails_energy_ch3)
  end_energy2_value=$(rails_energy_ch4)
  end_energy3_value=$(rails_energy_ch5)
  end_time=$(rails_read_time)

# Calculate the energy (Joule), time (second) and power (Joule/second):
  energy1=$(echo "scale=2; ($end_energy1_value - $start_energy1_value)" | bc)
  energy2=$(echo "scale=2; ($end_energy2_value - $start_energy2_value)" | bc)
  energy3=$(echo "scale=2; ($end_energy3_value - $start_energy3_value)" | bc)
  energy_rails=$(awk "BEGIN { printf \"%.2f\", ($energy1 + $energy2 + $energy3) / 1000000 }")
  time_rails=$(echo "scale=2; ($end_time - $start_time) / 1000" | bc)
  power_rails=$(echo "scale=2;  $energy_rails/ $time_rails" | bc)
  echo "Power_rails_CPUs: $power_rails" >> $OUTPUT_FILE
  echo "Energy_rails_CPUs: $energy_rails" >> $OUTPUT_FILE
  echo "Time_rails: $time_rails"  >> $OUTPUT_FILE


# Close all apps in backgrounds:
  unlock_screen
  sleep 2
  close_all_apps
  sleep 2
done
