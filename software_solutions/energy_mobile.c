#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>

// =======================================================
// calng -O2 energy_mbile.c -o energy_mobile -lpthread
// =======================================================

typedef struct {
   double energies[8];
   double t;
} energy_data_t;

int get_energy_data(energy_data_t *ed) {
   char buf[4096];
   int fd = open("/sys/bus/iio/devices/iio:device0/energy_value", O_RDONLY);
   if (fd < 0) { perror("open energy sensor"); return -1; }

   int n = read(fd, buf, sizeof(buf) - 1);
   if (n < 0) { perror("read energy sensor"); close(fd); return -1; }

   buf[n] = '\0';
   close(fd);

   for (int i = 0; i < 8; i++) ed->energies[i] = 0.0;
   ed->t = 0.0;

   char *line = strtok(buf, "\n");
   while (line) {
       if (strncmp(line, "t=", 2) == 0) {
           double time_val;
           if (sscanf(line, "t=%lf", &time_val) == 1)
               ed->t = time_val;
       } else if (strncmp(line, "CH", 2) == 0) {
           int ch;
           if (sscanf(line, "CH%d", &ch) == 1) {
               char *comma = strchr(line, ',');
               if (comma) {
                   double val;
                   if (sscanf(comma + 1, " %lf", &val) == 1) {
                       if (ch >= 0 && ch < 8)
                           ed->energies[ch] = val;
                   }
               }
           }
       }
       line = strtok(NULL, "\n");
   }
   return 0;
}

#define NUM_CORES 8
#define INITIAL_MAX_SNAPSHOTS 1024

int get_core_frequency(int cpu_id) {
   char path[128];
   snprintf(path, sizeof(path),
            "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_cur_freq", cpu_id);
   FILE *fp = fopen(path, "r");
   if (!fp) return -1;
   int freq;
   if (fscanf(fp, "%d", &freq) != 1) freq = -1;
   fclose(fp);
   return freq;
}

typedef struct {
   time_t timestamp;
   int freqs[NUM_CORES];
} snapshot_t;

static snapshot_t *snapshots = NULL;
static int snapshot_count = 0;
static int snapshot_capacity = 0;
static volatile int profiling_flag = 1;

void *frequency_profiler(void *arg) {
   (void)arg;

   snapshot_capacity = INITIAL_MAX_SNAPSHOTS;
   snapshots = malloc(snapshot_capacity * sizeof(snapshot_t));
   if (!snapshots) {
       perror("malloc snapshots");
       return NULL;
   }

   while (profiling_flag) {
       snapshot_t s;
       s.timestamp = time(NULL);
       for (int i = 0; i < NUM_CORES; i++)
           s.freqs[i] = get_core_frequency(i);

       if (snapshot_count >= snapshot_capacity) {
           snapshot_capacity *= 2;
           snapshot_t *tmp =
               realloc(snapshots, snapshot_capacity * sizeof(snapshot_t));
           if (!tmp) {
               perror("realloc snapshots");
               break;
           }
           snapshots = tmp;
       }

       snapshots[snapshot_count++] = s;
       sleep(1);
   }
   return NULL;
}



int main(int argc, char *argv[]) {
   if (argc < 2) {
       fprintf(stderr,
               "Usage: %s <program> [args...]\n", argv[0]);
       return 1;
   }

   energy_data_t energy_start, energy_end;
   struct timespec ts_start, ts_end;
   pthread_t profiler_thread;

   // Initial energy
   if (get_energy_data(&energy_start) != 0)
       fprintf(stderr, "Warning: failed initial energy read\n");

   clock_gettime(CLOCK_MONOTONIC, &ts_start);

   // Start frequency profiler
   profiling_flag = 1;
   pthread_create(&profiler_thread, NULL,
                  frequency_profiler, NULL);

   pid_t pid = fork();
   if (pid == 0) {
       execvp(argv[1], &argv[1]);
       perror("execvp failed");
       exit(1);
   }

   int status;
   waitpid(pid, &status, 0);

   profiling_flag = 0;
   pthread_join(profiler_thread, NULL);

   clock_gettime(CLOCK_MONOTONIC, &ts_end);
   if (get_energy_data(&energy_end) != 0)
       fprintf(stderr, "Warning: failed final energy read\n");


   double elapsed =
       (ts_end.tv_sec - ts_start.tv_sec) +
       (ts_end.tv_nsec - ts_start.tv_nsec) / 1e9;

   double energy_cpu =
       (energy_end.energies[3] - energy_start.energies[3]) +
       (energy_end.energies[4] - energy_start.energies[4]) +
       (energy_end.energies[5] - energy_start.energies[5]);

   double energy_all = 0.0;
   for (int i = 0; i < 8; i++)
       energy_all +=
           (energy_end.energies[i] - energy_start.energies[i]);


   energy_cpu /= 1e6;
   energy_all /= 1e6;

   double power_cpu = energy_cpu / elapsed;
   double power_all = energy_all / elapsed;


   printf("\n--- ENERGY MEASUREMENT SUMMARY ---\n");
   printf("Elapsed Time: %.3f s\n", elapsed);
   printf("Energy (CPUs only): %.6f J\n", energy_cpu);
   printf("Energy (All rails): %.6f J\n", energy_all);
   printf("Avg Power (CPUs): %.6f W\n", power_cpu);
   printf("Avg Power (All): %.6f W\n", power_all);

   printf("\n--- FREQUENCY SNAPSHOTS (%d) ---\n", snapshot_count);
   for (int i = 0; i < snapshot_count; i++) {
       printf("T=%ld :", snapshots[i].timestamp);
       for (int c = 0; c < NUM_CORES; c++)
           printf(" %d", snapshots[i].freqs[c]);
       printf("\n");
   }

   free(snapshots);
   return 0;
}
