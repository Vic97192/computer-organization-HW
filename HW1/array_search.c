#include <stdio.h>

int arraySearch(int *p_a, int arr_size, int target)
{
    int result = -1;

    asm volatile(
        // Your code
        "addi x5,x0,0               \n\t" //counter = 0
        "loop:                      \n\t"
        "bge x5,%[arr_size],nothing \n\t" //if counter >= array_size:->nothing
        "lw x6,0(%[p_a])            \n\t" //x9 = *p_a
        "addi %[p_a],%[p_a],4       \n\t" //p_a += 4
        "addi x5,x5,1               \n\t" // counter += 1
        "bne x6,%[target],loop      \n\t" //if x9 != target:->loop

        "addi %[result],x5,-1        \n\t" //return counter to result, subtract back 1
        "nothing:                   \n\t"

        :[result] "+r"(result),[p_a] "+r"(p_a)
        :[arr_size] "r"(arr_size),[target] "r"(target)
        :"x5","x6");

    return result;
}

// Main function to test the implementation
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
    int target;
    fscanf(input, "%d", &target);
    fclose(input);

    int *p_a = &arr[0];

    int index = arraySearch(p_a, arr_size, target);

    // Print the result
    printf("%d ", index);
    printf("\n");

    return 0;
}
