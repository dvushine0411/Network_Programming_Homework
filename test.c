// Bài tập tách dữ liệu địa chỉ IP và cổng //

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main() {
    char resp[] = "227 Entering Passive Mode (213,229,112,130,216,4)";
    char *p = strtok(resp, "(),");
    p = strtok(NULL, "(),");
    int i1 = atoi(p);
    p = strtok(NULL, "(),");
    int i2 = atoi(p);
    p = strtok(NULL, "(),");
    int i3 = atoi(p);
    p = strtok(NULL, "(),");
    int i4 = atoi(p);
    p = strtok(NULL, "(),");
    int p1 = atoi(p);
    p = strtok(NULL, "(),");
    int p2 = atoi(p);
    printf("Địa chỉ IP server: %d.%d.%d.%d\n", i1,i2,i3,i4);
    printf("Cổng kết nối Port: %d\n", (p1 * 256) + p2);




}