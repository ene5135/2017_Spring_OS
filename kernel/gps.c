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
	if(!access_ok(VERIFY_WRITE, loc, sizeof(struct gps_location)))
	{
		kfree(tmp_loc);
		return -EFAULT;
	}

//	printk(KERN_ERR "tmp_loc kmalloced\n");
//	struct gps_location *tmp_loc = NULL;
	
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
	int err=0;
	
	int len = strlen_user(pathname);

	if(!access_ok(VERIFY_WRITE, pathname, len+1))
	{
		kfree(tmp_loc);
		return -EFAULT;
	}

	if(!access_ok(VERIFY_WRITE, loc, sizeof(struct gps_location)))
	{
		kfree(tmp_loc);
		return -EFAULT;
	}

	/* 
	 atleasta0
	 user_path() must receive (char __user *) type path name.	otherwise it causes error.(-EFAULT)
	 plus, in user_path(), it calls strncpy_from_user(),
	 that means we don't need to use copy_from_user() function and so on.
	*/
	err = user_path(pathname, &fp);
	if (err < 0) {
		if(err == -EACCES)
			printk(KERN_ERR "failed to get gps location : Permission denied\n");
		kfree(tmp_loc);
		return err;
	}

	
	inode = fp.dentry->d_inode;

	if (!inode->i_op->get_gps_location) // if it is not ext2fs
	{
		printk(KERN_ERR "the file system is not ext2fs\n");
		kfree(tmp_loc);
		return -ENODEV;
	}

	else // check permission first, and then get return value
	{
		err = inode_permission(inode,MAY_READ);
		if (err < 0)
		{
			printk(KERN_ERR "failed to get gps location : Permission denied\n");
			kfree(tmp_loc);
			return err;
		}
		inode->i_op->get_gps_location(inode, tmp_loc);
	}

	copy_to_user(loc, tmp_loc, sizeof(*tmp_loc));

	kfree(tmp_loc);

	return 0;
}
