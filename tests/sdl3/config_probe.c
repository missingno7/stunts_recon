#include "port_runtime.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <wchar.h>

int wmain(int argc, wchar_t **argv)
{
    if (argc != 2) return 2;
    char *path = SDL_iconv_string("UTF-8", "UTF-16LE", (const char *)argv[1],
                                 (wcslen(argv[1]) + 1) * sizeof(wchar_t));
    if (!path) return 2;
    int loaded = port_config_load(path);
    SDL_free(path);
    printf("%d %d\n", loaded, port_config_manual_word_check());
    return 0;
}
