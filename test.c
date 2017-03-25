#include <linux/prinfo.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#define buf_size 500
struct pid_indent{
	int pid;
	int indent;
};

void pr_print(struct prinfo , int );
int push_pr(struct pid_indent *pr_stack, int *top, struct prinfo *buf,int curr);


int main(){

	int nr = buf_size;
	struct prinfo buf[buf_size] = {0};
	int for_i=0;
	long num_process=0;
	struct pid_indent pr_stack[buf_size] = {0};
	int pr_stack_top = -1;
	num_process = syscall(380, buf, &nr);
	if(num_process == 0){
		fprintf(stderr, "Error! : copy_to_user\n");
	}
	if(num_process < 0){
		fprintf(stderr, "Error code %d\n", errno);
		return 1;
	}
	//make_ptree(buf, 0, 0, nr);
	for(for_i = 0; for_i < nr; for_i++){
		pr_print( buf[for_i], push_pr(pr_stack, &pr_stack_top,buf, for_i));
	}


	return 0;
}

void pr_print(struct prinfo info , int indent){
	int i;
	for(i =0; i<indent; i++){
		printf("\t");
	}
	printf("%s,%d,%ld,%d,%d,%d,%ld\n", info.comm, info.pid,
			info.state, info.parent_pid, info.first_child_pid,
			info.next_sibling_pid, info.uid);
}
int push_pr(struct pid_indent *pr_stack, int *top, struct prinfo *buf, int curr){

	struct pid_indent temp;
	temp.pid = buf[curr].pid;
	if ((*top) == -1){
		temp.indent =0;
		pr_stack[++(*top)]= temp;
		return 0;
	}
	// top is parent
	
	if (buf[curr].parent_pid == pr_stack[(*top)].pid){
		temp.indent = pr_stack[*top].indent +1;
		pr_stack[++(*top)]= temp;
		return temp.indent;
	}
	
	// top is not my parent
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
}
