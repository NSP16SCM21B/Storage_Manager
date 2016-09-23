#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

#include "storage_mgr.h"
#include "dberror.h"
#include "test_helper.h"

#define TOTAL_PAGES 1000 //Define Page Size to be 1000

/* Initialize page file handle and page handle */
SM_FileHandle *fHandle;
SM_PageHandle memPage;


/* Method that initializes the Storage Manager */
void initStorageManager (void) {

	// Allocate space for the Page and File  handles.
	fHandle = (SM_FileHandle *) malloc(sizeof(SM_FileHandle *));
	memPage = (SM_PageHandle) malloc(sizeof(SM_PageHandle)); 

};
/*
******************************************************************************************************************
**
**	Method Name :createPageFile
**	Description: Create a new page file. The initial file size is one page. This method will fill this single page with '\0' bytes.
**	Input Parameters : pointer to fileName of type character - char *fileName
**	Return Value : RC_OK | RC_FILE_NOT_FOUND | RC_WRITE_FAILED 
**
******************************************************************************************************************
*/
RC createPageFile (char *fileName) {

	// mode defines the permisisons on a file descriptor
	// S_IRUSR file owner has read, write, and execute permissions.
	int mode = S_IRWXU;

	// generate a new file descriptor.
	int fd = creat(fileName, mode);

	// fd is a non-negative integer if the file descriptor is generated successfully.
	if (fd < 0) {
		printf("file not found.\n");
		return RC_FILE_NOT_FOUND;
	}

	// create a single page with '\0' bytes.
	char data[PAGE_SIZE];
	memset(data,'\0',sizeof(data));

	// write the single page to page file.
	if (write(fd, data, PAGE_SIZE) != PAGE_SIZE) {
		printf("Error writing to file %s\n", fileName);
		return RC_WRITE_FAILED;
	};

	// close file descriptor.
	close(fd);
	return RC_OK;

};
/*
******************************************************************************************************************
**
**      Method Name :openPageFile
**      Description: Opens an existing page file
**      Input Parameters : pointer to fileName of type character - char *fileName and second parameter is an existing file handle
**      Return Value : RC_OK | RC_FILE_NOT_FOUND
**
******************************************************************************************************************
*/
RC openPageFile (char *fileName, SM_FileHandle *fHandle) {

	// flag defines file access mode.
	// O_RDWR read and write mode.
	// O_APPEND append mode, used for appending new block to page file.
	int flag = O_RDWR|O_APPEND;
	int fd = open(fileName, flag);

	// fcntl gets or changes the file status, the second parameter "F_GETFL"
	// is used to get the flag, if the file is opened correctly, it returns a 
	// non-negative integer.
	if (fcntl(fd, F_GETFL) < 0) {
		return RC_FILE_NOT_FOUND;
	}

	// assign current fd to page file handle.
	fHandle->mgmtInfo = (void *)fd;

	// obtain file size via file descriptor.
	struct stat sat;
	fstat(fd, &sat);
	int size = sat.st_size;
	
	// assign values to fileName, totalNumPages, and initialize current page 
	// position as 0.
	fHandle->fileName = fileName;
	fHandle->totalNumPages = size/PAGE_SIZE;
	fHandle->curPagePos = 0;


	// close file descriptor.
	close(fd);

	return RC_OK;
}
/*
******************************************************************************************************************
**
**      Method Name :closePageFile
**      Description: Close a page file
**      Input Parameters :An existing file handle
**      Return Value : RC_OK | RC_FILE_NOT_FOUND 
**
******************************************************************************************************************
*/
RC closePageFile (SM_FileHandle *fHandle) {

	// access file descriptor by page file handle.
	int fd = fHandle->mgmtInfo;

	// close function returns 0 if the file descriptor is closed.
	if (close(fd) == 0) {
		return RC_OK;
	}
	// otherwise, return error code. 
	return RC_FILE_NOT_FOUND;
};
/*
******************************************************************************************************************
**
**      Method Name :destroyPageFile
**      Description: Destroys an existing page file
**      Input Parameters : filename
**      Return Value : RC_OK | RC_FILE_NOT_FOUND 
**
******************************************************************************************************************
*/
RC destroyPageFile (char *fileName) {
	// destroy file.
	int r = remove(fileName);
	if (r == 0) {
		return RC_OK;
	}
	return RC_FILE_NOT_FOUND;
}
/*
******************************************************************************************************************
**
**      Method Name :readBlock
**      Description: The method reads the "pageNum"th block from a file and stores its content in the memory pointed to by the memPage page handle.
**      Input Parameters : An Integer "pageNum", An existing file handle and a Page handle
**      Return Value : RC_OK | RC_READ_NON_EXISTING_PAGE
**
******************************************************************************************************************
*/
RC readBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage) {
	// if pageNum is greater than total number of pages in the file, return error.
	if (pageNum > (fHandle->totalNumPages)) {
		return RC_READ_NON_EXISTING_PAGE;
	}	

	// define offset used for finding and manipulating the particular page. 
	off_t offset = pageNum * PAGE_SIZE;

	// pread is used for reading PAGE_SIZE (here is 4096) bytes of data start
	// from offset and assign it to memPage.
	//
	// detail of this function can be found here:
	// http://pubs.opengroup.org/onlinepubs/009695399/functions/read.html
	if (pread(fHandle->mgmtInfo, memPage, PAGE_SIZE, offset) > 0 ) {
		return RC_OK;
	}
	else {
		return RC_READ_NON_EXISTING_PAGE;
	}
}
/*
******************************************************************************************************************
**
**      Method Name : getBlockPos
**      Description: The method returns the current page position in a file
**      Input Parameters : An existing file handle
**      Return Value : An Integer, the current page position in a file
**
******************************************************************************************************************
*/
int getBlockPos (SM_FileHandle *fHandle) {
	return fHandle->curPagePos;
}
/*
******************************************************************************************************************
**
**      Method Name : readFirstBlock
**      Description: The method reads the first block of page in the file
**      Input Parameters : An existing file handle and a Pahe handle
**      Return Value : RC_OK | RC_READ_NON_EXISTING_PAGE
**
*******************************************************************************************************************
*/
RC readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {

	if (pread(fHandle->mgmtInfo, memPage, PAGE_SIZE, 0) > 0) {
		return RC_OK;
	}
	else {
		return RC_READ_NON_EXISTING_PAGE;
	}
}
/*
******************************************************************************************************************
**
**      Method Name : readPreviousBlock
**      Description: The method reads the previous block of page in the file
**      Input Parameters : An existing file handle and a Page Handle
**      Return Value : RC_OK | RC_READ_NON_EXISTING_PAGE
**
******************************************************************************************************************
*/
RC readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {
	if (fHandle->curPagePos == 0) {
		return RC_READ_NON_EXISTING_PAGE;
	}	
	
	off_t offset = (fHandle->curPagePos - 1) * PAGE_SIZE;

	if (pread(fHandle->mgmtInfo, memPage, PAGE_SIZE, offset) < 0) {
		return RC_READ_NON_EXISTING_PAGE;
	}

	return RC_OK;
}
/*
******************************************************************************************************************
**
**      Method Name : readCurrentBlock
**      Description: The method reads the current block of page relative to the curPagePos of the file. 
**      Input Parameters : An existing file handle and a Page Handle
**      Return Value : RC_OK | RC_READ_NON_EXISTING_PAGE
**
******************************************************************************************************************
*/
RC readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {
	off_t offset = fHandle->curPagePos * PAGE_SIZE;
	if (pread(fHandle->mgmtInfo, memPage, PAGE_SIZE, offset) < 0) {
		return RC_READ_NON_EXISTING_PAGE;
	}	
	return RC_OK;
}
/*
******************************************************************************************************************
**
**      Method Name : readNextBlock
**      Description: The method reads the next block of page relative to the curPagePos of the file. 
**      Input Parameters : An existing file handle and a Page Handle
**      Return Value : RC_OK | RC_READ_NON_EXISTING_PAGE
**
******************************************************************************************************************
*/
RC readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {
	if (fHandle->curPagePos == fHandle->totalNumPages) {
		return RC_READ_NON_EXISTING_PAGE;
	}	
	off_t offset = (fHandle->curPagePos + 1) * PAGE_SIZE;

	if (pread(fHandle->mgmtInfo, memPage, PAGE_SIZE, offset) < 0) {
		return RC_READ_NON_EXISTING_PAGE;
	}
	return RC_OK;
		
}
/*
******************************************************************************************************************
**
**      Method Name : readLastBlock
**      Description: The method reads the last block of page relative to the curPagePos of the file. 
**      Input Parameters : An existing file handle and a Page Handle
**      Return Value : RC_OK | RC_READ_NON_EXISTING_PAGE
**
******************************************************************************************************************
*/
RC readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {
	off_t offset = fHandle->totalNumPages * PAGE_SIZE;
	if (pread(fHandle->mgmtInfo, memPage, PAGE_SIZE, offset) < 0) {
		return RC_READ_NON_EXISTING_PAGE;
	}	
	return RC_OK;
}
/*
******************************************************************************************************************
**
**      Method Name : writeBlock
**      Description: The method Writes a page to disk. 
**      Input Parameters : An Integer "pageNum", An existing file handle and a Page handle
**      Return Value : RC_OK | RC_READ_NON_EXISTING_PAGE | RC_WRITE_FAILED
**
******************************************************************************************************************
*/
RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage) {
	if (pageNum > fHandle->totalNumPages) {
		return RC_READ_NON_EXISTING_PAGE;
	}	

	off_t offset = pageNum * PAGE_SIZE;

	if (pwrite(fHandle->mgmtInfo, memPage, PAGE_SIZE, offset) > 0 ) {
		return RC_OK;
	}
	return RC_WRITE_FAILED;
	
}
/*
******************************************************************************************************************
**
**      Method Name : writeCurrentBlock
**      Description: The method Write a page to disk.  
**      Input Parameters :  An existing file handle and a Page handle
**      Return Value : RC_OK | RC_WRITE_FAILED
**
******************************************************************************************************************
*/
RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {
	off_t offset = fHandle->curPagePos * PAGE_SIZE;
	if (pwrite(fHandle->mgmtInfo, memPage, PAGE_SIZE, offset) < 0) {
		return RC_WRITE_FAILED;
	}	
	return RC_OK;
}



