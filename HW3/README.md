**Exercise 1:**
spike是模擬器，tags、ways都是用array去模擬cache的行為
tags[set_index * ways + way_index] 
= tags[set_index][way_index] = (addr >> idx_shift) | VALID = 拿出 addr 中的 tag 部分（通常是高位），並設定valid為1(valid（1ULL << 63）是tag最高位) 1ULL => Unsigned Long Long, 保證64bit.
在這個位運算中：idx = (address >> offset_bits) & (number_of_sets - 1)

為什麼要減 1？為什麼是 & (number_of_sets - 1) 而不是直接 & number_of_sets？
因為 number_of_sets - 1 剛好是二進位下「全為 1 的遮罩」，用來抓出 index 位元。
number_of_sets = 64 = 1000000，number_of_sets - 1 = 63 = 0b111111

c++中，new和malloc幾乎同義

original FIFO:
remain = -1
for i in range(i = way,i = 0,i-=1):
    if tag[sets][i] = 0:
        remain = i if i > remain else remain
if remain = -1: put addr in tag[sets][remain]
else:
    for i in range(i = way-1,i=1, i-=1):
        tag[sets][i-1] = tag[sets][i]
    tag[sets][0] = addr

但這樣太慢了，而且現實中cache不會真的在miss時調整way位置，即使邏輯正確，實務上不會這麼做。
實際上會用另一個資料去記錄way的順序。
我用一個list (fifo_way) 去記錄那一set的進入順序，但我的tag[]不會主動去做fifo，tag[]我會把踢出去的那格分給要放進去的東西。至於fifo_way就有點像一隻手去抓（決定）誰要被踢出，決定依據就是fifo

在python中可以import queue，這樣fifo_way = [queue.Queue(), queue.Queue(), queue.Queue(), ...]
我查到cpp也有相似的實作方式，使用的是#include <queue>，
vector queue是 std::vector<std::queue<size_t>> fifo_way; 
size_t的意思是「queue中每個element」是無號整數
pop<= 4 1 2 3 <=push (way)
      3 2 1 0 (element)

原本的構想：f是fifo_data簡寫
if f[idx].size<4:
    for way in range(ways):
        if (tag[idx][way] & VALID) == 0: #只看valid bit是否等於0(無效的)（找空位放，這裡invalid可以視為未註冊在f，所以用size去看沒有問題）
        #原先寫tag[idx][way] == NULL，但無效的也有可能有資料，所以不能單純用block是否等於0判斷。
            f[idx].push(way) #way是新資料放的地方，push(way) = 登記way在queue的尾端
            break
else:
    way = f[idx].front() #登記被踢掉的，等回把新東西放上去。
    f[idx].pop()
    f[idx].push(way)

tag[idx][way] = (addr)...

在header中要註冊我的fifo_data,並且在constructor(cache_sim_t::cache_sim_t)中初始化（因為要先知道set數目才能初始），我第一次嘗試時放錯cache_sim_t(copy constructor)，導致segfault。rhs = right-hand side（等號右邊的東西）cache_sim_t::cache_sim_t(const cache_sim_t& rhs)，意思是：「我要用一個現成的 cache_sim_t 物件來建立一個新的」，這是拿來還原回上一個狀態（做 模擬快照（checkpoint））的。copy constructor因為不會隨時被呼叫，所以不能把resize放在那邊。




**Exercise 2_1:**
*Matrix: A[row][column] 第幾排第幾列*
C vs Python Matrix:
[[1,2,3],[4,5,6],[7,8,9]] = [1,2,3,4,5,6,7,8,9] in c (row-wise) row：橫，column：直
python = c : a[i][j] = a[i*3+j],a[0][1] = a[0*3+1] = a[1] = 2
C: dst[y + x * n] = src[x + y * n] Python: dst[x][y] = src[y][x] (for x in range: for y in range:)

假設一個block size就能裝下一個row。自然而然每個column對應不同的index，
src是取column，我認為幾乎無法read hit，畢竟用過的就不會再用到了，所以專注於dst(row wise的放)
假如我已經用完了，等她再write一次，這樣我就算set全滿也沒差，裡面都是我不需要的東西
write: 但就怕有大於set的資料寫，這樣不夠的部分(miss)就只能not write allocate（直接寫進記憶體），很花時間
read: 與其一次「交換」，不如專注在把東西放在對的位置。
重點：不要替換。
想像一個 2 way 4 set cacheline = 2 word

1000 1001 1010 1011

1100 1101 1110 1111

10 tag 0 index, 0 byte offset.

無tile(column wise read): m m => index 0 : 1000 1001/ 1100 1101 ==> h h ==> m m
有tile: 
先畫出框框，再對框框內的東西做事
跟積分一樣，取最左上的當作基準點(i,j)，再去框框內部做transpose，

原始 (python)：
src = [[0,1,2,3],[4,5,6,7],[8,9,10,11],[12,13,14,15]]
dst = [[0,0,0,0],[0,0,0,0],[0,0,0,0],[0,0,0,0]]

