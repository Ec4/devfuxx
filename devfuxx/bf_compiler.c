#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <linux/fs.h>
#include "devfuxx_ioctl.h"

void safeexit(int *devfuxx, int *bffile)
{
	close(*devfuxx);
	close(*bffile);
	exit(1);
}

int main(int argc, char **argv)
{
	int devfuxx, bffile;
	if ((devfuxx = open("/dev/devfuxx", O_RDWR)) < 0) {
		perror("open");
		exit(1);
	}

	if ((bffile = open(argv[1], O_RDONLY)) < 0) {
		close(devfuxx);
		perror("open");
		exit(1);
	}

	char c, buf;
	while ((read(bffile, &buf, sizeof(buf))) != 0) {
		switch (buf) {
			case '>':
				if (ioctl(devfuxx, IOCTL_INC_PTR) == -1) {
					perror("ioctl(IOCTL_INC_PTR)");
					safeexit(&devfuxx, &bffile);
				}
				break;
			case '<':
				if (ioctl(devfuxx, IOCTL_DEC_PTR) == -1) {
					perror("ioctl(IOCTL_DEC_PTR)");
					safeexit(&devfuxx, &bffile);
				}
				break;
			case '+':
				if (ioctl(devfuxx, IOCTL_INC_DATA) == -1) {
					perror("ioctl(IOCTL_INC_DATA)");
					safeexit(&devfuxx, &bffile);
				}
				break;
			case '-':
				if (ioctl(devfuxx, IOCTL_DEC_DATA) == -1) {
					perror("ioctl(IOCTL_DEC_DATA)");
					safeexit(&devfuxx, &bffile);
				}
				break;
			case '.':
				if (read(devfuxx, &c, sizeof(c)) < 0) {
					perror("read");
					safeexit(&devfuxx, &bffile);
				}
				if (write(1, &c, sizeof(c)) < 0) {
					perror("write");
					safeexit(&devfuxx, &bffile);
				}
				break;
			case ',':
				if (read(0, &c, sizeof(c)) < 0) {
					perror("read");
					safeexit(&devfuxx, &bffile);
				}
				if (write(devfuxx, &c, sizeof(c)) < 0) {
					perror("write");
					safeexit(&devfuxx, &bffile);
				}
				break;
			default:
				if(isspace(buf))
					break;
				else
					printf("invalid input -> (%x)\n",buf);
		}
	}

	c = '\n';
	if (write(1, &c, sizeof(c)) < 0) {
		perror("write");
		safeexit(&devfuxx, &bffile);
	}

	close(devfuxx);
	close(bffile);

	return 0;
}
