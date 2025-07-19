#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *input = fopen(argv[1], "r");
    if (!input) {
        fprintf(stderr, "Error opening file: %s\n", argv[1]);
        return 1;
    }
    int arr_size;
    fscanf(input, "%d", &arr_size);
    int arr[arr_size];

    // Read integers from input file into the array
    for (int i = 0; i < arr_size; i++) {
        int data;
        fscanf(input, "%d", &data);
        arr[i] = data;
    }
    fclose(input);

    int *p_a = &arr[0];

    // array a bubble sort
    /* Original C code segment
    for (int i = 0; i < arr_size - 1; i++) {
        for (int j = 0; j < arr_size - i -1; j++) {
            if (*(p_a + j) > *(p_a + j + 1)) {
                int tmp = *(p_a + j);
                *(p_a + j) = *(p_a + j + 1);
                *(p_a + j + 1) = tmp;
            }
        }
    }
    */

    for (int i = 0; i < arr_size - 1; i++) {
        for (int j = 0; j < arr_size - i - 1; j++) {
            asm volatile(
                //"lw x5,4(%[p_a])\n\t" // x5 = p_a
                "slli x5,%[j],2\n\t"
                "add x6,%[p_a],x5\n\t" // x6 = p_a + j
                "addi x7,x6,4\n\t" // x7 = p_a + j + 1
                "lw x28,0(x6)\n\t" // x28 = *(p_a + j) (or *(x6))
                "lw x29,0(x7)\n\t" // x29 = *(p_a + j + 1)
                "blt x28,x29,exit\n\t"
                "add x30,x0,x28\n\t" // int tmp = *(p_a + j);
                "add x28,x0,x29\n\t" // *(p_a + j) = *(p_a + j + 1);
                "add x29,x0,x30\n\t" // *(p_a + j + 1) = tmp;
                "sw x28,0(x6)\n\t"
                "sw x29,0(x7)\n\t"
                "exit:\n\t"
                :
                :[p_a] "r"(p_a),[j] "r"(j)
                :"x5","x6","x7","x28","x29","x30"
            ); 
        }
    }

    p_a = &arr[0];
    for (int i = 0; i < arr_size; i++)
        printf("%d ", *p_a++);
    printf("\n");
    return 0;
}
