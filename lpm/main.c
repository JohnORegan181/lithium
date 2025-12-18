#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>

#define LISTFILE "./list"
#define OUTDIR   "./out"
#define MAXLINE  2048

void trim(char *s) {
    char *start = s;
    char *end;

    // Trim leading
    while (*start == ' ' || *start == '\t' || *start == '\n' || *start == '\r')
        start++;

    // Move string to beginning
    if (start != s)
        memmove(s, start, strlen(start) + 1);

    // Trim trailing
    end = s + strlen(s) - 1;
    while (end > s && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r'))
        *end-- = '\0';
}

int run_wget(const char *url, const char *outname) {
    pid_t pid = fork();
    if (pid == 0) {
        execlp("wget", "wget", url, "-O", outname, NULL);
        perror("wget exec failed");
        exit(1);
    } else if (pid < 0) {
        perror("fork failed");
        return 1;
    }

    int status;
    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid failed");
        return 1;
    }

    return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <package>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(LISTFILE, "r");
    if (!f) {
        perror("open list");
        return 1;
    }

    char buf[MAXLINE];
    while (fgets(buf, sizeof(buf), f)) {
        char *pkg = strtok(buf, "|");
        char *command = strtok(NULL, "\n");

        if (!pkg || !command)
            continue;

        trim(pkg);
        trim(command);

        if (strcmp(pkg, argv[1]) == 0) {
            // Extract URL from command (skip "wget " prefix)
            char *url = command;
            if (strncmp(command, "wget ", 5) == 0) {
                url = command + 5;
                trim(url); // Remove extra spaces after "wget "
            }

            // Extract filename from URL
            char *filename = strrchr(url, '/');
            filename = filename ? filename + 1 : url;

            // Create output directory
            if (mkdir(OUTDIR, 0755) != 0 && errno != EEXIST) {
                perror("mkdir");
                fclose(f);
                return 1;
            }

            char outpath[512];
            if (snprintf(outpath, sizeof(outpath), "%s/%s", OUTDIR, filename) >= (int)sizeof(outpath)) {
                fprintf(stderr, "Output path too long\n");
                fclose(f);
                return 1;
            }

            printf("Fetching %s from %s...\n", pkg, url);

            if (run_wget(url, outpath) != 0) {
                fprintf(stderr, "Failed to download %s\n", url);
                fclose(f);
                return 1;
            }

            if (chmod(outpath, 0755) != 0)
                perror("chmod");

            printf("Installed %s\n", outpath);

            fclose(f);
            return 0;
        }
    }

    fprintf(stderr, "Package not found\n");
    fclose(f);
    return 1;
}
