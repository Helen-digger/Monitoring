#include "uuid.h"

int Uuid(char *uu)
{
	printf("%s %s\n", __func__, (errno ? strerror(errno) : "ok"));
	FILE* file = fopen(UUID_FILE_PATH, "r");

	if (file == NULL)
	{
		fprintf(stderr,"'%s': Cannot open '%s'. Error: %s\n", __func__, UUID_FILE_PATH, strerror(errno));
		return -1;
	}

	if (EOF == fscanf(file, "%s", uu ))
	{
		fprintf(stderr,"'%s': Cannot read UUID from '%s'. Error: %s\n", __func__, UUID_FILE_PATH, strerror(errno));
		return -1;
	}

	fclose(file);

	return 0;
}
