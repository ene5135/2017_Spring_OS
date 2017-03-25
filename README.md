
Project 01

Wrting system call and adding to kernel

Introduction
This assignment includes implementing a new system call, ptree() in Linux. It returns the process tree information in depth-first-search order. To check whether the returned tree information is correct, we write a simple C program which calls ptree() system call. The program prints the entire process tree in pre-order using tabs to indent children with respect to their parents.

Implementation

1. Writing system call
  1-1 prinfo
    We used prinfo structure to save the process information while traversing the process tree.
    There is a struct description below.
    
    struct prinfo {
      long state;             /* current state of process */
      pid_t pid;              /* process id */
      pid_t parent_pid;       /* process id of parent */
      pid_t first_child_pid;  /* pid of oldest child */
      pid_t next_sibling_pid; /* pid of younger sibling */
      long uid;               /* user id of process owner */
      char commm[64];         /* name of program executed */
    };
    
    We declared the description in prinfo.h and included the file in include/linux as part of our solution.
  
  1-2 ptree
    ptree.c is the main part of this project. There is whole description of sys_ptree() system call.
    
    1-2-1 return value
      sys_ptree() returns the # of process entry or error number. Also ays_ptree() puts process 
      entries informations in buf by preorder and actual size of buf in nr.
    
    1-2-2 error check
      Before the specific implementation, the system call checks for the input arguments whether 
      they are correct or not. Mainly, there are four error cases. So we handle the errors by 
      four steps below.
      
      1. When the arguments (buf, nr) have NULL value : return -EINVAL;
      2. When the nr's address is not accessible.     : return -EFAULT;
      3. When the # of entries(*nr) is less than 1    : return -EINVAL;
      4. When the buf's address is not accessible.    : return -EFAULT;
        - In fourth step, we used access_ok macro which is defined in uaccess.h
        
        <whole definition of access_ok macro>
        
    1-2-3 read_lock, read_unlock
    
      We will traverse all the task_structs of whole processes at certain moment. Thus we need to
      lock the task list to avoid modification of the process' informations. We locked the tasklist
      using read_lock() until the traverse finishes. After the traverse is finished, we unlocked it
      by read_unlock.
      
      read_lock(&tasklist_lock);
      /* do the job... */
      read_unlock(&tasklist_lock);

      
    1-2-4 call recursive function
      We implemented the traversing algorithm using recursive call. We defined seperate recursive
      function, ptree_preorder(). So in sys_ptree(), what we have to do is just call the ptree_preorder
      once.
      
      <ptree_preOrder(temp_buf,nr,&init_task,&index);>
      
      temp_buf : It's not available to write the infomation in buf parameter directly.
                 temp_buf is temporary buf which has the same size of original buf.
                 By copy_to_user(), whole contents of temp_buf will be copied in buf.
      nr : The same as the nr parameter
      &init_task : It's the first process which is defined in sched.h. 
                   &init_task will be the root of the tree.
      index : It's the index of temp_buf. When all recursive calls are finished, 
              the # of whole entries will be saved in index.
      
    1-2-5 copy_to_user
       buf and nr, which are passed through as parameters, are on the user space. So in kernel mode,
      the parameters cannot be written. So we must use copy_to_user() in uaccess.h to copy the informations
      which we got in kernel mode to the user space.
      
      copy_to_user(buf, temp_buf, sizeof(struct prinfo)*(*nr)));
      
      By recursive call, prinfos are copied from temp_buf to buf.
      
      if(*nr > index) copy_to_user(nr, &index, sizeof(int));
      
      If *nr is bigger than the # of whole entries, write the # of whole entries in nr.
      
      copy_to_user error handling : sys_ptree() returns 0 when the error is occurred in copy_to_user.
                                    This wasn't mentioned in specification of the project, but it can
                                    occur serious problems in system call execution so we decided to 
                                    include error handling for this case too.
      
  1-3 ptree_dfs
  
    ptree_preOrder()
    
    parameters
      - struct prinfo * buf : the buffer which contains the process' informations
      - int *nr : the size of buffer
      - struct task_struct * curr : While traversing, the current process node's task struct
      - int * index : buf's index which points where the current process' prinfo should be written
      
    return value
      When whole the recursive calls are finished, all the process' informations are written in buf
    by preorder, and the # of entries are written in index.
    
    the flow of function call
      When the function is called, process' information in curr will be the indexth element of
      buf and *index will increment. After that, recursive calls will be executed according to 
      the cases below.
      
        - leaf case
          If curr node doesn't have any child process, the curr node is leaf node.
          Which can be represented by following statement.
          
          list_empty(&(curr->children))==true
          
          make first_child_pid 0 and return
          
        - middle case
          We need to call each children with oldest order to satisfy the preorder.
          list_for_each() macro was very useful.
          
          <list_for_each() description>
          
          ** meaning of (*index <= *nr)
            When the index become bigger than the buffer size, which means that the entries are more
            than the size of buffer, we don't write the values in the buffer 
            but only increments the index value.
            
          ** handling the case when curr process is youngest sibling
            The youngest sibling's listhead points the parent process' children listhead.
            So we shouldn't give sibling to parameter of list_entry. Instead, we must give children
            to parameter to get exact address.
            
        <preorder flow chart>
        description of list head and list_entry(container_of)
        

      
2. Adding system call to kernel
  
  2-1 calls.S modification
    in Linux-3.10-artik/arch/arm/kernel/calls.S
    
    CALL(sys_ptree) 
    
    added.
    
  2-2 unistd.h modification
    in Linux-3.10-artik/arch/arm/include/asm/unistd.h
    
    #define __NR_syscalls (380)
    
    modified to
    
    #define __NR_syscalls (384)
    
  2-3 syscalls.h modification
    in Linux-3.10-artik/include/linux/syscalls.h
    
    Asmlinkage long sys-ptree(struct prinfo *buf, int *nr)
    
    added.
    
  2-4 Makefile modification
    in Linux-3.10-artik/kernel/Makefile
    
    Obj-y = ......... ptree.o 
    
    added
    
3. Implementing test program

  3-1 test.c implementation
