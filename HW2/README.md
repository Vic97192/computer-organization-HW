excercise1.c:
怕會出現跟HW1 assignment 3的address一樣的問題，我先釐清了為什麼不能用 fadd.d，只能用fadd.s
typedef struct {
    float Re;
    float Im;
} Complex;
float是1 word, double是2 word
sizeof(struct Node*)是因為*是64 bit所以才會是8 byte,因此用2 word的問題和exercise1無關
complex multiply中複習分科的內容，(are + aim)(bre + bim) = (arebre - aimbim) + (arebim + aimbre)i

萊布尼茲的c:
for (int i = 0; i < iter; i++) {
    float term = (i % 2 == 0 ? 1.0 : -1.0) / (2*i + 1);
    pi += term;
}

至於i%2，原本以為是用fdiv.s，但%是取餘數的意思，也沒有"frem.s"，
查詢後發現可以用andi t0, t1, 1   
x6 = i & 1，相當於i%2。

原本寫：
"add t1,t1,x0           \n\t"
"bge t1,%[N],break      \n\t"
"andi t2,t2,1           \n\t" // i % 2
"beq t2,x0,else         \n\t"
"addi x7,x0,1           \n\t" // x7 = -1
"jal x0,out             \n\t"
"else:                  \n\t"
"addi x7,x0,-1          \n\t" // x7 = 1
"out:                   \n\t"
"mul x28,x5,2           \n\t" // 2*i
"addi x28,x28,1         \n\t" // 2*i+1
"fdiv.s x29,x7,x28      \n\t" // term(x29) = (i % 2 == 0 ? 1.0 : -1.0) / (2*i + 1)
"fadd.s %[pi],%[pi],x29 \n\t" // pi += term

但後來發現整數和浮點數的暫存器不能混用、loop label忘記標，後來透過重複使用暫存器處理。
slli比mul還要快，因此用slli代替mul。
注意t4(2*i+1)要轉成float

log.c:
我們想知道2的幾次方會剛好小於等於 N

比如
log2(1) → 0 （因為 2⁰ = 1）
log2(2) → 1 （因為 2¹ = 2）
log2(3) → 1 （因為 2¹ = 2 < 3 < 4）
log2(4) → 2 （因為 2² = 4）
log2(5) → 2 （因為 2² = 4 < 5 < 8）

所以應該是i = 1,初始值應該會小於N，若N>2^i就j loop，否則就跳出，回傳i-1。
python如下（原版）：
N = 10
log = 0
t1 = 1
while N>t1: #if N <= t1 -> exit
    log += 1
    t1 = 2**log
print(log-1)
但後來發現N=8 log=2,更正為：
N = 8
log = 1
t1 = 1
while N>=t1: if N< t1 ->exit
    log += 1
    t1 = 2**log
print(log-1)

bit reverse:
//copy b,t1
//result(b) = 0
for (int i(t0) = 0; i < m; i++) { // if i>=m -> exit
    result(b) <<= 1;
    result(b) |= (index(t1) & 1)(t2);  // 把 index 最右邊的 bit 放進 result
    index(t1) >>= 1;
}

發現input和output是同一個，可能要複製input

first run:
viclee@Mac CO_StudentID_HW2 % riscv64-unknown-elf-gcc -o main exercise1.c
exercise1.c: Assembler messages:
exercise1.c:74: Error: symbol `loop' is already defined
exercise1.c:88: Error: symbol `loop' is already defined
exercise1.c:97: Error: symbol `exit' is already defined
忽略了每個c的label不應該重複。

接著是complex mul中發現乘法有錯誤
在complex_mul((Complex){1.0f, 2.0f},(Complex){3.0f, 4.0f});之下
B_Re = -5.000000
result -5.000000+-6.000000  
後來發現C_Re會污染到B_Re，

可能解釋：
B_Re 是輸入（“f”）
C_Re 是輸出（”=f”）

GCC ：C_Re 是 output only，應該可以 reuse 某個暫存器，
於是它可能把 C_Re 也分配給 f12（例如 B_Re 用的暫存器）
結果在寫 C_Re 的時候就覆蓋了 B_Re 的值  

exercise2_1.c:
記得要回去loop並且終止條件要對就不會有太大問題
較為重要的是和以前不同，這裡沒有獨立的i去記數，要透過arr_size-1的方式處理

沒有加fadd.s f1,f1,%[id]，即使所有的counter都正確仍會全錯
以此推測評測標準為結果先算對，counter才會跟著計算。

產生測資是發現一個有趣的事情：
當我的數字是10^7 * 1，結果不會出錯，但10^8 * 1就會讓count全部出錯，並且result會少2（10000016)，
當迴圈次數大到某一個臨界值時，計數器累加溢位（wrap-around），後續所有判分欄位都失準。
但因為C code都是32 bit，private test case應該不會出這類數字，因此並沒有特別為這個而調整。

exercise2_2.c:
主要是研究 vsetvli t0, a4, e32, m1, ta, mu
可以設定el = 32 (sizeof(float)) 機器根據VLEN去決定vl（幾項），vl會放進t0。a4是決定我們的陣列還有幾筆數據，系統會幫我們根據這個算t0要多少，避免越界。

所以主要流程是：
loop:
if arr_size <= 0 -> exit
iteration 1: arr_size = a4
vsetvli->return t0
load vl筆data from p_h(h) to v0
load vl筆data from p_x(x) to v1
v2 = v0 * v1
v2 = v2 + id
store vl筆data to p_y(y)

h = h + vl*4
x = x + vl*4
y = y + vl*4

arr_size -= t0

j loop
exit

Important: 不能用addi %[h],%[h],4，因為t0不一定是四（若剩餘的項數<4）

exercise2_2.c: Assembler messages:
exercise2_2.c:33: Error: unrecognized opcode `vsetvli t0,a5,e32', extension `v' or `zve64x' or `zve32x' required
查詢了一下，應該使用riscv64-unknown-elf-gcc -march=rv64gcv -o main exercise2_2.c -lm 來啟用v extension

