#include <stdio.h>
/**
 * 现代PC大多采用小端字节序，因此小端字节序又被称为主机字节序。
 * 
 */ 
void byteorder()
{
    // union 中的成员共用内存
    // short占两个字节
    // char 数组也占两个字节
    union 
    {
        short value;
        char union_bytes[ sizeof( short )];
    }test;
    test.value = 0x0102;
    if((test.union_bytes[0] == 1) && (test.union_bytes[1] == 2))
    {
        printf("big endian\n");
    }
    else if((test.union_bytes[0] == 2) && (test.union_bytes[1] == 1)){
        printf("little endian\n");
    }
    else{
        printf("unkonwn...\n");
    }
    
}

int main(int argc, const char** argv) {
    byteorder();
    return 0;
}