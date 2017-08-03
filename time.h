#include <stdio.h>
#include <time.h>
#include <errno.h>

typedef struct  {
        int tm_sec;                         // секунды 0..59 
        int tm_min;                         // минуты  0..59 
        int tm_hour;                        // час дня 0..23 
        int tm_mday;                    // день месяца 1..31 
        int tm_mon;                           // месяц 0..11 
        int tm_year;                       // год после 1900 
        int tm_wday;         // день недели, 0..6 (Sun..Sat) 
        int tm_yday;                    // день года, 0..365 
        int tm_isdst;        // >0, если есть поправка на сдвиг,
                          // =0, если нет поправки,
                          // <0, если неизвестно 
      } tm ;
int Time(tm *time1);

