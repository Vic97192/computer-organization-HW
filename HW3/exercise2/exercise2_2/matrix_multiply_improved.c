void matrix_multiply(int *A, int *B, int *Output, int i, int k, int j) {

    int block = 8;
    int row_limit = (i + block - 1) / block;
    int column_limit = (j + block - 1) / block;
    int depth_limit = (k + block - 1) / block;

    int box_height,box_length,box_e;

    for (int row=0; row<row_limit; row++) {
        for (int column=0; column<row_limit; column++) {
            for (int element=0; element<row_limit; element++) {

                if ((row+1) * block > i) {
                    box_height = i - row*block;
                } 
                else {
                    box_height = block;
                }

                if ((column+1) * block > j) {
                    box_length = j - column*block;
                } 
                else {
                    box_length = block;
                }

                if ((element+1) * block > k) {
                    box_e = k - element*block;
                } 
                else {
                    box_e = block;
                }

                for (int ii=0; ii<box_height; ii++) {
                    for (int jj=0; jj<box_length; jj++) {
                        int sum = Output[(block*row + ii) * j + (block*column + jj)];
                        for (int kk=0; kk<box_e; kk++) {
                            sum = sum + A[(block*row + ii) * k + (block*element + kk)] * B[(block*element + kk) * j + (block*column + jj)];
                        }
                        Output[(block*row + ii) * j + (block*column + jj)] = sum;
                    }
                }
            }
        }
    }
}