function [ FreqTable ] = get_frequency_table( T )
% This function returns frequency table of one column
% Return:
% First column = value
% Second column = frequency
% Third column = percentage
% ----------------------------
% e.g
% 2353 	2 		 0.20
% 2312 	6		 0.60
% 2303 	125	 	12.5
% 2326 	140 	14
% 2300 	320 	32
% 2315 	403 	40.3
% ----------------------------

    FreqTable = [T(1), 1]; % init

    len_T = length(T);
    for i=1:len_T

        result = num_in_freq_table(T(i), FreqTable);
        flag = result(1);
        number = result(2);

       if flag
            % increase the number of frequency (or count number)
            FreqTable(number, 2) = FreqTable(number, 2) + 1;
        else
            % add new item at the end of the frequency table
            table_end = length(FreqTable(:,1));       
            FreqTable(table_end + 1, 1) = T(i);
            FreqTable(table_end + 1, 2) = 1;
        end   

    end

    FreqTable = sortrows(FreqTable, 2);
    
%     delete zero element
    
    FreqTable (:,3) = FreqTable(:,2).*100/len_T;

end