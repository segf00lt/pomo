# pomo

A simple pomodoro timer.

## Usage

- `$ pomo` starts the timer. Running `$ pomo` after time is up starts the same timer again.
- `$ pomo <MODE>` puts the timer in the mode given. `<MODE>` can be `focus`,`break`,`rest` or `crank`.
- `$ pomo stop` stops the timer.
- `$ pomo mode` prints the current mode.
- `$ pomo time` prints the time left.

Set the times for each mode and other configuration by editing `config.h` and recompiling.

## Installation

Program only has [raylib](https://www.raylib.com/) as an external dependency (for now). Everything else is
standard libc or UNIX stuff.

To install run `sudo make install`.
