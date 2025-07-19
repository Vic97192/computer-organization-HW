"loop:                      \n\t"

"bge x0,%[arr_size],exit    \n\t"
"addi %[others_cnt],%[others_cnt],1 \n\t"

"flw f0,0(%[h])             \n\t"
"addi %[flw_cnt],%[flw_cnt],1 \n\t"
"flw f1,0(%[x])             \n\t"
"addi %[flw_cnt],%[flw_cnt],1   \n\t"

"fmul.s f1,f1,f0            \n\t"
"addi %[fmul_cnt],%[fmul_cnt],1 \n\t"

"fadd.s f1,f1,%[id]         \n\t"
"addi %[fadd_cnt],%[fadd_cnt],1 \n\t"

"fsw f1,0(%[y])             \n\t"
"addi %[fsw_cnt],%[fsw_cnt],1   \n\t" 

"add %[h],%[h],4            \n\t"
"addi %[add_cnt],%[add_cnt],1   \n\t" 

"add %[x],%[x],4            \n\t"
"addi %[add_cnt],%[add_cnt],1   \n\t" 

"add %[y],%[y],4            \n\t"
"addi %[add_cnt],%[add_cnt],1   \n\t" 

"addi %[arr_size],%[arr_size],-1 \n\t"
"addi %[add_cnt],%[add_cnt],1   \n\t" 

"addi %[others_cnt],%[others_cnt],1 \n\t"
"jal x0,loop                \n\t"

"exit:                      \n\t"
"addi %[others_cnt],%[others_cnt],1 \n\t"
