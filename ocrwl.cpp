#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>

using namespace std;

int main() {
	const int BSIZE = 512;
	const int NUM_REWRITES = 4;

	off_t head_pos;
	ssize_t bytes_written;
	ssize_t bytes_read;
	unsigned char buffer[BSIZE];
	
	int fd = open("foo.txt", O_RDWR);

	if (fd >= 0) {
		bytes_read = read(fd, buffer, BSIZE);
		if (bytes_read > 0) {
			unsigned char * p = buffer;
			while (p < buffer + bytes_read) {
				*(p++) += 1;
			}

			if ((head_pos = lseek(fd, 0, SEEK_SET)) != 0) {
				cout << "Attempting to seek to beginning returned: " << head_pos << endl;
			} else {
				if ((bytes_written = write(fd, buffer, bytes_read)) != bytes_read) {
					cout << "Write returned: " << bytes_written << " expected: " << bytes_read << endl;
					perror("Error writing");
				}
			}
		}
		close(fd);
	}
	return 0;
}
/*

		for (int i = 0; i < NUM_REWRITES; i++) {
			memset(buffer, i, BSIZE);
			cout << "Initialized buffer to all " << i << "'s" << endl;

			if ((head_pos = lseek(fd, 0, SEEK_SET)) != 0) {
				cout << "Attempting to seek to beginning returned: " << head_pos << endl;
				break;
			}
			//cout << "Write head: " << WhereAmI(fd) << endl;

			if ((bytes_written = write(fd, buffer, BSIZE)) != BSIZE) {
				cout << "Write returned: " << bytes_written << " expected: " << BSIZE << endl;
				perror("Error writing");
				break;
			}
			cout << "Wrote block of " << i << "'s" << endl;
			//cout << "Write head: " << WhereAmI(fd) << endl << endl;
		}
		close(fd);
	} else {
		perror("The file did not open");
	}
	return 0;

}
*/

