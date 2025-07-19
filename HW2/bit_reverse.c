"add t1,x0,%[b]     \n\t"
"addi %[add_cnt],%[add_cnt],1 \n\t"

"add %[b],x0,x0     \n\t"
"addi %[add_cnt],%[add_cnt],1 \n\t"

"add t0,x0,x0       \n\t"
"addi %[add_cnt],%[add_cnt],1 \n\t"

"loop_bit:              \n\t"

"bge t0,%[m],exit_bit   \n\t"
"addi %[others_cnt],%[others_cnt],1 \n\t"

"slli %[b],%[b],1   \n\t"
"addi %[others_cnt],%[others_cnt],1 \n\t"

"addi t2,x0,1       \n\t"
"addi %[add_cnt],%[add_cnt],1 \n\t"

"and t2,t1,t2       \n\t"
"addi %[others_cnt],%[others_cnt],1 \n\t"

"or %[b],%[b],t2    \n\t"
"addi %[others_cnt],%[others_cnt],1 \n\t"

"srli t1,t1,1       \n\t"
"addi %[others_cnt],%[others_cnt],1 \n\t"

"addi t0,t0,1       \n\t"
"addi %[add_cnt],%[add_cnt],1 \n\t"

"addi %[others_cnt],%[others_cnt],1 \n\t"
"jal x0,loop_bit        \n\t"


"exit_bit:              \n\t"
"addi %[others_cnt],%[others_cnt],1 \n\t"