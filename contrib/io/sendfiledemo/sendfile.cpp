#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/sendfile.h>

int main(int argc, const char** argv) {
    const char* ip = argv[1];
    int port = atoi(argv[2]);
    const char* file_name = argv[3];

    int filefd = open(file_name, O_RDONLY);
    assert(filefd > 0);
    struct stat stat_buf;
    fstat(filefd, &stat_buf);


    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    //inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    // 
      int optval = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval,
                 sizeof(optval)) == -1) {
     close(sock);
        return -1;
    }
    int ret = bind(sock, (struct sockaddr*)&address, sizeof(address));
    assert(ret != -1);

    ret = listen(sock, 5);
    assert(ret != -1);

    struct sockaddr_in client;
    socklen_t client_addresslength = sizeof(client);
    int connfd = accept(sock, (struct sockaddr*)&client, &client_addresslength);

    if(connfd < 0){
        printf("errno is: %d\n", errno);
    }
    else{

        sendfile(connfd, filefd, NULL, stat_buf.st_size);
    }
    close(sock);
    
    return 0;
}