n = 4
block = 2
for i in range(0,n//2): #n//2的2是block，但這裡我要直覺呈現故不用變數
    for j in range(0,n//2): #i j代表有幾個框框
        for x in range(block):
            for y in range(block):
                print(src[2*i+y][2*j+x])
                dst[2*j+x][2*i+y] = src[2*i+y][2*j+x]
print(dst)

不要去理超出範圍的事情 block照常運作，但反正你只要用if去判斷那個東西要不要搬就好，這樣超出了也不會出錯。

改版：
n = 5
block = 2
src = [[i + j * n for i in range(n)] for j in range(n)]
dst = [[0]*n for _ in range(n)]

for i in range(0,n//2+1): #+1是為了彌補沒有填滿的tile,原本是只有滿足(n//2 * block)^2 的大小。
    for j in range(0,n//2+1):
        for x in range(block): #一次遍歷block*block
            for y in range(block):
                if block*i+y<n and block*j+x<n: #沒有出邊界就做。
                    print(src[block*i+y][block*j+x])
                    dst[block*j+x][block*i+y] = src[block*i+y][block*j+x]
print(dst)


block size candidates: 8/16(太大怕replacement)，
最後 8=> Improved ratio:  1.5327778486193173
    16=>Improved ratio:  1.447057551196204

但improve ratio依然<1.6，我認為如果把if放越外面的迴圈，越能消除branch miss penalty。比如如果事先把tile的長寬決定好，這樣進入迴圈時就不會多出penalty在已經知道會miss但懶得處理的if block*i+y<n and block*j+x<n上

最終版本：

n = 5
block = 2
src = [[i + j * n for i in range(n)] for j in range(n)]
dst = [[0]*n for _ in range(n)]
limit = (n + block - 1) // block #需要時才+1。偶數變奇數，奇數變偶數。避免偶數被誤傷，比如4//2=2，但4+2//2=3，這時把 4-1 +2 // 2=2。而奇數已經多1了，所以-1也無所謂。這樣就自然而然總數+1。

for row in range(0,limit): #+1是為了彌補沒有填滿的tile,原本是只有滿足(n//2 * block)^2 的大小。
    for column in range(0,limit): # column是直的排數，row是橫的排數
        
        if (column+1)*block - n > 0: #因為column會變動，所以不會第一次就偵測到超出範圍。但只要迴圈讓column長度超出範圍，我們就收窄框框到column長度與邊界的差距。n-(block*i) = 正方形全長-之前已完成的數目
            box_length = n - column*block
        else:
            box_length = block #length變相就是column排數
        if (row+1)*block - n > 0:
            box_height = box_height = n - row*block
        else:
            box_height = block

        for i in range(box_height): #一次遍歷block*block。column是直的那一排數字數量，row是橫的那一排的數字數量
            for j in range(box_length):
                #複習時，按照i j變化思考。 i box_height = row，以此類推
                print(src[block*row + i][block*column + j]) 
                dst[block*column + j][block*row + i] = src[block*row + i][block*column + j]

**Exercise 2_2:**
a[i][k] * b[k][j] = c[i][j]
i= a 的 row ; j= b的column ; k = row / column 個別的元素
我先直接拿transpose的來改。tile移動方式，A是左右->上下，B是上下->左右

原版：
n = 4
a = [[i + j * n for i in range(n)] for j in range(n)]
b = [[i + j * n for i in range(n)] for j in range(n)]
c = [[0 for _ in range(n)] for _ in range(n)]
block = 2

for x in range(n//2):
    for y in range(n//2):
        for i in range(2):
            for j in range(2):
                sum = c[2*x+i][2*x+j]
                for k in range(2):
                    print(f"{2*x+i} {2*y+k} * {2*y+k} {2*x+j}")
                    sum = sum + a[2*x+i][2*y+k]*b[2*y+k][2*x+j]
                c[2*x+i][2*x+j] = sum

Output = 
[[ 56  62   0   0]
 [152 174   0   0]
 [  0   0 324 362]
 [  0   0 452 506]]
正確答案是
[[ 56  62  68  74]
 [152 174 196 218]
 [248 286 324 362]
 [344 398 452 506]]

我以為和transpose一樣只要移動方塊就好，若a b切成s[2][2]，如果a的s[0][0] s[0][1]沒有走過兩次，和b的s[0][0] s[1][0]，那就會少掉 output的s[0][1]和s[1][0]
最後總結就是tile也要看成一個矩陣。概念是大矩陣->小矩陣->矩陣相乘

概要版
n = 4
a = [[i + j * n for i in range(n)] for j in range(n)]
b = [[i + j * n for i in range(n)] for j in range(n)]
c = [[0 for _ in range(n)] for _ in range(n)]
block = 2

for x in range(n//2):
    for y in range(n//2):
        for z in range(n//2):
            for i in range(2):
                for j in range(2):
                    sum = c[2*x+i][2*x+j]
                    for k in range(2):
                        print(f"{2*x+i} {2*z+k} * {2*z+k} {2*y+j}")

                        sum = sum + a[2*x+i][2*z+k]*b[2*z+k][2*y+j]

                    c[2*x+i][2*y+j] = sum

print(c)

因為接下來只剩應對超出邊界的辦法，改變tile大小，所以我就沒有寫成python，但基本上是一樣的寫法
雖然助教說不會改變matrix的大小，但保險起見我還是分別算row column element的limit。