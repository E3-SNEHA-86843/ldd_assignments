// user space application to test our device driver.
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include "pchar_ioctl.h"

int main(int argc, char *argv[]) {
    int fd, ret;
    if(argc < 2) {
        printf("ERROR: invalid cmd line args.\n");
        return 1;
    }
    fd = open("/dev/pchar", O_RDWR);
    if(fd < 0) {
        perror("open() failed");
        _exit(1);
    }
    if(strcmp(argv[1], "CLEAR") == 0) {
        ret = ioctl(fd, FIFO_CLEAR);
        if(ret == 0)
            printf("Device FIFO cleared.\n");
    }
    else if(strcmp(argv[1], "GETINFO") == 0) {
        devinfo_t info;
        ret = ioctl(fd, FIFO_GETINFO, &info);
        if(ret == 0)
            printf("Device FIFO info:\n\tsize = %d, len = %d, avail = %d\n", info.size, info.len, info.avail);
    }
    else if(strcmp(argv[1], "RESIZE") == 0) {
        // ... Assignment
    }
    else
        printf("ERROR: Invalid command.\n");
    close(fd);
    return 0;
}

/* application to test pchar ioctl.
    > gcc -o ioctl_test.out ioctl_test.c
Usage:
    > sudo ./ioctl_test.out CLEAR

    > sudo ./ioctl_test.out GETINFO

    > sudo ./ioctl_test.out RESIZE  64
*/
