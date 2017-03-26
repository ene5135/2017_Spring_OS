
# Project 01

# Wrting system call and adding to kernel

# Introduction
 This assignment includes implementing a new system call, `ptree()` in Linux. It returns the process tree information in depth-first-search order. To check whether the returned tree information is correct, we write a simple C program which calls `ptree()` system call. The program prints the entire process tree in pre-order using tabs to indent children with respect to their parents.

# Implementation

## 1. Writing system call
### 1-1 prinfo
 We used `prinfo` structure to save the process information while traversing the process tree.
There is a struct description below.
    
```c    
struct prinfo {
      long state;             /* current state of process */
      pid_t pid;              /* process id */
      pid_t parent_pid;       /* process id of parent */
      pid_t first_child_pid;  /* pid of oldest child */
      pid_t next_sibling_pid; /* pid of younger sibling */
      long uid;               /* user id of process owner */
      char commm[64];         /* name of program executed */
      };
```
 We declared the description in prinfo.h and included the file in `include/linux` as part of our solution.
  
### 1-2 ptree
 `ptree.c` is the main part of this project. There is whole description of `sys_ptree()` system call.
    
#### 1-2-1 return value
 `sys_ptree()` returns the number of process entry or error number. Also `sys_ptree()` puts process 
entries informations in `buf` by preorder and actual size of `buf` in `nr`.
    
#### 1-2-2 error check
 Before the specific implementation, the system call checks for the input arguments whether 
they are correct or not. Mainly, there are four error cases. So we handle the errors by 
four steps below.
      
   1. When the arguments (`buf`, `nr`) have NULL value : return `-EINVAL`;
   2. When the nr's address is not accessible.     : return `-EFAULT`;
   3. When the number of entries(`*nr`) is less than 1    : return `-EINVAL`;
   4. When the buf's address is not accessible.    : return `-EFAULT`;
      - In second and fourth step, we used `access_ok` macro which is defined in `uaccess.h`
        
        "definition of access_ok macro"
 
#### 1-2-3 read_lock, read_unlock
 We will traverse all the `task_struct`s of whole processes at certain moment. Thus we need to
lock the task list to avoid modification of the process' informations. We locked the tasklist
using `read_lock()` until the traverse finishes. After the traverse is finished, we unlocked it
by `read_unlock()`.
      
```c
read_lock(&tasklist_lock);
 /* do the job... */
read_unlock(&tasklist_lock);
```
      
#### 1-2-4 call recursive function
 We implemented the traversing algorithm using recursive call. We defined seperate recursive
function, `pre_order()`. So in `sys_ptree()`, what we have to do is just call the `pre_order`
once.

```c
pre_order(buf_kernel,buf_size,&init_task,&count_process);
```   

- buf_kernel : It's not available to write the infomation in buf parameter directly. `buf_kernel` is temporary buf which has the same size of original `buf`. By `copy_to_user()`, whole contents of `buf_kernel` will be copied in `buf`.             
- buf_size : The same as the nr parameter        
- &init_task : It's the first process which is defined in sched.h. &init_task will be the root of the tree.                          
- count_process : It's the index of buf_kernel. When all recursive calls are finished, the # of whole entries will be saved in count_process.
      
#### 1-2-5 copy_to_user
 `buf` and `nr`, which are passed through as parameters, are on the user space. So in kernel mode,
the parameters cannot be written. So we must use `copy_to_user()` in `uaccess.h` to copy the informations
which we got in kernel mode to the user space.
      
```c
copy_to_user(buf, buf_kernel, sizeof(struct prinfo)*(*nr)));
```
`prinfo`s are copied from `buf_kernel` to `buf`.

```c
if(*nr > count_process) copy_to_user(nr, &count_process, sizeof(int));
```     
If `*nr` is bigger than the # of whole entries, write the # of whole entries in `nr`.
      
* copy_to_user error handling : `sys_ptree()` returns 0 when the error is occurred in `copy_to_user.`
                                This wasn't mentioned in specification of the project, but it can
                                occur serious problems in system call execution so we decided to 
                                include error handling for this case too.
      
### 1-3 ptree_dfs
  
```c
pre_order(struct prinfo * buf_kernel, int *buf_size, struct task_struct * curr, int * index)
```
    
* parameters
   - `struct prinfo * buf_kernel` : the buffer which contains the process' informations
   - `int *buf_size` : the size of buffer
   - `struct task_struct * curr` : While traversing, the current process node's task struct
   - `int * index` : buf's index which points where the current process' prinfo should be written
      
 * return value
 When whole the recursive calls are finished, all the process' informations are written in `buf_kernel`
by preorder, and the # of entries are written in `index`.
    
 * the flow of function call
 When the function is called, process' information in `curr` will be the indexth element of
`buf_kernel` and `*index` will increment. After that, recursive calls will be executed according to 
the cases below.
      
#### leaf case

If curr node doesn't have any child process, the curr node is leaf node.
Which can be represented by following statement.
  
```c
if(list_empty(&(curr->children)))
```

```c
/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
*/
static inline int list_empty(const struct list_head *head)
{
         return READ_ONCE(head->next) == head;
}
```    
  and according to assignment spec, make `first_child_pid` 0 and return
          
#### middle case

We need to call each children with oldest order to satisfy the preorder.
We just simply used `list_for_each()` for calling all each children.

