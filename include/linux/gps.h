DEFINE_SPINLOCK(gps_lock);

struct gps_location {
	int lat_integer;
	int lat_fractional;
	int lng_integer;
	int lng_fractional;
	int accuracy;
};

struct gps_location curr_gps_location = {0,0,0,0,0};

