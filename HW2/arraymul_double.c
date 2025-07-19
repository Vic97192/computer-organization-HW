"loop:                      \n\t"

"bge x0,%[arr_size],exit    \n\t"
"addi %[others_cnt],%[others_cnt],1 \n\t"

"fld f0,0(%[h])             \n\t"
"addi %[dlw_cnt],%[dlw_cnt],1 \n\t"
"fld f1,0(%[x])             \n\t"
"addi %[dlw_cnt],%[dlw_cnt],1 \n\t"

"fmul.d f2,f1,f0            \n\t"
"addi %[dmul_cnt],%[dmul_cnt],1 \n\t"

"fmul.d %[result],%[result],f2            \n\t"
"addi %[dmul_cnt],%[dmul_cnt],1 \n\t"

"add %[h],%[h],8            \n\t"
"addi %[add_cnt],%[add_cnt],1 \n\t"

"add %[x],%[x],8            \n\t"
"addi %[add_cnt],%[add_cnt],1 \n\t"

"addi %[arr_size],%[arr_size],-1 \n\t"
"addi %[add_cnt],%[add_cnt],1 \n\t"

"addi %[others_cnt],%[others_cnt],1 \n\t"
"jal x0,loop                \n\t"

"exit:                      \n\t"
"addi %[others_cnt],%[others_cnt],1 \n\t"