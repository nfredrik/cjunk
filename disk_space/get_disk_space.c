#include <sys/statvfs.h>
#include <glib.h>

gchar *
g_get_capacity ( gchar * dev_path)
{
	unsigned long long result = 0;
	int n;
	gchar s_cap[50];
	gchar * ss_cap = "N/A";
	struct statvfs sfs;
	if ( statvfs ( dev_path, &sfs) != -1 )
	{
		result = (unsigned long long)sfs.f_bsize * sfs.f_blocks;
	}
	if (result > 0)
	{
		double f_cap = (double)result/(1024*1024);
		n = sprintf(s_cap, "%.2f Mb", f_cap);
		ss_cap = g_strdup(s_cap);
	}
	return ss_cap;
}

gchar * 
g_get_free_space ( gchar * dev_path)
{
	unsigned long long result = 0;
	int n;
	gchar s_cap[50];
	gchar * ss_cap = "N/A";
	struct statvfs sfs;
	if ( statvfs ( dev_path, &sfs) != -1 )
	{
		result = (unsigned long long)sfs.f_bsize * sfs.f_bfree;
	}
	if (result > 0)
	{
		double f_cap = (double)result/(1024*1024);
		n = sprintf(s_cap, "%.2f Mb", f_cap);
		ss_cap = g_strdup(s_cap);
	}
	return ss_cap;
}