exercise2_2.c: Assembler messages:
exercise2_2.c:37: Error: illegal operands `vadd.vx v2,v2,fa5'

id應該要轉成vector register，但我怕指令只能辦到把一個scalar搬進一格，信好vfmv.v.f可以做到類似「複製每個scalar進一個vector」或scalar broadcast to vector。

exercise2_2.c: Assembler messages:
exercise2_2.c:38: Error: illegal operands `vadd.vx v2,v2,v0'
這裡 v0 是向量暫存器，不能被當作 scalar 來源
但 .vx 指的是要從 x0 ~ x31（整數暫存器）取 scalar。
所以其實可以直接用vfadd.vf（vector+float)

First run:
===== Question 2-2 =====
array size = 4
student id = 0
z  0000000000000000 ra 0000000000010244 sp 0000003ffffffae0 gp 0000000000026bd0
tp 0000000000000000 t0 0000000000000004 t1 0000000000000000 t2 000000007ebf85a0
s0 0000003ffffffb30 s1 0000000000000000 a0 0000000000000000 a1 0000000000000000
a2 0000000000000000 a3 0000000000000000 a4 0000000000000000 a5 0000000000000004
a6 0000000000000000 a7 0000000000000000 s2 0000000000000000 s3 0000000000000000
s4 0000000000000000 s5 0000000000000000 s6 0000000000000000 s7 0000000000000000
s8 0000000000000000 s9 0000000000000000 sA 0000000000000000 sB 0000000000000000
t3 0000000000000000 t4 0000000000028050 t5 0000000000028030 t6 0000000000028010
pc 00000000000102d8 va/inst 000000000107f2d7 sr 8000000200006020
An illegal instruction was executed!

後來知道我沒有開啟向量支援，故應該改成：
riscv64-unknown-elf-gcc -march=rv64gcv -o main exercise2_2.c -lm
spike --isa=rv64gcv pk main

second run:
===== Question 2-2 =====
array size = 4
student id = 0
output:  -0.000000  -0.056164  9533143598398561880899584.000000  0.000000
我先用python來寫出程式，透過step by step看程式應該怎麼跑：

a = "26.113884 29.327766 12.934367 61.528931 24.393055 36.206884 23.293595 21.180578 18.536312 84.477998 29.485476 84.255929 62.406174 82.741802 10.95326 47.335843 78.739438 13.185833 55.657887 60.131699 92.561924 49.912106 32.611975 63.537327 30.743154 98.163269 51.987503 45.194283 54.899475 1.74196 69.798284 16.64124 58.486347 23.759928 7.107788 71.031779 5.267374 54.840711 ......(arrayinput.txt)"
a = a.split(" ")
for i in range(len(a)):
    a[i] = float(a[i])
stack = []
for i in range(4):
    stack.append(a[i+4] * a[i])
    
print(stack)

然後我將程式精簡化成：
"    vsetvli t0, %[arr_size], e32, m1, ta, mu\n"
"    vmv.v.i v2, 0\n"
"    vmv.v.i v0, 0\n"
"    add t1,%[y],4\n" //debug api 放在y+4
"    add t2,%[y],8\n" //debug api 放在y+8
//"    vfmv.v.f v0, %[id]\n"
"    vle32.v v1, (%[h])\n"
"    vse32.v v1, (t1)\n" //store what is loaded y+4
"    vle32.v v2, (%[x])\n"
"    vse32.v v2, (t2)\n" //store what is loaded y+8
"    vmul.vv v0, v1, v2\n"
"    mv %[debug_vl], t0\n"
"    vse32.v v0, (%[y])\n"
我發現我設成
float h[4] = {1.00};
float x[4] = {1.00};
output = 0.0
float h[4] = {1.01};
float x[4] = {1.01};
output =  -301672447604667489146159864217600.000000
發現原來是我一直用整數的vmul.vv，但其實應該用vfmul.vv，改完之後終於正常了

查詢後也發現 vfmacc.vv v0, v1, v2 可以做到vfmul.vv在做的事情，在vmul.vv一直報錯被解決之前，查看邏輯是否正確基本上都是用vfmacc.vv

exercise 3_1:
延伸自exercise 2_1，挪出一個f2來儲存f1*f0，並將加法改成乘法，result = result*f2。架構基本雷同。

exercise 3_2:
延伸自exercise 3_1，注意flw要改成fld，offset要改成8，架構基本雷同

macro_define.h:
我在猶豫要不要將每一個counter都列入計算，怕可能會有例外，但local judge都是正確的，所以最終我就沒有將所有counter補上

error: lvalue required as left operand of assignment
   41 | #define macro_arraymul_baseline_cycle_count arraymul_baseline_cycle_count = add_cnt+sub_cnt+mul_cnt+div_cnt+lw_cnt+sw_cnt+others_cnt+fadd_cnt+fsub_cnt+fmul_cnt+fdiv_cnt+flw_cnt+fsw_cnt+dadd_cnt+dsub_cnt+dmul_cnt+ddiv_cnt+dlw_cnt+dsw_cnt;
      |                                                                           ^
但查詢網路都是c++相關的資料，沒找到c的解決辦法，後來直接重新下載後更改，問題就被解決了。