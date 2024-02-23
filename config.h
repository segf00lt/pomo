int timers[] = {
    40  * 60, // focus
    5   * 60, // break
    15  * 60, // rest
    120 * 60, // crank
};

enum PomoCMD {
    FOCUS = 0,
    BREAK,
    REST,
    CRANK,
    PAUSE,
    RESET,
    MODE,
    TIME,
};

char *timer_labels[] = {
    "focus",
    "break",
    "rest",
    "crank",
    "pause",
};

int time_between_rings = 1;
int num_rings = 3;

char *pid_path = "/tmp/pomo.pid";
char *fifo_path = "/tmp/pomo.fifo";
