# ocrwl

You may have not written any systems-like code in your young careers. Network programming, the way we are going to learn it, is all about systems-style code - that means low level - and pointers.

In the warm-up program you are asked to write a file that is comprised of 16 512 byte blocks, a total of 8KB in all. Then, read the file back in and for each 512 byte block, swap it for another 512 byte block. In this document I'll take you through the parts of doing this.

*I just noticed I am writing binary to a file called foo.txt - oops*

# open

The open system call returns an integer called a file descriptor. File descriptors are pretty cool but that's for the Operating Systems course. If you get back a value of less than 0, you've got an error. Check out the use of ```perror``` below. ```perror``` comes from the ```errno.h``` include file.

[open](http://man7.org/linux/man-pages/man2/open.2.html)

[errno](http://man7.org/linux/man-pages/man3/errno.3.html)

Here is a sample of opening a file for writing. Using this syntax, the file must already exist. It's old contents will be destroyed.

```c
int fd = open("file.txt", O_WRONLY);
```
Here's a version that will create the file if it was not already present:

```c
int fd = open("file.txt", O_WRONLY | O_CREAT);
```

Notice the use of the bitwise or operator ```|```. 

Here's one that opens a file for both reading and writing:

```c
int fd = open("file.txt", O_RDWR);
```

# close

close() is simply ```close(fd);```.

[close](http://man7.org/linux/man-pages/man2/close.2.html)

# Putting the two together

```c
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

/* You want this too */
#include <errno.h>

int main() {
	int fd = open("foo.txt", O_WRONLY);

	if (fd >= 0) {
		std::cout << "Yay! It is open!" << std::endl;
		close(fd);
	} else {
		perror("The file did not open");
	}
	return 0;
}
```

Compile like this: ```g++ open_close.cpp``` I used ```g++``` so I could get ```cout```.

Run the program, you should get this:
```
The file did not open: No such file or directory
```

This is the correct result.

Change:
```
int fd = open("foo.txt", O_WRONLY);
```

to
```
int fd = open("foo.txt", O_WRONLY | O_CREAT);
```

And compile again. Run it. You'll get:
```
Yay! It is open!
```

Now do an ```ls```. You'll see ```foo.txt``` in your directory.

# A buffer

Making a buffer of a specific size sounds like a job for an array of ```char```. We're interested in binary values so it is more appropriate to use ```unsigned char.```

## Defining the size

To set the (constant) block size you might do this the old fashioned way:

```c
#define	BSIZE	512
```

or the C++ way:

```c++
const int BSIZE = 512;
```

## Allocating the memory

You might do it like this:

```c
unsigned char buffer[BSIZE];
```

or you might consider doing it with dynamically allocated memory (such as with ```new``` and ```delete```). More on dynamic (much ) later.

Now you have exactly 512 bytes.

## Initializing the block to a constant

You can certainly write a loop that puts zero or one or whatever in each byte. But why do that? Use ```memset``` instead.

Notice ```memset``` comes from chapter 3 of the manual. There is a pattern to the Unix manual. Chapter 2 leads to system calls and Chapter 3 leads to library calls from the standard library.

[memset](http://man7.org/linux/man-pages/man3/memset.3.html)

*Warning* ```memset``` is dangerous. If you exceed the length of your buffer, you are overwriting the memory of whatever comes after it.

```c++
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

/* You want this too */
#include <errno.h>

int main() {
	const int BSIZE = 512;
	int fd = open("foo.txt", O_CREAT | O_WRONLY);
	unsigned char buffer[BSIZE];

	if (fd >= 0) {
		std::cout << "Yay! It is open!" << std::endl;
		memset(buffer, 0, BSIZE);
		// Notice no std::endl...
		std::cout << "Initialized to zero\n";
		close(fd);
	} else {
		perror("The file did not open");
	}
	return 0;
}
```

# write

write() requires a file descriptor, a pointer to data and the number of bytes to write.

[write](http://man7.org/linux/man-pages/man2/write.2.html)

```c++
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

/* You want this too */
#include <errno.h>

int main() {
	const int BSIZE = 512;
	int fd = open("foo.txt", O_CREAT | O_WRONLY);
	unsigned char buffer[BSIZE];

	if (fd >= 0) {
		std::cout << "Yay! It is open!" << std::endl;
		memset(buffer, 0, BSIZE);
		// Notice no std::endl...
		std::cout << "Initialized to zero\n";
		write(fd, buffer, BSIZE);
		std::cout << "Wrote zeros\n";
		close(fd);
	} else {
		perror("The file did not open");
	}
	return 0;
}
```

In ```write(fd, buffer, BSIZE);``` where is the pointer? ```buffer``` is an array - the name of an array is a pointer to the data in the array.

Below you'll see the output of the program. Then, notice the file size is exactly 512. And finally, it contains only binary zeros.

```
./a.out
Yay! It is open!
Initialized to zero
Wrote zeros
hyde warmup $> ls -l
total 56
-rwxr-xr-x  1 perrykivolowitz  staff  16104 Feb  8 18:45 a.out
-rw-r--r--  1 perrykivolowitz  staff   2300 Feb  8 17:57 fileio.c
-rw-------  1 perrykivolowitz  staff    512 Feb  8 18:47 foo.txt
-rw-r--r--@ 1 perrykivolowitz  staff    513 Feb  8 18:45 open_close.cpp
hyde warmup $> od -X foo.txt 
0000000          00000000        00000000        00000000        00000000
*
0001000
hyde warmup $> 
```

But this is not good enough. ```write()``` returns a value. It's value must be checked to ensure the write was successful.

Here is a section from the man page for ```write()```:

>On success, the number of bytes written is returned (zero indicates nothing was written). On error, -1 is returned, and errno is set appropriately.

There are lots of reasons that writes can fail. Good programmers who do not wish to bring about ```end times``` will check for errors. So, the following is better:

```c++
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

/* You want this too */
#include <errno.h>

int main() {
	const int BSIZE = 512;
	ssize_t bytes_written;
	int fd = open("foo.txt", O_CREAT | O_WRONLY);
	unsigned char buffer[BSIZE];

	if (fd >= 0) {
		std::cout << "Yay! It is open!" << std::endl;
		memset(buffer, 0, BSIZE);
		// Notice no std::endl...
		std::cout << "Initialized buffer to zero\n";

		bytes_written = write(fd, buffer, BSIZE);
		if (bytes_written == BSIZE)
			std::cout << "Wrote zeros\n";
		else
			perror("Error writing zeros");

		close(fd);
	} else {
		perror("The file did not open");
	}
	return 0;
}
```

Or perhaps:

```c++
if ((bytes_written = write(fd, buffer, BSIZE)) == BSIZE)
	std::cout << "Wrote zeros\n";
else
	perror("Error writing zeros");
```

*Warning* ```write``` is dangerous. If you exceed the length of your buffer, you are exposing the memory of whatever comes after it possibly leaking secrets.

# lseek

When you opened the file for writing, the "write head" was at position 0. When you wrote 512 bytes, the "write head" moved to 512. Let's confirm this.

```c++
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

/* You want this too */
#include <errno.h>

using namespace std;

off_t WhereAmI(int fd) {
	// This causes a seek to zero bytes away
	// from where we are right now.
	return lseek(fd, 0, SEEK_CUR);
}

int main() {
	const int BSIZE = 512;
	ssize_t bytes_written;
	int fd = open("foo.txt", O_CREAT | O_WRONLY);
	cout << "Write head: " << WhereAmI(fd) << endl;

	unsigned char buffer[BSIZE];

	if (fd >= 0) {
		std::cout << "Yay! It is open!" << std::endl;
		memset(buffer, 0, BSIZE);
		// Notice no std::endl...
		std::cout << "Initialized buffer to zero\n";

		if ((bytes_written = write(fd, buffer, BSIZE)) == BSIZE) {
			std::cout << "Wrote zeros\n";
			cout << "Write head: " << WhereAmI(fd) << endl;
		}
		else
			perror("Error writing zeros");

		close(fd);
	} else {
		perror("The file did not open");
	}
	return 0;
}
```

The output:

```
hyde warmup $> ./a.out
Write head: 0
Yay! It is open!
Initialized buffer to zero
Wrote zeros
Write head: 512
hyde warmup $>
```

```lseek``` is the lowest level way of moving around where the next read or write will take place. It takes three arguments: which file to move around in (the fd), an offset, how to measure the offset.

[lseek](https://linux.die.net/man/2/lseek) - I'll stop linking man pages after this - you know how to source this information now.

Using ```lseek``` you can ask to go some number of bytes away from the beginning of the file or the end of the file or from where you are right new. ```lseek``` returns the new position so what I did above is to seek 0 bytes away from where I am right now.

*In case you are wondering, you can seek beyond the end of a file but not before its beginning.*

### Better style for ```WhereAmI()```

Here I am adding an ```assert```. ```assert``` is your friend.

```c++
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>

using namespace std;

off_t WhereAmI(int fd) {
	assert(fd >= 0);
	// This causes a seek to zero bytes away
	// from where we are right now.
	return lseek(fd, 0, SEEK_CUR);
}
```

Please ask me to talk about it in class.

### And while we're at it

Have you seen this?

```c++
inline off_t WhereAmI(int fd) {
	assert(fd >= 0);
	// This causes a seek to zero bytes away
	// from where we are right now.
	return lseek(fd, 0, SEEK_CUR);
}
```

I added the ```inline```. Please ask me about this if interested.

# Let's tighten up this sample program

I am going to refactor this to write the same block over and over again (4 times) incrementing the value of the data within the block. You will see how to use ```lseek``` to move around the file.

Here is the output:

```
hyde warmup $> g++ open_close.cpp 
hyde warmup $> ./a.out
Yay! It is open!
Initialized buffer to all 0's
Write head: 0
Wrote block of 0's
Write head: 512

Initialized buffer to all 1's
Write head: 0
Wrote block of 1's
Write head: 512

Initialized buffer to all 2's
Write head: 0
Wrote block of 2's
Write head: 512

Initialized buffer to all 3's
Write head: 0
Wrote block of 3's
Write head: 512

hyde warmup $> od -X foo.txt 
0000000          03030303        03030303        03030303        03030303
*
0001000
hyde warmup $> 
```

Here is the refactored program:

```c++
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>

using namespace std;

/*	WhereAmI() - this function returns the head position of the
	file referred to by the given file descriptor.

	Parameters:
	int fd	The aforemention file descriptor which is assumed to be valid.

	Returns:
	int		The current position of the I/O head.
*/

inline off_t WhereAmI(int fd) {
	assert(fd >= 0);
	return lseek(fd, 0, SEEK_CUR);
}

int main() {
	const int BSIZE = 512;
	const int NUM_REWRITES = 4;

	off_t head_pos;
	ssize_t bytes_written;
	unsigned char buffer[BSIZE];
	
	int fd = open("foo.txt", O_CREAT | O_WRONLY);

	if (fd >= 0) {
		std::cout << "Yay! It is open!" << std::endl;
		for (int i = 0; i < NUM_REWRITES; i++) {
			memset(buffer, i, BSIZE);
			cout << "Initialized buffer to all " << i << "'s" << endl;

			if ((head_pos = lseek(fd, 0, SEEK_SET)) != 0) {
				cout << "Attempting to seek to beginning returned: " << head_pos << endl;
				break;
			}
			cout << "Write head: " << WhereAmI(fd) << endl;

			if ((bytes_written = write(fd, buffer, BSIZE)) != BSIZE) {
				cout << "Write returned: " << bytes_written << " expected: " << BSIZE << endl;
				perror("Error writing");
				break;
			}
			cout << "Wrote block of " << i << "'s" << endl;
			cout << "Write head: " << WhereAmI(fd) << endl << endl;
		}
		close(fd);
	} else {
		perror("The file did not open");
	}
	return 0;
}
```

# read()

Now I will rewrite the program to read an existing file (since foo.txt is around now), add one the the value and write it back.

```read()``` looks just like ```write()```. Remember to check for errors. Also remember that it is OK to read fewer bytes than you expected in general because, for example, you may have reached the end-of-file. From the man page:

>On success, the number of bytes read is returned (zero indicates end of file), and the file position is advanced by this number. It is not an error if this number is smaller than the number of bytes requested; this may happen for example because fewer bytes are actually available right now (maybe because we were close to end-of-file, or because we are reading from a pipe, or from a terminal), or because read() was interrupted by a signal. On error, -1 is returned, and errno is set appropriately. In this case it is left unspecified whether the file position (if any) changes.

```c++
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
```

Here's the output:

```
hyde warmup $> !od
od -X foo.txt 
0000000          03030303        03030303        03030303        03030303
*
0001000
hyde warmup $> ./a.out
hyde warmup $> od -X foo.txt 
0000000          04040404        04040404        04040404        04040404
*
0001000
hyde warmup $> ./a.out
hyde warmup $> od -X foo.txt 
0000000          05050505        05050505        05050505        05050505
*
0001000
hyde warmup $> 
```

Notice how the single 512 byte block has each byte going up in value by 1.

**This completes the discussion of open, close, read, write and lseek**

# That wicked looking ```while``` loop

```c++
unsigned char * p = buffer;
while (p < buffer + bytes_read) {
	*(p++) += 1;
}
```

What's cool about this?

Ask me about this in class.
 
