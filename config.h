int timers[] = {
    40  * 60, // focus
    5   * 60, // break
    15  * 60, // rest
    120 * 60, // crank
};

char *timer_labels[] = {
    "focus",
    "break",
    "rest",
    "crank",
};

int alert_wait_time = 5;
int time_between_rings = 1;
int num_rings = 3;

char *pid_path = "/tmp/pomo.pid";
char *mode_path = "/tmp/pomo.mode";
char *log_path = "/tmp/pomo.log";