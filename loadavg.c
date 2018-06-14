#include "loadavg.h"

int Load(AVGList * avginfo)
{
	/*printf("%s\n", __func__);*/
	
	FILE * file = fopen(PROCLOADFILE, "r");
	if (file == NULL) 
	{
		perror("Cannot open " PROCLOADFILE);
		return -1;
	}
	int rc = 0;

	if (0 >= (rc = fscanf(file,
	                      "%lf %f %f ",
	                      &avginfo->avg_now,
	                      &avginfo->avg_five,
	                      &avginfo->avg_fifteen)))
	{
		fprintf(stderr, "<%s> read file '%s' failed fscanf rc(%d), errno = %d(%s)", __func__,
		                 PROCLOADFILE, rc, errno, strerror(errno));
		return -1;
	}

	/*printf("cpu %lf %f %f ",
	       avginfo->avg_now,
	       avginfo->avg_five,
	       avginfo->avg_fifteen
	       );*/

	if (NULL != file)
	{
		fclose(file);
		file = NULL;
	}

	return (0);
}
