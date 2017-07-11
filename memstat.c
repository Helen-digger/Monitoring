#include "memstat.h"

int MemInfo(struct MemList_scan *MemList) 
{
    char buffer1[128];
    FILE * file = fopen(PROCMEMINFOFILE, "r");
    if (file == NULL) 
    {
        perror("Cannot open " PROCMEMINFOFILE);
    }
    for (int i=0; i<N; i++)
    {
        while (fgets(buffer1, 128, file)) 
        {
            sscanf(buffer1, "MemTotal: %32lu kB", &MemList->MemTotal);
            sscanf(buffer1, "MemFree: %32lu kB", &MemList->MemFree);
            sscanf(buffer1, "MemAvailable: %32lu kB", &MemList->MemAvailable);
            sscanf(buffer1, "Buffers: %32lu kB", &MemList->Buffers);
            sscanf(buffer1, "Cached: %32lu kB", &MemList->Cached);
            sscanf(buffer1, "SwapCached: %32lu kB", &MemList->SwapCached);
            sscanf(buffer1, "Active: %32lu kB", &MemList->Active);
            sscanf(buffer1, "Inactive: %32lu kB", &MemList->Inactive);
            break;
        }
    }

    /*printf("MemTotal:%32lu kB\n MemFree: %32lu kB\n MemAvailable: %32lu kB\n Buffers: %32lu kB\n Cached: %32lu kB\n SwapCached: %32lu kB\n Active: %32lu kB\n Inactive: %32lu kB\n", 
    MemList->MemTotal, 
    MemList->MemFree, 
    MemList->MemAvailable,
    MemList->Buffers, 
    MemList->Cached,
    MemList->SwapCached, 
    MemList->Active,
    MemList->Inactive);*/

    fclose(file);

    //unsigned  long int RealUsed=(MemList.MemTotal-(MemList.Slab+MemList.Cached+MemList.Buffers+MemList.MemFree)); 
    return (0);
}

 	
