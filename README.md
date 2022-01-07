# Shell-Project
For this project, I built a command interpreter (shell) that reads commands 
from standard input and executes these commands. Command that end with a '&' 
are run in the background while commands that do not end in a '&' are run as
foreground commands (shell waits for it to complete before proceeding).

When a background command completes, the shell will print the exit code from
the process, or if the process is exited due to a signal, the signal that 
killed the process.
