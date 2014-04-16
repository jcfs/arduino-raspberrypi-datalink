#include <stdio.h>
#include <sys/select.h>

int main(void) {
    fd_set s_rd, s_wr, s_ex;
    FD_ZERO(&s_rd);
    FD_ZERO(&s_wr);
    FD_ZERO(&s_ex);
    FD_SET(fileno(stdin), &s_rd);
    select(fileno(stdin)+1, &s_rd, &s_wr, &s_ex, NULL);
    return 0;
}
