void matrix_transpose(int n, int *dst, int *src) {
    
    int block = 8;
    int limit = (n + block - 1) / block;
    int box_length,box_height = 0;

    for (int row=0; row<limit; row++) {
        for (int column=0; column<limit; column++) {

            if ((column+1) * block - n > 0) {
                box_length = n - column*block;
            } 
            else {
                box_length = block;
            }

            if ((row+1) * block - n > 0) {
                box_height = n - row*block;
            } 
            else {
                box_height = block;
            }

            for (int i=0; i<box_height; i++) {
                for (int j=0; j<box_length; j++) {
                    dst[n * (block*column + j) + (block*row + i)] = src[n * (block*row + i) + (block*column +j)];
                }
            }
        }
    }
}