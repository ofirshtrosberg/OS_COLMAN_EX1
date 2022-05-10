// אופיר שטרוסברג, 207828641
// לידור דנון, 211823240
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
int main(int argc, char* argv[]){
//incorrect amount of arguments
if(argc != 3)
        return 1;
//open the given files for read only
int openfile1 = open(argv[1], O_RDONLY);
int openfile2 = open(argv[2], O_RDONLY);
//check if the files open worked
if(openfile1 == -1)
        return 1;
if(openfile2 == -1)
        return 1;
//if the files opened correctly compare them
char charfile1;
char charfile2;
int readBytes1;
int readBytes2;
do{
readBytes1 = read(openfile1,&charfile1,1);
readBytes2 = read(openfile2,&charfile2,1);
if((charfile1!=charfile2)){
close(openfile1);
close(openfile2);
return 1;
}
}while(readBytes1==1 && readBytes2==1);
if((readBytes1==1 && readBytes2==0)||(readBytes1==0 && readBytes2==1)){
close(openfile1);
close(openfile2);
return 1;
}
return 2;
}
