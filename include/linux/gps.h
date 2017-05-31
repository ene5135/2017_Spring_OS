// atleasta0

DEFINE_RWLOCK(gps_lock); // lock for global gps_location
DEFINE_RWLOCK(i_gps_lock); // lock for inode gps_location

struct gps_location {
	int lat_integer;
	int lat_fractional;
	int lng_integer;
	int lng_fractional;
	int accuracy;
};

struct gps_location curr_gps_location = {0,0,0,0,0};

