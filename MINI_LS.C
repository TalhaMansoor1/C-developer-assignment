#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>

void walkFolders(const char *dirPath, int indent) {
    DIR *d;
    struct dirent *thing;

    d = opendir(dirPath);
    if (d == NULL) {
        // if the folder does not exist
        perror("opendir failed");
        return;
    }

    while ((thing = readdir(d)) != NULL) {
        if (strcmp(thing->d_name, ".") == 0 || strcmp(thing->d_name, "..") == 0) {
            continue;
        }

        // giving spaces
        for (int k = 0; k < indent; k++) {
            printf("  ");   // two spaces each level
        }

        printf("%s\n", thing->d_name);

        // Building a path for it
        char full[1024];
        snprintf(full, sizeof(full), "%s/%s", dirPath, thing->d_name);

        struct stat info;
        int stat_ok = stat(full, &info);
        if (stat_ok == 0) {
            if (S_ISDIR(info.st_mode)) {
            
                walkFolders(full, indent + 1);
            }
        } else {
            // meh, stat failed — maybe a broken symlink?
            // printf("Couldn't stat %s\n", full);   
        }
    }

    closedir(d);
}

int main(int argc, char *argv[]) {
    const char *rootDir = ".";   // for current directory
    if (argc > 1) {
        rootDir = argv[1];
    }

    // later: maybe add options like max depth or printing file sizes
    walkFolders(rootDir, 0);

    return 0;
}
