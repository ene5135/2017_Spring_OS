// #include <linux/kernel.h>
// #include <linux/linkage.h>
// #include <linux/sched.h>

#include <linux/gps.h>
#include <asm-generic/uaccess.h>
#include <linux/fs.h>

DEFINE_RWLOCK(gps_lock);

asmlinkage long sys_set_gps_location(struct gps_location __user *loc) 
{
	struct gps_location *tmp_loc;
	
	if (copy_from_user(tmp_loc, loc, sizeof(*tmp_loc)) < 0)
		return -EACCES;
	
	if (tmp_loc == NULL)
		return -EINVAL;

	write_lock(&gps_lock);
	
	memcpy(&curr_gps_loc, tmp_loc, sizeof(*tmp_loc));

	write_unlock(&gps_lock);

	return 0;
}

asmlinkage long sys_get_gps_location(const char __user *pathname, 
		struct gps_location __user *loc)
{
	struct path fp;
	struct inode *inode;
	struct gps_location *tmp_loc;
	const char *pname;
	int len;

	len = strlen_user(pathname);
	copy_from_user(pname, pathname, len);

	if (user_path_at(AT_FDCWD, pathname, LOOKUP_FOLLOW, &fp) < 0)
		return -EACCES;
	
	inode = fp.dentry->d_inode;
	
	if (get_gps_location(inode, tmp_loc) < 0)
		return -ENODEV;
	
	copy_to_user(loc, tmp_loc, sizeof(*tmp_loc));

	return 0;
}
