#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/capability.h>
#define filename "main"

int main()
{
    cap_t cp1, cp2;
    cp1 = cap_get_file((const char*)filename);
    char *st = cap_to_text(cp1, NULL);
    printf("file capabilities:\n%s\n", st);

    cp2 = cap_from_text("cap_fowner=+eip");
    int rv = cap_set_file(filename, cp2);
    if(rv == -1)
        perror("cap_set_file");
    else printf("\ncap_set_file 'file' success\n");

    cp1 = cap_get_file((const char*)filename);
    st = cap_to_text(cp1, NULL);
    printf("\nfile capabilities:\n%s\n", st);

    return 0;
}
