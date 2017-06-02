// #include <linux/kernel.h>
// #include <linux/linkage.h>
// #include <linux/sched.h>

#include <asm-generic/uaccess.h>
#include <linux/fs.h>
#include <linux/namei.h>
#include <linux/slab.h>

DEFINE_RWLOCK(gps_lock); // lock for global gps_location
DEFINE_RWLOCK(i_gps_lock); // lock for inode gps_location

struct gps_location curr_gps_location = {0,0,0,0,0};


asmlinkage long sys_set_gps_location(struct gps_location __user *loc) 
{
	struct gps_location *tmp_loc = kmalloc(sizeof(struct gps_location), GFP_KERNEL);

//	printk(KERN_ERR "tmp_loc kmalloced\n");
	
	if (copy_from_user(tmp_loc, loc, sizeof(*tmp_loc)) < 0) {
		kfree(tmp_loc);
		return -EACCES;
	}
	
	if (tmp_loc == NULL) {
		kfree(tmp_loc);
		return -EINVAL;
	}

	write_lock(&gps_lock);
//	printk(KERN_ERR "write_lock held");
	
	memcpy(&curr_gps_location, tmp_loc, sizeof(*tmp_loc));
//	printk(KERN_ERR "memcpy");

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
	char *pname;
	int len;

	int debug=0;

	len = strlen_user(pathname);

	//pname = kzalloc(sizeof(char __user) * (len+1), GFP_KERNEL);
	//copy_from_user(pname, pathname, sizeof(char __user) * (len+1));
	//strncpy_from_user(pname,pathname,len);
	//debug = user_path(pathname, &fp);

///////////////////////////////////////

	// atleasta0
	// user_path() must receive (char __user *) type path name.	otherwise it causes error.(-EFAULT)
	// plus, in user_path(), it calls strncpy_from_user(),
	// that means we don't need to use copy_from_user() function and so on.

//////////////////////////////////////

	if (user_path(pathname, &fp) < 0) {
//	if (debug < 0) {
		kfree(tmp_loc);
//		kfree(pname);
		printk(KERN_DEBUG "errno = %d\n",debug);
		return -EACCES;
	}

	
	inode = fp.dentry->d_inode;
	
	if (inode->i_op->get_gps_location(inode, tmp_loc) < 0) {
		kfree(tmp_loc);
//		kfree(pname);
//		printk(KERN_DEBUG "get_gps_location() part\n");
		return -ENODEV;
	}
	
	copy_to_user(loc, tmp_loc, sizeof(*tmp_loc));

	kfree(tmp_loc);
//	kfree(pname);

	return 0;
}
