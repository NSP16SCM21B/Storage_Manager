Storage Manager :
-----------------

The goal of this assignment is to implement a simple storage manager - a module that is capable of reading blocks from a file on disk into memory and writing blocks from memory to a file on disk. The storage manager deals with pages (blocks) of fixed size (PAGE_SIZE). In addition to reading and writing pages from a file, it provides methods for creating, opening, and closing files. The storage manager has to maintain several types of information for an open file: The number of total pages in the file, the current page position (for reading and writing), the file name, and a POSIX file descriptor or FILE pointer. In your implementation you should implement the interface described below. Please commit a text file README.txt that (shortly) describes the ideas behind your solution and the code structure. Comment your code!

Group Members:
--------------

1) Dalin Yang - 

2) Nikhil Birur Lakshminarayana - nlakshm3@hawk.iit.edu - A20357121  

3) Utkarsh Khanna 

4) Yongjie Kang

Description of the Methods used and their implementation:
---------------------------------------------------------


1) createPageFile()

Create a new page file fileName. The initial file size should be one page. This method should fill this single page with '\0' bytes. 

	a) Generate File Descriptor
	b) Create a single page with '\0' bytes.
	c) Write the single page to page file.
	d) Close file descriptor.

Return Value : RC_FILE_NOT_FOUND, if file is not found ; RC_WRITE_FAILED, if writing to   File fails, else RC_OK
*******************************************************************************************
2) openPageFile() 

Opens an existing page file. Should return RC_FILE_NOT_FOUND if the file does not exist. The second parameter is an existing file handle. If opening the file is successful, then the fields of this file handle should be initialized with the information about the opened file. For instance, you would have to read the total number of pages that are stored in the file from disk.

	a) Check if File exists.
	b) assign current file descriptor to page file handle.
	c) Get fileName, totalNumPages, and initialize current page
	d) Close file descriptor.

Return Value : RC_FILE_NOT_FOUND, if file is not found else RC_OK

*******************************************************************************************
3) closePageFile() 

Close an open page file.

	a) It closes the current page file.

Return Value : RC_FILE_NOT_FOUND, if file is not found else RC_OK
*******************************************************************************************
4) destroyPageFile()

Deleted an open page file.

	a) It removes the current page file.

Return Value : RC_FILE_NOT_FOUND, if file is not found else RC_OK

*******************************************************************************************
5) readBlock()

The method reads the pageNumth block from a file and stores its content in the memory pointed to by the memPage page handle. If the file has less than pageNum pages, the method should return RC_READ_NON_EXISTING_PAGE.

	a) Check if file has less than pageNum pages.
	b) A function pread is used. (pread is used for reading PAGE_SIZE (here is 4096) 
	   bytes of data start from offset and assign it to memPage.Detail of this function 	   can be found here: http://pubs.opengroup.org/onlinepubs/009695399/functions/		   read.html)

Return Value : RC_READ_NON_EXISTING_PAGE, if file has less than pageNum pages else RC_OK
*******************************************************************************************
6) readFirstBlock() 

The method reads the first block of page in the file

Return Value : RC_READ_NON_EXISTING_PAGE, if file has less than pageNum pages else RC_OK
*******************************************************************************************

7) getBlockPos() 
 
The method returns the current page position in a file

Return Value : An Integer, the current page position in a file
*******************************************************************************************

8) readPreviousBlock() 

The method reads the previous block of page in the file by computing an offset.

Return Value : RC_READ_NON_EXISTING_PAGE, if file has less than pageNum pages else RC_OK

*******************************************************************************************
9) readCurrentBlock()
 
The method reads the current block of page in the file by computing an offset.

Return Value : RC_READ_NON_EXISTING_PAGE, if file has less than pageNum pages else RC_OK

*******************************************************************************************
10) readNextBlock() 

The method reads the next block of page in the file by computing an offset.

Return Value : RC_READ_NON_EXISTING_PAGE, if file has less than pageNum pages else RC_OK

*******************************************************************************************
11) readLastBlock()  

The method reads the last block of page in the file by computing an offset.

Return Value : RC_READ_NON_EXISTING_PAGE, if file has less than pageNum pages else RC_OK

*******************************************************************************************
12) writeBlock()  

The method Writes a page to disk using current position.

	a) Check if file has less than pageNum pages
	b) Compute Offset from where we need to begin writing
	c) Start writing into the page from the offset

Return Value : RC_READ_NON_EXISTING_PAGE, if file has less than pageNum pages, RC_WRITE_FAILED, if writing to a file failed else RC_OK

*******************************************************************************************
13) writeCurrentBlock() 

The method Writes a page to disk using absolute position.

	a) Check if file has less than pageNum pages
	b) Compute Offset from where we need to begin writing
	c) Start writing into the page from the offset

Return Value : RC_READ_NON_EXISTING_PAGE, if file has less than pageNum pages, RC_WRITE_FAILED, if writing to a file failed else RC_OK

*******************************************************************************************
14) appendEmptyBlock()

Increase the number of pages in the file by one. The new last page is filled with zero bytes.
	
Return Value : RC_WRITE_FAILED, if writing to a file failed else RC_OK
*******************************************************************************************
15) ensureCapacity() 

If the file has less than numberOfPages pages then the method increases the size to numberOfPages.
 
Return Value : RC_OK
*******************************************************************************************



How to run Storage Manager (Test Case 1):
------------------------------------------

1) Navigate to the terminal where the Storage Manager root folder is stored.

2) Type : make -f MakeFile_1

3) Press Return/Enter

3) Type ./storage_Manager_TC1


How to run Storage Manager (Test Case 2):
------------------------------------------

1) Navigate to the terminal where the Storage Manager root folder is stored.

2) Type : make -f MakeFile_2

3) Press Return/Enter

3) Type ./storage_Manager_TC2