/*
******************************************************************************************************************
**
**      Method Name : appendEmptyBlock
**      Description: Increase the number of pages in the file by one. The new last page is filled with zero bytes.  
**      Input Parameters :  An existing file handle
**      Return Value : RC_OK | RC_WRITE_FAILED
**
******************************************************************************************************************
*/
RC appendEmptyBlock (SM_FileHandle *fHandle) {
	
	char data[PAGE_SIZE];
	memset(data,'\0',sizeof(data));
	
	int fd = fHandle->mgmtInfo;

	if(write(fd, memPage, PAGE_SIZE) < 0) {
		close(fd);
		return RC_WRITE_FAILED;
	}
	else {
		close(fd);
		return RC_OK;
	}
	
}
/*
******************************************************************************************************************
**
**      Method Name : ensureCapacity
**      Description: If the file has less than numberOfPages pages then the method increases the size to numberOfPages.  
**      Input Parameters :  An Integer "totalNumPages" and An existing file handle
**      Return Value : RC_OK
**
******************************************************************************************************************
*/
RC ensureCapacity (int numberOfPages, SM_FileHandle *fHandle) {
	if (fHandle->totalNumPages < numberOfPages) {
		int diff = numberOfPages - fHandle->totalNumPages;
		int i;
		for (i = 0; i < diff; ++i) {
			appendEmptyBlock(fHandle);	
		}
		return RC_OK;
	}
	return RC_OK;
}