```c
/**
 * list_for_each        -       iterate over a list
 * @pos:        the &struct list_head to use as a loop cursor.
 * @head:       the head for your list.
*/
#define list_for_each(pos, head) \
        for (pos = (head)->next; pos != (head); pos = pos->next)
```

#### meaning of if( (*index) < (*buf_size))

When the `index` become bigger than the buffer size, which means that the entries are more
 than the size of buffer, we don't write the values in the buffer 
 but only increments the `index` value.
            
#### handling the case when `curr` process is youngest sibling

The youngest sibling's listhead points the parent process' children listhead.
  So we shouldn't give sibling to member name parameter of `list_entry`. Instead, we must give children
   to member name parameter to get exact address.
            

## 2. Adding system call to kernel
  
### 2-1 `calls.S` modification
in `Linux-3.10-artik/arch/arm/kernel/calls.S`
    
```c 
CALL(sys_ptree) 
``` 
 added.
    
### 2-2 `unistd.h` modification
in `Linux-3.10-artik/arch/arm/include/asm/unistd.h`
    
```c
#define __NR_syscalls (380)
```
modified to
    
 ```c
 #define __NR_syscalls (384)
 ``` 
### 2-3 `syscalls.h` modification
in `Linux-3.10-artik/include/linux/syscalls.h`
    
```c 
asmlinkage long sys-ptree(struct prinfo *buf, int *nr)
```
added.
    
### 2-4 `Makefile` modification
in `Linux-3.10-artik/kernel/Makefile`
    
```c
Obj-y = ......... ptree.o 
```
added.
    
## 3. Implementing test program

### 3-1 `test.c` error check
```c
num_process = syscall(380,buf,&nr);
```
syscall ptree(380) return value is the number of process.
if error ocurred, 0 or < 0 is returned.

### 3-2 'test.c' print by stack-concept
we print process tree by stack-concept.
stack's element is pid-indent.
```c
struct pid_indent{
	int pid;
	int indent;
};
```
main function calls push_pr by for-loop.
```c
for(for_i = 0; for_i < nr; for_i++){
		pr_print( buf[for_i], push_pr(pr_stack, &pr_stack_top,buf, for_i));
}
```
push_pr return value of indent of current prinfo.

First, stack is empty.
just into stack.
```c
if ((*top) == -1){
		temp.indent =0;
		pr_stack[++(*top)]= temp;
		return 0;
}
```

Second, meet parent.
current indent is parent indent+1. 
and push into stack.
```c
if (buf[curr].parent_pid == pr_stack[(*top)].pid){
		temp.indent = pr_stack[*top].indent +1;
		pr_stack[++(*top)]= temp;
		return temp.indent;
	}
 ```
 
Third, if top is not parent, not bottom. (sibling)
pop stack top until meet parent. (actually, sibling is just one stack).
```c
	else {
		 while(1){
			  if( (*top) ==-1)	break;
			  if( pr_stack[*top].pid == buf[curr].parent_pid){
			 	  temp.indent = pr_stack[*top].indent+1;
			 	  pr_stack[++(*top)] = temp;
			 	 return temp.indent;
			  }
			  else{
			 	(*top)--;
			  }
		 }
	}
 ```

Last, indent print.
tap the number of indent count.
and print.
```c
void pr_print(struct prinfo info , int indent){
	int i;
	for(i =0; i<indent; i++){
		printf("\t");
	}
	printf("%s,%d,%ld,%d,%d,%d,%ld\n", info.comm, info.pid,
			info.state, info.parent_pid, info.first_child_pid,
			info.next_sibling_pid, info.uid);
}
```
# Lessons learned
## System call implementation
We've already done programming pretty much while going through another comp.sci courses, but not in the operating system. Implementing and modifing our own system call was a very new kind of challenge. The system call should not enter into forbidden memory area, and must be terminated at any kind of situations. Otherwise, the kernel goes into panic and whole the device stops. It was the most tricky part to guarantee that kernel never fails. Also, we had to notify that our new system call came in to kernel and therefore, we had to update the system call table.
## Brand new data structure : task_struct & Doubly linked list in linux kernel
Using struct list_head instead of implementing normal linked list makes handling dynamic memory address issues more comfortable. There is no need to allocate the memory each time for new element. Just put the list_head inside the struct makes neatly organized linked list. Then how can we find out the exact address where the structure is allocated? container_of(), which is a macro implemented in kernel.h returns the exact address of the structure by getting the list_head's address and the variable name of the list_head inside the struct. task_struct's hierachy is represented by the list_head. The list_head named children points the first child's list_head which is named sibling. And the sibling list_head points the next list_head which is also named sibling. Finally, the sibling list_head points the children list_head of it's parent task_struct. This structure makes traverse more simple and comfortable.
## Cooperating
In this project, actually we didn't seperated each of member's task. Instead, we went through the whole project parallely in each one's environment. Our cooperating in this project was just be gathered in same space and talking about the tricky parts while doing the project in each one's labtop. In other words, we three people done the same thing repeatedly. So we could understand every implementation about this project, but it took quiet much time. Therefore, we've decided to seperate the tasks and go through the project 'concurrently' in next projects. Then, maybe we can reserve the implementation time much shorter and it's the real 'cooperating'.

# Thank you.
