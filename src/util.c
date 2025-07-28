// util.c
#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define MEMORY_LIMIT_MB 256
#define CGROUP_PATH "/sys/fs/cgroup/memory/cgrp1"
#define BUFLEN 110

double getCPUFreq(void)
{
    FILE *sysinfo;
    char buf[BUFLEN];
    const char key[] = "cpu MHz";
    const size_t keylen = sizeof(key) - 1;
    double freq = -1.0;

    sysinfo = fopen("/proc/cpuinfo", "r");
    if (sysinfo) {
        while (fgets(buf, BUFLEN, sysinfo)) {
            if (!strncmp(buf, key, keylen)) {
                char *ptr = strchr(buf, ':');
                if (ptr) freq = atof(ptr + 1) * 1e6;
                break;
            }
        }
        fclose(sysinfo);
    }
    // 可注释掉
    fprintf(stderr, "Freq = %f GHz\n", freq / 1e9);
    return freq;
}

int getCPUCount(void)
{
    return sysconf(_SC_NPROCESSORS_CONF);
}

static int safe_write_str(int fd, const char *s)
{
    size_t len = strlen(s);
    return (write(fd, s, len) == (ssize_t)len) ? 0 : -1;
}

void Initialize(void)
{
    struct stat st = {0};
    char mlim_path[]  = CGROUP_PATH "/memory.limit_in_bytes";
    char tasks_path[] = CGROUP_PATH "/tasks";
    char mlim_str[64], pid_str[64];

    // 若没有权限或不在 cgroup v1 环境，可直接返回
    if (stat("/sys/fs/cgroup", &st) == -1) {
        perror("cgroup fs not available");
        return;
    }

    // 尝试删除已有目录（忽略错误）
    if (stat(CGROUP_PATH, &st) == 0) {
        if (rmdir(CGROUP_PATH) == -1 && errno != ENOENT) {
            // 无法删除说明可能已有内容，忽略即可
        }
    }

    if (mkdir(CGROUP_PATH, S_IRUSR | S_IWUSR | S_IXUSR) == -1) {
        // 如果失败，打印警告然后返回
        perror("mkdir cgroup");
        return;
    }

    int mlim_fd = open(mlim_path, O_WRONLY);
    if (mlim_fd == -1) {
        perror("open memory.limit_in_bytes");
        return;
    }

    snprintf(mlim_str, sizeof(mlim_str), "%dM", MEMORY_LIMIT_MB);
    if (safe_write_str(mlim_fd, mlim_str) == -1) {
        perror("write memory limit");
        close(mlim_fd);
        return;
    }
    close(mlim_fd);

    int tasks_fd = open(tasks_path, O_WRONLY);
    if (tasks_fd == -1) {
        perror("open tasks file");
        return;
    }

    snprintf(pid_str, sizeof(pid_str), "%d", getpid());
    if (safe_write_str(tasks_fd, pid_str) == -1) {
        perror("write tasks");
        close(tasks_fd);
        return;
    }
    close(tasks_fd);
}
