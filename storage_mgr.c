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

#define TOTAL_PAGES 1000

// Initialize page file handle and page handle.
SM_FileHandle *fHandle;
SM_PageHandle memPage;

void initStorageManager (void) {

	// allocate space for 2 handles.
	fHandle = (SM_FileHandle *) malloc(sizeof(SM_FileHandle *));
	memPage = (SM_PageHandle) malloc(sizeof(SM_PageHandle)); 

};

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

	// write the singe page to page file.
	if (write(fd, data, PAGE_SIZE) != PAGE_SIZE) {
		printf("Error writing to file %s\n", fileName);
		return RC_WRITE_FAILED;
	};

	// close file descriptor.
	close(fd);
	return RC_OK;

};

RC openPageFile (char *fileName, SM_FileHandle *fHandle) {

	// flag defines file access mode.
	// O_RDWR read and write mode.
	// O_APPEND append mode, used for appending new block to page file.
	int flag = O_RDWR|O_APPEND;
	int fd = open(fileName, flag);

	// fcntl gets or changes the file status, the seconed parameter "F_GETFL"
	// is used for get the flag, if the file is opened correctly, it returns a 
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

	return RC_OK;
}

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

RC destroyPageFile (char *fileName) {
	// destroy file.
	int r = remove(fileName);
	if (r == 0) {
		return RC_OK;
	}
	return RC_FILE_NOT_FOUND;
}

/* reading blocks from disc */
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
int getBlockPos (SM_FileHandle *fHandle) {
	return fHandle->curPagePos;
}

RC readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {

	if (pread(fHandle->mgmtInfo, memPage, PAGE_SIZE, 0) > 0) {
		return RC_OK;
	}
	else {
		return RC_READ_NON_EXISTING_PAGE;
	}
}

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

RC readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {
	off_t offset = fHandle->curPagePos * PAGE_SIZE;
	if (pread(fHandle->mgmtInfo, memPage, PAGE_SIZE, offset) < 0) {
		return RC_READ_NON_EXISTING_PAGE;
	}	
	return RC_OK;
}

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
RC readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {
	off_t offset = fHandle->totalNumPages * PAGE_SIZE;
	if (pread(fHandle->mgmtInfo, memPage, PAGE_SIZE, offset) < 0) {
		return RC_READ_NON_EXISTING_PAGE;
	}	
	return RC_OK;
}

/* writing blocks to a page file */
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

RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {
	off_t offset = fHandle->curPagePos * PAGE_SIZE;
	if (pwrite(fHandle->mgmtInfo, memPage, PAGE_SIZE, offset) < 0) {
		return RC_WRITE_FAILED;
	}	
	return RC_OK;
}

RC appendEmptyBlock (SM_FileHandle *fHandle) {
	
	char data[PAGE_SIZE];
	memset(data,'\0',sizeof(data));
	
	int fd = fHandle->mgmtInfo;

	if(write(fd, memPage, PAGE_SIZE) < 0) {
		return RC_WRITE_FAILED;
	}
	else {
		return RC_OK;
	}
}

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
