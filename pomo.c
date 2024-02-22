#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <raylib.h>
#include "basic.h"
#include "config.h"

bool get_new_mode = false;
int mode = 0;
char mode_str[8];
FILE *pid_file = NULL;
FILE *mode_file = NULL;
FILE *log_file = NULL;

void signal_handler(int sig) {
    switch(sig) {
        case SIGALRM:
            get_new_mode = true;
            break;
        case SIGTERM:
            fclose(log_file);
            remove(pid_path);
            remove(mode_path);
            remove(log_path);
            CloseAudioDevice();
            exit(EXIT_SUCCESS);
            break;
    }
}

int main(int argc, char **argv) {
    pid_t pid, sid;

    pid_file = fopen(pid_path, "r");

    if(pid_file) {
        if(argc > 1) {
            fscanf(pid_file, "%d\n", &pid);
            fclose(pid_file);
    
            int sig = SIGALRM;
    
            if(!strcmp("stop", argv[1])) {
                sig = SIGTERM;
            } else if(strstr("focus\vbreak\vrest\vcrank", argv[1])) {
                mode_file = fopen(mode_path, "w");
                fprintf(mode_file, "%s\n", argv[1]);
                fclose(mode_file);
            } else {
                exit(EXIT_SUCCESS);
            }

            kill(pid, sig);
            exit(EXIT_SUCCESS);
        } else {
            mode_file = fopen(mode_path, "r");
            fscanf(mode_file, "%s\n", mode_str);
            fclose(mode_file);
            printf("%s\n", mode_str);
            exit(EXIT_SUCCESS);
        }
    }

    pid = fork();
    if(pid < 0) exit(EXIT_FAILURE);

    if(pid > 0) {
        pid_file = fopen(pid_path, "w");
        fprintf(pid_file, "%d\n", pid);
        fclose(pid_file);
        exit(EXIT_SUCCESS);
    }

    sid = setsid();
    if(sid < 0) exit(EXIT_FAILURE);

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    signal(SIGALRM, signal_handler);
    signal(SIGTERM, signal_handler);

    mode_file = fopen(mode_path, "w");
    fprintf(mode_file, "%s\n", timer_labels[mode]);
    fclose(mode_file);

    log_file = fopen(log_path, "w");

    InitAudioDevice();

    Sound ring = LoadSound("/usr/local/share/pomo/bell.wav");

    while(true) {
        int cur_mode = mode;
        sleep(timers[mode]);
        while(!get_new_mode) {
            for(int i = 0; i < num_rings; ++i) {
                PlaySound(ring);
                sleep(time_between_rings);
            }
            sleep(alert_wait_time);
        }
        get_new_mode = false;
        mode_file = fopen(mode_path, "r");
        fscanf(mode_file, "%s\n", mode_str);
        fclose(mode_file);
        for(int i = 0; i < STATICARRLEN(timer_labels); ++i) {
            if(!strcmp(mode_str, timer_labels[i]))
                mode = i;
        }
        fprintf(log_file, "changing mode from %s to %s\n", timer_labels[cur_mode], timer_labels[mode]);
        fflush(log_file);
    }

    fclose(log_file);
    CloseAudioDevice();

    return 0;
}
