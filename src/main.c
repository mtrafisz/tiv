#ifdef _WIN32
#error "No software for you, windows user :3"
#endif

#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <libgen.h>
#include <assert.h>

#include <logger.h>
#include <car.h>
#include <sha256.h>

#define PATH_MAX_LEN 256
#define FILE_MAX_SIZE_BYTES 100 * 1024 * 1024

void usage(int argc, char* argv[]) {
    printf("usage: %s <path>", argv[0]);
}

// TODO: allocate path on heap - for now its like this to avoid annoying memory management.
typedef struct {
    char full_path[PATH_MAX_LEN];
    BYTE hash[SHA256_BLOCK_SIZE];
} FileHash;

Array(FileHash) collect_file_hashes(const char* path) {
    assert(path != NULL);
    if (strlen(path) > PATH_MAX_LEN) {
        log_message(LOG_FATAL, "Path %s is too long", path);
        exit(1);
    }

    SHA256_CTX sha_ctx;
    sha256_init(&sha_ctx);

    struct stat path_stat;
    if (lstat(path, &path_stat) != 0) {
        log_errno("Couldn't read %s stats: %s", path);
        exit(1);
    }

    Array file_hashes = array(FileHash, STD_ALLOCATOR);

    if (S_ISREG(path_stat.st_mode)) {
        FILE* ffd = fopen(path, "r");
        if (ffd == NULL) {
            log_errno("Couldn't open %s", path);
            exit(1);
        }

        fseek(ffd, 0, SEEK_END);
        size_t fsize = ftell(ffd);
        fseek(ffd, 0, SEEK_SET);

        if (fsize > FILE_MAX_SIZE_BYTES) {
            log_message(LOG_FATAL, "File %s is to big", path);
            exit(1);
        }

        char* file_contents = malloc(fsize);
        assert(file_contents);

        if (fread(file_contents, fsize, 1, ffd) == 0) {
            log_errno("Couldn't read from file %s", path);
            exit(1);
        }

        fclose(ffd);

        sha256_update(&sha_ctx, (BYTE*)file_contents, fsize);

        free(file_contents);

        FileHash fh = {0};
        
        memcpy(fh.full_path, path, strlen(path));
        sha256_final(&sha_ctx, fh.hash);

        array_append(&file_hashes, &fh);
        return file_hashes;
    } else if (S_ISDIR(path_stat.st_mode)) {
        DIR* dfd = opendir(path);
        if (dfd == NULL) {
            log_errno("Couldn't open %s: %s", path);
            exit(1);
        }

        errno = 0;
        struct dirent* direntry;
        while ((direntry = readdir(dfd)) != NULL) {
            if (strcmp(direntry->d_name, ".") == 0 || strcmp(direntry->d_name, "..") == 0) continue;
            
            if (strlen(path) + strlen(direntry->d_name) > PATH_MAX_LEN) {
                log_message(LOG_FATAL, "Full child path would be too long");
            }

            char child_full_path[PATH_MAX_LEN] = {0};
            memcpy(child_full_path, path, strlen(path));
            child_full_path[strlen(path)] = '/';
            memcpy(child_full_path + strlen(path) + 1, direntry->d_name, strlen(direntry->d_name));

            Array child_result = collect_file_hashes(child_full_path);
            array_combine(&file_hashes, &child_result);
            array_destroy(&child_result);
        }

        closedir(dfd);

        return file_hashes;
    }

    log_message(LOG_FATAL, "File %s not recognized", path);
    exit(1);
}

int main(int argc, char** argv) {
    logger_init();
    set_log_append_dt(false);

    if (argc != 2) {
        log_message(LOG_FATAL, "Invalid argument list");
        usage(argc, argv);
        return 1;
    }

    char* dir_path = argv[1];

    Array file_hashes = collect_file_hashes(dir_path);

    ArrayIterator it = iterator(&file_hashes);
    FileHash* fh;
    while ((fh = iterator_next(&it))) {
        log_message(LOG_INFO, "hash for file %s:", fh->full_path);
        for (int i = 0; i < SHA256_BLOCK_SIZE; ++i) {
            printf("0x%02X ", fh->hash[i]);
        }
        puts("");
    }

    array_destroy(&file_hashes);

    return 0;
}
