# Project 04

# Geo-tagged File System

# Introduction

# Implementation

## 1. `gps_location` definition

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
