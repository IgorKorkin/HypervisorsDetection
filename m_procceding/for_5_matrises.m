
function [ res_matrix_1, res_small_1, ...
        res_matrix_2, res_small_2, ...
        res_matrix_3, res_small_3, ...
        res_matrix_4, res_small_4, ...
        res_matrix_5, res_small_5, res_allinall ] = ...
for_5_matrises( OriginalMatrix_1, OriginalMatrix_2, ... 
                OriginalMatrix_3, OriginalMatrix_4, OriginalMatrix_5 )

    [res_matrix_1, res_small_1] = one_matrix( OriginalMatrix_1 );
    [res_matrix_2, res_small_2] = one_matrix( OriginalMatrix_2 );
    [res_matrix_3, res_small_3] = one_matrix( OriginalMatrix_3 );
    [res_matrix_4, res_small_4] = one_matrix( OriginalMatrix_4 );
    [res_matrix_5, res_small_5] = one_matrix( OriginalMatrix_5 );

    for i=1:4
        min_array = [res_small_1(i,1), res_small_2(i,1), res_small_3(i,1), res_small_4(i,1), res_small_5(i,1)];
        res_allinall(i,1) = min( min_array );
        
        max_array = [res_small_1(i,2), res_small_2(i,2), res_small_3(i,2), res_small_4(i,2), res_small_5(i,2)];
        res_allinall(i,2) = max( max_array );
    end
    clear min_array max_array
end

