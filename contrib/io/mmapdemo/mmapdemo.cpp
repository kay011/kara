
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
 
#define TRUE        1
#define FALSE       -1
#define FILE_SIZE   100
 
#define MMAP_FILE_PATH "./mmap.txt"


size_t get_file_size(const char* file_path){
    struct stat buf;
    if(stat(file_path, &buf) < 0){
        printf("%s[%d]:%s", __FUNCTION__, __LINE__,  strerror(errno));
        return -1;
    }
    return buf.st_size;
}

int main(int argc, const char** argv) {
    int fd = -1;
    void *result;
    int lseek_result = -1;
    int file_length = -1;
    // 1. open the file
    fd = open(MMAP_FILE_PATH, O_RDWR | O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP| S_IWGRP | S_IROTH | S_IWOTH);
    if( -1 == fd){
        printf("open failed \n");
        printf("%s \n", strerror(errno));
    }
    // 2. adjust the file size
    lseek_result = lseek(fd, FILE_SIZE-1, SEEK_SET);
    if(-1 == lseek_result){
        printf("lseek failed\n");
        printf("%s\n", strerror(errno));
        return FALSE;
    }

    write(fd, "\0", 1);
    lseek(fd, 0, SEEK_SET);

    // 3. get the filesize
    file_length = get_file_size(MMAP_FILE_PATH);
    if(-1 == file_length){
        printf("get file size failed \n");
        return FALSE;
    }
    printf("filesize = %d\n", file_length);

    // 4. call mmap
    // 函数原型: 
    // void* mmap(void *start, size_t length, int prot, int flags, int fd, offset_t offset);
    result = mmap(0, file_length, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if( result == (void*)-1)
    {
        printf("mmap failed\n");
        printf("%s\n", strerror(errno));
        return FALSE;
    }
    //5. release the file descriptor
    close(fd);
 
    //6. write something to mmap addr,
    strncpy((char* )result, "hello world\n",file_length);

    // 7. call munmap
    munmap(0, file_length);
    return 0;
}