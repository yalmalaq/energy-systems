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

void swipe(int startX, int startY, int endX, int endY) {
    char command[100];
    snprintf(command, sizeof(command), "input touchscreen swipe %d %d %d %d", startX, startY, endX, endY);
    system(command);
}

void tap(int x, int y) {
    char command[100];
    snprintf(command, sizeof(command), "input touchscreen tap %d %d", x, y);
    system(command);
}

void openApp(const char* appActivity) {
    char command[150];
    snprintf(command, sizeof(command), "am start -n %s", appActivity);
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




double get_current_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}


int main() {
    double initial_power = measure_power_consumption();
    double start_time = get_current_time();
    long start_value = read_counter();
    printf("Start rails data:\n");
    read_energy_values();
    
    openApp("com.twitter.android/com.twitter.android.StartActivity");

    // doing realistic behav on X app
    swipe(100, 1500, 100, 200);
    sleep(2);
    swipe(100, 1500, 100, 200);
    sleep(2);
    swipe(100, 1500, 100, 200);
    sleep(2);
    swipe(100, 1500, 100, 200);
    sleep(2);
    swipe(100, 1500, 100, 200);
    sleep(2);
    swipe(100, 1500, 100, 200);
    sleep(2);
    swipe(100, 1500, 100, 200);
    sleep(2);
    swipe(100, 1500, 100, 200);
    sleep(2);
    swipe(100, 200, 100, 1500);
    sleep(2);
    tap(990, 2100);
    sleep(2);
    tap(990, 2100);
    sleep(2);
    tap(600, 1900);
    sleep(2);
    tap(850, 1700);
    sleep(2);
    tap(1000, 200);
    sleep(2);

    openApp("com.zhiliaoapp.musically/com.ss.android.ugc.aweme.splash.SplashActivity");

    swipe(100, 1500, 100, 200);
    sleep(5);
    swipe(100, 1500, 100, 200);
    sleep(5);
    swipe(100, 1500, 100, 200);
    sleep(5);
    swipe(100, 1500, 100, 200);
    sleep(5);
    swipe(100, 1500, 100, 200);
    sleep(5);
    swipe(100, 1500, 100, 200);
    sleep(5);

    double end_time = get_current_time();
 

    double final_power = measure_power_consumption();
    printf("power_batterystate: %.6f W\n", final_power);
    
    double duration = end_time - start_time;
    printf("time: %.6f seconds\n", duration);
    
    double energy_consumed = (initial_power + final_power) / 2 * duration;
    printf("energy_batterystate: %.6f J\n", energy_consumed);
    long end_value = read_counter();
    
    double energy_charge_counter = 0.0138 *  (end_value - start_value);
    double power_charge_counter = energy_charge_counter / duration;
    printf("Power_charge_counter Energy_charge_chounter Time\n%lf %lf %lf\n", power_charge_counter, energy_charge_counter, duration);
    
    printf("\nEnd rails data:\n");
    read_energy_values();
    return 0;
}

