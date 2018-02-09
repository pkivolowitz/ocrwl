# ocrwl

You may have not written any systems-like code in your young careers. Network programming, the way we are going to learn it, is all about systems-style code - that means low level - and pointers. Lots of pointers.

In this program you are asked to write a file that is comprised of 16 512 byte blocks, a total of 8KB in all. Then, read the file back in and for each 512 byte block, swap it for another 512 byte block.

# open

The open system call returns an integer called a file descriptor. File descriptors are pretty cool but that's for the Operating systems course. If you get back a value of less than 0, you've got an error.

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




