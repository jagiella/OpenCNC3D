/* Before SDL_Init loads libGL, ask for the non-boot GPU with DRI_PRIME=1.
   An exported DRI_PRIME wins, including DRI_PRIME=0. One GPU is left alone. */
#ifndef OPENCNC3D_GPU_PRIME_H
#define OPENCNC3D_GPU_PRIME_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__linux__)
#include <dirent.h>

static int opencnc3d_has_discrete_gpu(void)
{
    DIR* d;
    struct dirent* ent;

    d = opendir("/sys/class/drm");
    if (!d) return 0;
    while ((ent = readdir(d)) != NULL) {
        char path[288];
        FILE* f;
        int boot = 1;
        if (strncmp(ent->d_name, "card", 4) != 0) continue;
        if (strchr(ent->d_name, '-')) continue;
        snprintf(path, sizeof path, "/sys/class/drm/%s/device/boot_vga", ent->d_name);
        f = fopen(path, "r");
        if (!f) continue;
        if (fscanf(f, "%d", &boot) != 1) boot = 1;
        fclose(f);
        if (boot == 0) {
            closedir(d);
            return 1;
        }
    }
    closedir(d);
    return 0;
}
#endif

static void opencnc3d_prefer_discrete_gpu(void)
{
#if defined(__linux__)
    if (getenv("DRI_PRIME")) return;
    if (!opencnc3d_has_discrete_gpu()) return;
    setenv("DRI_PRIME", "1", 0);
    fprintf(stderr, "gpu: DRI_PRIME=1\n");
#endif
}

#endif
