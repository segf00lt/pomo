#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <raylib.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "config.h"

bool timer_pause;
bool get_command;
bool get_response;
bool timer_finished;
int mode;
int time_left;
int time_elapsed;
enum PomoCMD cmd;
FILE *pid_file;
int fifo_fd;
Sound bell;
Sound click;

void signal_handler(int sig) {
    switch(sig) {
        case SIGALRM:
            get_command = true;
            break;
        case SIGTERM:
            remove(fifo_path);
            remove(pid_path);
            UnloadSound(bell);
            UnloadSound(click);
            CloseAudioDevice();
            exit(EXIT_SUCCESS);
            break;
    }
}

void control(int argc, char **argv) {
    pid_t pid = 0;

    fscanf(pid_file, "%d\n", &pid);
    fclose(pid_file);

    cmd = RESET;

    if(argc > 1) {
        if(!strcmp("stop", argv[1])) {
            kill(pid, SIGTERM);
            exit(EXIT_SUCCESS);
        } else if(!strcmp("focus", argv[1])) {
            cmd = FOCUS;
        } else if(!strcmp("break", argv[1])) {
            cmd = BREAK;
        } else if(!strcmp("rest", argv[1])) {
            cmd = REST;
        } else if(!strcmp("crank", argv[1])) {
            cmd = CRANK;
        } else if(!strcmp("pause", argv[1])) {
            cmd = PAUSE;
        } else if(!strcmp("mode", argv[1])) {
            cmd = MODE;
        } else if(!strcmp("time", argv[1])) {
            cmd = TIME;
        } else {
            exit(EXIT_SUCCESS);
        }
    }

    kill(pid, SIGALRM);
    fifo_fd = open(fifo_path, O_WRONLY);
    write(fifo_fd, &cmd, sizeof(cmd));
    close(fifo_fd);

    if(cmd < MODE) exit(EXIT_SUCCESS);

    fifo_fd = open(fifo_path, O_RDONLY);

    if(cmd == MODE) {
        read(fifo_fd, &mode, sizeof(mode));
        close(fifo_fd);
        printf("%s\n", timer_labels[mode]);
    } else {
        read(fifo_fd, &time_left, sizeof(time_left));
        close(fifo_fd);
        printf("%02d:%02d\n", time_left / 60, time_left % 60);
    }

    exit(EXIT_SUCCESS);
}

void pomodaemon(void) {
    pid_t pid = 0, sid = 0;
    struct timespec start_time, cur_time;

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

    InitAudioDevice();

    bell = LoadSound("/usr/local/share/pomo/bell.wav");
    click = LoadSound("/usr/local/share/pomo/click.wav");

    mkfifo(fifo_path, 0666);

    time_left = timers[0];

    PlaySound(click);

    while(true) {
        if(!timer_pause && !timer_finished) {
            clock_gettime(CLOCK_MONOTONIC, &start_time);
            sleep(time_left);
            clock_gettime(CLOCK_MONOTONIC, &cur_time);
            time_elapsed = cur_time.tv_sec - start_time.tv_sec;
            timer_finished = (time_elapsed >= time_left);

            if(timer_finished && !get_command) {
                for(int i = 0; i < num_rings; ++i) {
                    PlaySound(bell);
                    sleep(time_between_rings);
                }
            }
        }

        if(get_command) {
            get_command = false;

            fifo_fd = open(fifo_path, O_RDONLY);
            read(fifo_fd, &cmd, sizeof(cmd));
            close(fifo_fd);

            switch(cmd) {
                default:
                    break;
                case FOCUS: case BREAK: case REST: case CRANK:
                    mode = cmd;
                case RESET:
                    timer_finished = false;
                    time_left = timers[mode];
                    PlaySound(click);
                    break;
                case PAUSE:
                    timer_pause = !timer_pause;
                    PlaySound(click);
                    break;
                case MODE:
                    timer_finished = false;
                    clock_gettime(CLOCK_MONOTONIC, &cur_time);
                    time_left -= cur_time.tv_sec - start_time.tv_sec;
                    fifo_fd = open(fifo_path, O_WRONLY);
                    write(fifo_fd, &mode, sizeof(mode));
                    close(fifo_fd);
                    break;
                case TIME:
                    if(timer_finished)
                        time_left = 0;
                    else if(!timer_pause)
                        time_left -= time_elapsed;
                    fifo_fd = open(fifo_path, O_WRONLY);
                    write(fifo_fd, &time_left, sizeof(time_left));
                    close(fifo_fd);
                    break;
            }
        }
    }

    UnloadSound(bell);
    UnloadSound(click);
    CloseAudioDevice();

    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
    pid_file = fopen(pid_path, "r");

    if(pid_file)
        control(argc, argv);
    else
        pomodaemon();

    return 0;
}
