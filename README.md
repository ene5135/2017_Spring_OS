# Project 04

# Geo-tagged File System

# Introduction
The objective of this project is to upgrade file system, geo-tagged ext2 file system. By tagged gps lcation, we also have to control the file permissions. If the distance between location of kernel and the file, which will be calculated from their gps coordinates, is shorter than sum of their accuracy, than our policy decides to give the user permission to access the file. By upgrading whole file system, we can understand how the kernel file system works and how the file system maintains whole the files in the system flawless.

# Implementation

## 1. `gps_location` definition
We made definition of `gps_location` in `include/linux/gps.h`.
```c
struct gps_location {
  int lat_integer;      /* latitude = lat_integer + lat_fractional * 10^(-6) */
  int lat_fractional;   /* range of latitude : [-90, 90] */
  int lng_integer;      /* longitude = lng_integer + lng_fractional * 10^(-6) */
  int lng_fractional;   /* range of longitude : [-180, 180] */
  int accuracy;         /* range of accuracy : nonnegative */
};
```
`lat` stands for latitude, and `lng` stands for longitude. Due to the kernel characteristic that the floating point variables cannot be used, each values are seperated by integer parts and fractional parts.

## 2. system call implementation

## 2-1. `sys_set_gps_location`
## 2-2. `sys_get_gps_location`

## 3. `ext2_inode_operations` implementation
we need to append the following fields at the end of the ext2 inode structure.

### 3-1. `ext2_set_gps_location`
where to call set_gps_location?
### 3-2. `ext2_get_gps_location`
### 3-3. `ext2_permission`
how the permission is granted. 

# Lessons Learned
