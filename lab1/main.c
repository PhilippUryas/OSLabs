#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>
#include <inttypes.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

#define BUFFSIZE 80

void strmode(mode_t mode, char *buf)
{
    const char chars[] = "rwxrwxrwx";
    for (size_t i = 0; i < 9; i++)
    {
        buf[i] = (mode & (1 << (8 - i))) ? chars[i] : '-';
    }
    buf[9] = '\0';
}

void printL(struct dirent *ds)
{
    struct stat fileStat;

    stat(ds->d_name, &fileStat);
    char timebuf[BUFFSIZE];
    strftime(timebuf,
             BUFFSIZE,
             "%c", localtime(&fileStat.st_mtime));
    char protbuf[BUFFSIZE];
    strmode(fileStat.st_mode, protbuf);

    struct passwd *pwd;
    pwd = getpwuid(fileStat.st_uid);

    struct group *fileGroup;
    fileGroup = getgrgid(fileStat.st_gid);

    printf("%s %s %s %s %i %s\n",
           protbuf,
           pwd->pw_name,
           fileGroup->gr_name,
           timebuf,
           fileStat.st_size,
           ds->d_name);
}

int main(int argc, char **argv)
{
    int opt;

    bool flagA = false;
    bool flagL = false;

    while ((opt = getopt(argc, argv, ":al")) != -1)
    {
        switch (opt)
        {
        case 'a':
            flagA = true;
            break;
        case 'l':
            flagL = true;
            break;
        }
    }

    DIR *dptr;
    struct dirent *ds;
    dptr = opendir("./");
    while ((ds = readdir(dptr)) != 0)
    {
        if (flagA)
        {
            if (flagL)
                printL(ds);
            else
                printf("%s\n", ds->d_name);
        }
        else
        {
            if (ds->d_name[0] != '.')
            {
                if (flagL)
                    printL(ds);
                else
                    printf("%s\n", ds->d_name);
            }
        }
    }
    closedir(dptr);
    return 0;
}