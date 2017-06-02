// #include <linux/kernel.h>
// #include <linux/linkage.h>
// #include <linux/sched.h>

#include <asm-generic/uaccess.h>
#include <linux/fs.h>
#include <linux/namei.h>
#include <linux/slab.h>

struct gps_location curr_gps_location = {0,0,0,0,0};

DEFINE_RWLOCK(gps_lock); // lock for global gps_location
DEFINE_RWLOCK(i_gps_lock); // lock for inode gps_location

asmlinkage long sys_set_gps_location(struct gps_location __user *loc) 
{
	struct gps_location *tmp_loc = kmalloc(sizeof(struct gps_location), GFP_KERNEL);

	if (copy_from_user(tmp_loc, loc, sizeof(*tmp_loc)) < 0) {
		kfree(tmp_loc);
		return -EACCES;
	}
	
	if (tmp_loc == NULL) {
		kfree(tmp_loc);
		return -EINVAL;
	}
	if (tmp_loc->lat_fractional > 999999 || tmp_loc->lat_fractional < 0 ||
		tmp_loc->lng_fractional > 999999 || tmp_loc->lng_fractional < 0 ||
		tmp_loc->lat_integer > 90 || tmp_loc->lat_integer < -90 ||
		tmp_loc->lng_integer > 180 || tmp_loc->lng_integer < -180 ||
		((tmp_loc->lat_integer == 90 || tmp_loc->lat_integer == -90) 
		 		&& tmp_loc->lat_fractional != 0) ||
		((tmp_loc->lng_integer == 180 || tmp_loc->lng_integer == -180) 
		 		&& tmp_loc->lng_fractional != 0) ||
		tmp_loc->accuracy < 0) {
		kfree(tmp_loc);
		return -EINVAL;
	}

	write_lock(&gps_lock);
	
	memcpy(&curr_gps_location, tmp_loc, sizeof(*tmp_loc));

	write_unlock(&gps_lock);

	kfree(tmp_loc);
	return 0;
}

asmlinkage long sys_get_gps_location(const char __user *pathname, 
		struct gps_location __user *loc)
{
	struct path fp;
	struct inode *inode;
	struct gps_location *tmp_loc = kmalloc(sizeof(struct gps_location), GFP_KERNEL);
	int err=0;

	// atleasta0
	// user_path() must receive (char __user *) type path name.	
	// otherwise it causes error.(-EFAULT)
	// plus, in user_path(), it calls strncpy_from_user(),
	// that means we don't need to use copy_from_user() function and so on.

	if (user_path(pathname, &fp) < 0) {
		kfree(tmp_loc);
		printk(KERN_ALERT "ERROR : USER_PATH");
		return -EACCES;
	}

	inode = fp.dentry->d_inode;

	if (!inode->i_op->get_gps_location) // if it is not ext2fs
	{
		kfree(tmp_loc);
		printk(KERN_ALERT "ERROR : NOT EXT2"); 
		return -ENODEV;
	}

	else // check permission first, and then get return value
	{
		err = inode_permission(inode,MAY_READ);
		if(err < 0)
		{
			kfree(tmp_loc);
			printk(KERN_ALERT "ERROR : PERMISSION DENIED");
			return err;
		}
		inode->i_op->get_gps_location(inode, tmp_loc);
	}

	copy_to_user(loc, tmp_loc, sizeof(*tmp_loc));
	kfree(tmp_loc);

	return 0;
}
