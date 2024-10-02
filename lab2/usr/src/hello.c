#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char *argv[]){

    char hello[] = "Hello world !";

    printf("%s\n", hello);

    printf("%p\n", &hello);

    //adress 0xbffcef2c
    printf("%c\n", *(char*)0xbffcef2c);
    

    for(unsigned i=0; i < strlen(hello); ++i){
        ++(*(char*)(0xbffcef2c + i));
        
    }
    

    printf("%s\n", hello);
    printf("%c\n", *(char*)0xbffcef2c);

    return 0;
}