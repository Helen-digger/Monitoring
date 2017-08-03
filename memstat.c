#include "memstat.h"

int Mem_Info(Memlist *mem_stat) 
{
	printf("%s %s\n", __func__, errno ? strerror(errno) : "ok");
	char buffer1[128];
	FILE * file = fopen(PROCMEMINFOFILE, "r");
	if (file == NULL)
	{
		perror("Cannot open " PROCMEMINFOFILE);
		return -1;
	}

	for (int i=0; i<N; i++)
	{
		while (fgets(buffer1, 128, file)) 
		{
			sscanf(buffer1, "MemTotal: %32lu kB",     &mem_stat->MemTotal);
			sscanf(buffer1, "MemFree: %32lu kB",      &mem_stat->MemFree);
			sscanf(buffer1, "MemAvailable: %32lu kB", &mem_stat->MemAvailable);
			sscanf(buffer1, "Buffers: %32lu kB",      &mem_stat->Buffers);
			sscanf(buffer1, "Cached: %32lu kB",       &mem_stat->Cached);
			sscanf(buffer1, "SwapCached: %32lu kB",   &mem_stat->SwapCached);
			sscanf(buffer1, "Active: %32lu kB",       &mem_stat->Active);
			sscanf(buffer1, "Inactive: %32lu kB",     &mem_stat->Inactive);
			break;
		}
	}

	fclose(file);

	return 0;
}
