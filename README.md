# ocrwl

You may have not written any systems-like code in your young careers. Network programming, the way we are going to learn it, is all about systems-style code - that means low level - and pointers. Lots of pointers.

In this program you are asked to write a file that is comprised of 16 512 byte blocks, a total of 8KB in all. Then, read the file back in and for each 512 byte block, swap it for another 512 byte block.

# open

The open system call returns an integer called a file descriptor. File descriptors are pretty cool but that's for the Operating Systems course. If you get back a value of less than 0, you've got an error. Check out the use of ```perror``` below.

[open](http://man7.org/linux/man-pages/man2/open.2.html)

Here is a sample of opening a file for writing. Using this syntax, the file must already exist. It's old contents will be destroyed.

```c
int fd = open("file.txt", O_WRONLY);
```
Here's a version that will create the file if it was not already present:

```c
int fd = open("file.txt", O_WRONLY | O_CREAT);
```

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

Making a buffer of a specific size sounds like a job for and array of ```char```. We're interested in binary values so it is more appropriate to use ```unsigned char.```

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

