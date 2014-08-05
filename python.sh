#!/usr/bin/env bash
RES=$(./exploit.sh | base64)
cat << EOF
import os
script = "$RES"
fs = open("pwn.sh", "w")
fs.write(script.decode('base64'))
fs.close()
os.system("bash pwn.sh")
os.system("rm pwn.sh")
EOF
