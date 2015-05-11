function [ res_details, res_summary ] = one_matrix( OriginalMatrix )

    for i=1:length(OriginalMatrix(1, :) )  
        res_details(:,i) = one_column(OriginalMatrix(:, i));
    end
    
    for i=1:length(res_details(:, 1) )  
        res_summary(i,1) = min(res_details(i,1:10));
        res_summary(i,2) = max(res_details(i,1:10));
    end
    

