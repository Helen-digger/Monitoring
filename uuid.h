#include <stdio.h>
#include <string.h>
#include <errno.h>

#define UUID_FILE_PATH "/sys/class/dmi/id/product_uuid"
#define ID_LEN  64

int Uuid(char *uu);
