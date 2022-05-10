// אופיר שטרוסברג, 207828641
// לידור דנון, 211823240
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
int main(int argc, char *argv[])
{
    if(argc < 2){
        printf("error, not enough arguments");
        exit(-1);
    }
    int dirListfd, readBytes;
    int compResult;
    int stat;
    pid_t pid;
    char charDirList;
    int countName = 0;
    int i = 0;
    char *configLine1 = NULL;
    char *configLine2 = NULL;
    char *configLine3 = NULL;
    size_t  len1 = 0;
    size_t  len2 = 0;
    size_t  len3 = 0;
    int screen = dup(1);
    int resultfd = open("result.csv", O_CREAT | O_RDWR | O_TRUNC, 0700);
    if (resultfd == -1)
    {
        printf("error, in open result.csv\n");
        exit(-1);
    }
    close(resultfd);
    FILE *configStream = fopen(argv[1], "r");
    if (configStream == NULL)
    {
        printf("error, in open config file\n");
        exit(-1);
    }
    if (getline(&configLine1, &len1, configStream) <= 0)
    {
        printf("error, couldn't read first line in config file\n");
        free(configLine1);
        fclose(configStream);
        exit(-1);
    }
    if (getline(&configLine2, &len2, configStream) <= 0)
    {
        printf("error, couldn't read second line in config file\n");
        free(configLine1);
        free(configLine2);
        fclose(configStream);
        exit(-1);
    }
    if (getline(&configLine3, &len3, configStream) <= 0)
    {
        printf("error, couldn't read third line in config file\n");
        free(configLine1);
        free(configLine2);
        free(configLine3);
        fclose(configStream);
        exit(-1);
    }
    // remove "\n"
    configLine1[strcspn(configLine1, "\n")] = 0;
    configLine2[strcspn(configLine2, "\n")] = 0;
    configLine3[strcspn(configLine3, "\n")] = 0;
    fclose(configStream);
    // create output.txt
    int outputfd = open("output.txt", O_CREAT | O_RDWR | O_TRUNC, 0700);
    if (outputfd == -1)
    {
        printf("error, in open output.txt\n");
        free(configLine1);
        free(configLine2);
        free(configLine3);
        exit(-1);
    }
    close(outputfd);
    if ((pid = fork()) < 0)
    {
        printf("error in fork\n");
        free(configLine1);
        free(configLine2);
        free(configLine3);
        remove("output.txt");
        exit(-1);
    }
    else
    {
        if (pid == 0)
        {
            // child
            dirListfd = open("dirList.txt", O_CREAT | O_RDWR | O_TRUNC, 0700);
            if (dirListfd == -1)
            {
                printf("error, couldn't open dirList.txt\n");
                free(configLine1);
                free(configLine2);
                free(configLine3);
                remove("output.txt");
                exit(-1);
            }
            close(1);
            dup(dirListfd);
            close(dirListfd);
            char *arguments[] = {"ls", configLine1, NULL};
            if (execvp("ls", arguments) == -1)
            {
                close(1);
                dup(screen);
                printf("error, couldn't run ls\n");
                free(configLine1);
                free(configLine2);
                free(configLine3);
                remove("dirList.txt");
                remove("output.txt");
                exit(-1);
            }
            close(1);
            dup(screen);
        }
        else
        {
            wait(&stat);
            // read the dirlist file
            FILE *dirListStream = fopen("dirList.txt", "r");
            if (dirListStream == NULL)
            {
                printf("error, couldn't open dirList.txt\n");
                free(configLine1);
                free(configLine2);
                free(configLine3);
                remove("dirList.txt");
                remove("output.txt");
                exit(-1);
            }
            while (1)
            {
                char *name = NULL;
                size_t nameLen = 0;
                if (getline(&name, &nameLen, dirListStream) <= 0)
                {
                    free(name);
                    break;
                }
                else
                {
                    name[strcspn(name, "\n")] = 0;
                    if((pid=fork())< 0){
                        printf("error in fork\n");
                        free(name);
                        free(configLine1);
                        free(configLine2);
                        free(configLine3);
                        fclose(dirListStream);
                        remove("dirList.txt");
                        remove("output.txt");
                        exit(-1);
                    }
                    else{
                    if (pid == 0)
                    {
                        // second fork
                        ///////////////////////////////////
                        outputfd = open("output.txt", O_RDWR);
                        if (outputfd == -1)
                        {
                            printf("error, couldn't open output.txt\n");
                            free(name);
                            free(configLine1);
                            free(configLine2);
                            free(configLine3);
                            fclose(dirListStream);
                            remove("dirList.txt");
                            remove("output.txt");
                            exit(-1);
                        }
                        close(1);
                        dup(outputfd);
                        close(outputfd);
                        int inputfd = open(configLine2, O_RDWR);
                        close(0);
                        dup(inputfd);
                        close(inputfd);
                        char * path = strcat(strcat(strcat(configLine1, "/"),name), "/main.exe");
                        char *arguments2[] = {path, configLine2, NULL};
                        if(execvp(arguments2[0], arguments2) == -1) {
                            close(1);
                            dup(screen);
                            printf("error, couldn't run main.exe\n");
                            free(name);
                            free(configLine1);
                            free(configLine2);
                            free(configLine3);
                            fclose(dirListStream);
                            remove("dirList.txt");
                            remove("output.txt");
                            exit(-1);
                        }
                        close(1);
                        dup(screen);

                    }
                    else
                    {
                        wait(&stat);
                        // third fork
                        if((pid=fork())< 0){
                            printf("error in fork\n");
                            free(name);
                            free(configLine1);
                            free(configLine2);
                            free(configLine3);
                            fclose(dirListStream);
                            remove("dirList.txt");
                            remove("output.txt");
                            exit(-1);
                        }
                        else{
                            if (pid == 0)
                            {
                                char *arguments3[] = {"./comp.out", "output.txt", configLine3, NULL};
                                if(execvp(arguments3[0], arguments3)==-1){
                                    printf("error, couldn't run comp.out\n");
                                    free(name);
                                    free(configLine1);
                                    free(configLine2);
                                    free(configLine3);
                                    fclose(dirListStream);
                                    remove("dirList.txt");
                                    remove("output.txt");
                                    exit(-1);
                                }
                            }
                            else
                            {
                                wait(&stat);
                                //close(1);
                                //dup(screen);
                                compResult = WEXITSTATUS(stat);
                                //printf("%d", compResult);
                                resultfd = open("result.csv", O_RDWR | O_APPEND);
                                                if (resultfd == -1)
                                {
                                    printf("error, couldn't open result.csv\n");
                                    free(name);
                                    free(configLine1);
                                    free(configLine2);
                                    free(configLine3);
                                    fclose(dirListStream);
                                    remove("dirList.txt");
                                    remove("output.txt");
                                    exit(-1);
                                }
                                                close(1);
                                dup(resultfd);
                                close(resultfd);
                                if (compResult == 2)
                                {
                                    printf("%s,100\n",name);
                                }
                                else
                                {
                                    printf("%s,0\n",name);
                                }
                                free(name);
                                close(resultfd);
                            }
                       }
                    }
                 }
             }
          }
            //
            fclose(dirListStream);
        }
    }
    remove("dirList.txt");
    remove("output.txt");
    free(configLine1);
    free(configLine2);
    free(configLine3);
}
