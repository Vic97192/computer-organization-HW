"add t1,t1,x0                   \n\t"
"addi %[add_cnt],%[add_cnt],1   \n\t"

"loop:                          \n\t"
"bge t1,%[N],break              \n\t"
"addi %[others_cnt],%[others_cnt],1 \n\t"

"andi t2,t1,1                   \n\t"
"addi %[others_cnt],%[others_cnt],1 \n\t"

"bne t2,x0,else                 \n\t"
"addi %[others_cnt],%[others_cnt],1 \n\t"

"addi t3,x0,1                   \n\t"
"addi %[add_cnt],%[add_cnt],1   \n\t"

"addi %[others_cnt],%[others_cnt],1 \n\t"
"jal x0,out                     \n\t"

"else:                          \n\t"
"addi %[others_cnt],%[others_cnt],1 \n\t"

"addi t3,x0,-1                  \n\t"
"addi %[add_cnt],%[add_cnt],1   \n\t"

"out:                           \n\t"
"fcvt.s.w f1,t3                 \n\t"
"addi %[others_cnt],%[others_cnt],1 \n\t"

"slli t4,t1,1                   \n\t"
"addi %[others_cnt],%[others_cnt],1 \n\t"

"addi t4,t4,1                   \n\t"
"addi %[add_cnt],%[add_cnt],1   \n\t"

"fcvt.s.w f2,t4                 \n\t"
"addi %[others_cnt],%[others_cnt],1 \n\t"

"fdiv.s f2,f1,f2                \n\t"
"addi %[fdiv_cnt],%[fdiv_cnt],1 \n\t"

"fadd.s %[pi],%[pi],f2          \n\t"
"addi %[fadd_cnt],%[fadd_cnt],1 \n\t"

"addi t1,t1,1                   \n\t"
"addi %[add_cnt],%[add_cnt],1   \n\t"

"addi %[others_cnt],%[others_cnt],1 \n\t"
"jal x0,loop                    \n\t"


"break:                         \n\t"
"addi %[others_cnt],%[others_cnt],1 \n\t"