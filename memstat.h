#include <stdio.h>
#include <sys/times.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <errno.h>

#ifndef PROCDIR
#define PROCDIR "/proc"
#endif
#ifndef PROCMEMINFOFILE
#define PROCMEMINFOFILE PROCDIR "/meminfo"
#endif
#define N 8

/*example
MemTotal:       16354576 kB
MemFree:         1943976 kB
MemAvailable:   12930988 kB
Buffers:          128612 kB
Cached:         11308336 kB
SwapCached:            0 kB
Active:          9287204 kB
Inactive:        4674756 kB
Active(anon):    2284452 kB
Inactive(anon):   582256 kB
Active(file):    7002752 kB
Inactive(file):  4092500 kB
Unevictable:        8544 kB
Mlocked:            8544 kB
SwapTotal:      16698364 kB
SwapFree:       16698364 kB
Dirty:               216 kB
Writeback:             0 kB
AnonPages:       2325276 kB
Mapped:           474276 kB
Shmem:            334248 kB
Slab:             282756 kB
SReclaimable:     232020 kB
SUnreclaim:        50736 kB
KernelStack:       13776 kB
PageTables:        53760 kB
NFS_Unstable:          0 kB
Bounce:                0 kB
WritebackTmp:          0 kB
CommitLimit:    24875652 kB
Committed_AS:    9278920 kB
VmallocTotal:   34359738367 kB
VmallocUsed:           0 kB
VmallocChunk:          0 kB
HardwareCorrupted:     0 kB
AnonHugePages:    483328 kB
ShmemHugePages:        0 kB
ShmemPmdMapped:        0 kB
CmaTotal:              0 kB
CmaFree:               0 kB
HugePages_Total:       0
HugePages_Free:        0
HugePages_Rsvd:        0
HugePages_Surp:        0
Hugepagesize:       2048 kB
DirectMap4k:      299776 kB
DirectMap2M:    11157504 kB
DirectMap1G:     6291456 kB */

struct MemList_scan
{
   	unsigned long int MemTotal ;
  	unsigned long int MemFree;
  	unsigned long int MemAvailable;
  	unsigned long int Buffers;
  	unsigned long int Cached;
  	unsigned long int SwapCached;
  	unsigned long int Active;
  	unsigned long int Inactive;
 /* unsigned  long int Active_anon;
  unsigned  long int Inactive_anon;
  unsigned  long int Active_file;
  unsigned  long int Inactive_file;
  unsigned  long int Unevictable;
  unsigned  long int Mlocked;
  unsigned  long int SwapTotal;
  unsigned  long int SwapFree;
  unsigned  long int Dirty;
  unsigned  long int Writeback;
  unsigned  long int AnonPages;
  unsigned  long int Mapped;
  unsigned  long int Shmem;
  unsigned  long int Slab;
  unsigned  long int SReclaimable;
  unsigned  long int SUnreclaim;
  unsigned  long int KernelStack;
  unsigned  long int PageTables;
  unsigned  long int NFS_Unstable;
  unsigned  long int Bounce;
  unsigned  long int WritebackTmp;
  unsigned  long int CommitLimit;
  unsigned  long int Committed_AS;
  unsigned  long int VmallocTotal;
  unsigned  long int VmallocUsed;
  unsigned  long int VmallocChunk;
  unsigned  long int HardwareCorrupted;
  unsigned  long int AnonHugePages;
  unsigned  long int ShmemHugePages;
  unsigned  long int ShmemPmdMapped;
  unsigned  long int CmaTotal;
  unsigned  long int CmaFree;
  unsigned  long int HugePages_Total;
  unsigned  long int HugePages_Free;
  unsigned  long int HugePages_Rsvd;
  unsigned  long int HugePages_Surp;
  unsigned  long int Hugepagesize;
  unsigned  long int DirectMap4k ;
  unsigned  long int DirectMap2M;
  unsigned  long int DirectMap1G;*/
} ;

int MemInfo(struct MemList_scan *meminf);
