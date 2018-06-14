#include "memstat.h"

int Mem_Info(Memlist *mem_stat) 
{
	/*printf("%s %s\n", __func__, (errno ? strerror(errno) : "ok"));*/
	char buffer[BUF_SIZE];
	memset(buffer, 0, sizeof(buffer));

	FILE * file = fopen(PROCMEMINFOFILE, "r");
	if (file == NULL)
	{
		perror("Cannot open " PROCMEMINFOFILE);
		return -1;
	}

	for (int i=0; i<N; i++)
	{
		while (fgets(buffer, BUF_SIZE, file)) 
		{
			sscanf(buffer, "MemTotal: %32lu kB",     &mem_stat->MemTotal);
			sscanf(buffer, "MemFree: %32lu kB",      &mem_stat->MemFree);
			sscanf(buffer, "MemAvailable: %32lu kB", &mem_stat->MemAvailable);
			sscanf(buffer, "Buffers: %32lu kB",      &mem_stat->Buffers);
			sscanf(buffer, "Cached: %32lu kB",       &mem_stat->Cached);
			sscanf(buffer, "SwapCached: %32lu kB",   &mem_stat->SwapCached);
			sscanf(buffer, "Active: %32lu kB",       &mem_stat->Active);
			sscanf(buffer, "Inactive: %32lu kB",     &mem_stat->Inactive);
			break;
		}
	}

	if (NULL != file)
	{
		fclose(file);
		file = NULL;
	}

	return 0;
}
