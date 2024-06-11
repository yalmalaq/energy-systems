#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

double read_value(const char* file_path) {
    FILE *file = fopen(file_path, "r");
    if (!file) {
        exit(EXIT_FAILURE);
    }
    double value;
    fscanf(file, "%lf", &value);
    fclose(file);
    return value;
}


void openApp(const char* appActivity) {
    char command[150];
    snprintf(command, sizeof(command), "am start -n %s", appActivity);
    system(command);
}

void openurl(const char* appActivity) {
    char command[150];
    snprintf(command, sizeof(command), "am start -a android.intent.action.VIEW -d %s", appActivity);
    system(command);
}


double measure_power_consumption() {
    double current = read_value("/sys/class/power_supply/battery/current_now") / 1000000.0;
    double voltage = read_value("/sys/class/power_supply/battery/voltage_now") / 1000000.0;
    return current * voltage;
}

long read_counter() {
    int fd = open("/sys/class/power_supply/battery/charge_counter", O_RDONLY);
    if (fd < 0) {
        perror("Unable to open charge_counter file");
        exit(EXIT_FAILURE);
    }
    char buf[500];
    read(fd, buf, sizeof(buf));
    long ret = atol(buf);
    close(fd);
    return ret;
}

long read_energy_values() {
    int fd = open("/sys/bus/iio/devices/iio:device0/energy_value", O_RDONLY);
    if (fd < 0) {
        perror("Unable to open energy_value file");
        exit(EXIT_FAILURE);
    }
    char buf[500];
    ssize_t bytes_read = read(fd, buf, sizeof(buf));
    if (bytes_read < 0) {
        perror("Unable to read energy_value file");
        close(fd);
        exit(EXIT_FAILURE);
    }
    printf("Energy values: %.*s\n", (int)bytes_read, buf);
    close(fd);
    return 0;
}

void set_charging_charge_counter(int on) {
    int fd = open("/sys/class/power_supply/usb/device/usb_limit_sink_enable", O_WRONLY);
    if (fd < 0) {
        perror("Failed to open usb_limit_sink_enable");
        exit(EXIT_FAILURE);
    }
    char buf[] = "0\n";
    buf[0] = on ? '0' : '1';
    ssize_t size = write(fd, buf, sizeof(buf));
    if (size < (ssize_t)sizeof(buf)) {
        perror("Failed to write to usb_limit_sink_enable");
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd);
}

void set_charging_battery_state(int on)
{
     int fd = open("/sys/class/power_supply/main-charger/online", O_WRONLY);
    if (fd < 0) {
        perror("Failed to open usb_limit_sink_enable");
        exit(EXIT_FAILURE);
    }
    char buf[] = "0\n";
    buf[0] = on ? '1' : '0';
    int size = write(fd, buf, sizeof(buf));
    if (size < sizeof(buf)) {
                perror("no!");
                exit(-1);
            }
            close(fd);
}




double get_current_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}



void social_media() {
    openApp("com.twitter.android/com.twitter.android.StartActivity");
    
    for (int i = 0; i < 9; ++i) {
        system("input touchscreen swipe 100 1500 100 200");
        sleep(2);
    }
    sleep(2);
    system("input touchscreen swipe 100 200 100 1500");
    
    for (int i = 0; i < 2; ++i) {
        system("input touchscreen tap 990 2100");
        sleep(2);
    }
        system("input touchscreen tap 990 2100");
        sleep(2);
        system("input touchscreen tap 600 1900");
        sleep(2);
        system("input touchscreen tap 850 1700");
        sleep(2);
        system("input touchscreen tap 1000 200");
        sleep(2);
    
    
    openApp("com.zhiliaoapp.musically/com.ss.android.ugc.aweme.splash.SplashActivity");
    
    for (int i = 0; i < 5; ++i) {
        system("input touchscreen swipe 100 1500 100 200");
        sleep(5);
    }
}

void games() {
    openApp("com.JindoBlu.OfflineGames/com.unity3d.player.UnityPlayerActivity");
    sleep(20);
    
    system("input touchscreen tap 100 1500");
    sleep(2);
    
    system("input touchscreen tap 540 1900");
    sleep(2);
    
    system("input touchscreen tap 500 1700");
    sleep(2);
    
    for (int i = 0; i < 30; ++i) {
        system("input touchscreen tap 540 960");
        sleep(1);
    }

}
    

void web_browsing() {
    openurl("https://youtube.com");
    sleep(5);
    
    for (int i = 0; i < 5; ++i) {
        system("input touchscreen swipe 100 1500 100 200");
        sleep(2);
    }
    
    system("input touchscreen tap 100 1500");
    sleep(5);
    
    for (int i = 0; i < 3; ++i) {
        system("input touchscreen swipe 100 1500 100 200");
        sleep(2);
    }
    sleep(5);
    
    openurl("https://reddit.com");
    sleep(5);
    
    for (int i = 0; i < 5; ++i) {
        system("input touchscreen swipe 100 1500 100 200");
        sleep(2);
    }
    
    system("input touchscreen tap 100 1500");
    sleep(2);
    
    for (int i = 0; i < 5; ++i) {
        system("input touchscreen swipe 100 1500 100 200");
        sleep(2);
    }
}


int main() {
    set_charging_battery_state(0);
    set_charging_charge_counter(0);
    
    
    double initial_power = measure_power_consumption();
    double start_time = get_current_time();
    long start_value = read_counter();
    printf("Start rails data:\n");
    read_energy_values();
    


    // social_media app ( X + TikTook)
    //social_media();
    
    // Offiline games
   // games();

    web_browsing();
    


    double end_time = get_current_time();
 

    double final_power = measure_power_consumption();
//    printf("power_batterystate: %.6f W\n", final_power);
    
    double duration = end_time - start_time;
//    printf("time: %.6f seconds\n", duration);
    
    double energy_consumed = (initial_power + final_power) / 2 * duration;
//    printf("energy_batterystate: %.6f J\n", energy_consumed);
    long end_value = read_counter();
    
    double energy_charge_counter = 0.0138 *  (end_value - start_value);
    double power_charge_counter = energy_charge_counter / duration;
    
    printf("\nEnd rails data:\n");
    read_energy_values();
    
    printf("power_batterystate energy_batterystate Time\n%lf %lf %lf\n", final_power, energy_consumed, duration);
    
    printf("Power_charge_counter Energy_charge_chounter Time\n%lf %lf %lf\n", power_charge_counter, energy_charge_counter, duration);
    
    set_charging_charge_counter(1);
    set_charging_battery_state(1);
    return 0;
}

