"addi %[log],x0,0       \n\t"
"addi %[add_cnt],%[add_cnt],1 \n\t"

"addi t1,x0,1           \n\t"
"addi %[add_cnt],%[add_cnt],1 \n\t"

"addi t0,x0,1           \n\t"
"addi %[add_cnt],%[add_cnt],1 \n\t"

"loop_log:                  \n\t"

"blt %[N],t1,exit_log       \n\t" 
"addi %[others_cnt],%[others_cnt],1 \n\t"

"addi %[log],%[log],1   \n\t"
"addi %[add_cnt],%[add_cnt],1 \n\t"

"sll t1,t0,%[log]       \n\t"
"addi %[others_cnt],%[others_cnt],1 \n\t"

"addi %[others_cnt],%[others_cnt],1 \n\t"
"jal x0,loop_log        \n\t"


"exit_log:                  \n\t"
"addi %[others_cnt],%[others_cnt],1 \n\t"

"addi %[log],%[log],-1  \n\t"
"addi %[add_cnt],%[add_cnt],1 \n\t"