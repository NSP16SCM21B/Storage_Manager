
Description and Implementation of the functions:
------------------------------------------------

1) createPageFile() 

i)Creates a new page in the file. 
ii)Opens the page in write mode and fills the page with '\0' bytes upto the PAGE_SIZE.
iii)If the files is not found ,returns RC_FILE_NOT_FOUND.

2) openPageFile() 

i)Opens an existing page file .
ii)If found and initialises the handler with the information in the file like filename,current page position, total number of pages.
iii)If the file is not found , returns RC_FILE_NOT_FOUND.

3) closePageFile() 

i)It closes the current page file using fclose().

4) destroyPageFile()

i)It destroys the page in the file using remove() and returns RC_OK upon success.
ii)If the file is not found returns RC_FILE_NOT_FOUND.

5) readBlock()

i)Reads the page from the start to PAGE_SIZE as per the page number information provided.
ii)If the page number is greater than the total number of pages, returns RC_READ_NON_EXISTING_PAGE. 
iii)If the file is not found, returns RC_FILE_NOT_FOUND.
iv)If the page is successfully read it returns RC_OK.

6) readFirstBlock() 

i)Reads the first block till the end. 
ii)Current cursor position is set to 0 as it is the first block.
iii) readBlock() has been called recursively


7) getBlockPos() 
 
i)returns current cursor position inside the file.


8) readPreviousBlock() 

i) Computes the previous cursor position based on the cursor position obtained from getBlockPos()
ii) readBlock() has been called recursively

9) readCurrentBlock()
 
i) Sets the current cursor position of the block using getBlockPos().
ii) readBlock() has been called recursively

10) readNextBlock() 

i) Computes the next cursor position based on the cursor position obtained from getBlockPos()
ii) readBlock() has been called recursively


11) readLastBlock()  

i) Computes the last cursor position based on the cursor position obtained from getBlockPos()
ii) readBlock() has been called recursively

12) writeBlock()  

i)It opens the file and writes the contents page handler to the file.
ii)If the Pagenumber exceeds the Total number of Pages, returns RC_READ_NON_EXISTING_PAGE .
iii)If the file is not found, returns RC_FILE_NOT_FOUND.

13) writeCurrentBlock() 

i) Fetch the current cursor position using getBlockPos().
ii)writeBlock() has been called recursively

14) appendEmptyBlock()

i)Appends the file with the new empty block initilizing it to '\0' Bytes.
ii) Here createPageFile() has been recursively called
	
15) ensureCapacity() 

i)If the required number of blocks exceed the total number of blocks present then increase the existing number of blocks to the required number.
 
ii)Here createPageFile() has been recursively called.


Steps to run Storage Manager:
-----------------------------

1) In the terminal, navigate to the directory.

2) Type: 
	make -f makefile1

3) ./storageManager1


