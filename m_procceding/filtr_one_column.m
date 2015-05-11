function [ColumnAfterFiltr] = filtr_one_column(OriginalColumn, ThresholdPercentage)

% I) Get frequency table and find valeus, 
% whoes frequence if bigger than ThresholdPercentage 

freq_table_original = get_frequency_table(OriginalColumn);
k = 0;
if (ThresholdPercentage > 0)
    for i=1:length(freq_table_original(:,1))
        if freq_table_original(i,3) >  ThresholdPercentage
            k = k + 1;
            freq_table_after_filtr(k,:) = [freq_table_original(i,1), freq_table_original(i,2), freq_table_original(i,3)];
        end
    end
else
    freq_table_after_filtr = freq_table_original;
end

clear freq_table_original i
% OUTPUT = freq_table_after_filtr

%-------------------------------------

% II) Create column which includes only high-frequency values

if k > 0
    column_after_filtr = 0;

    if (ThresholdPercentage > 0)
        k = 0;
        for i=1:length(OriginalColumn)
            if check_presence(OriginalColumn(i), freq_table_after_filtr(:,1))
                k = k + 1;
                column_after_filtr(k) = OriginalColumn(i);
            end
        end
    else
       column_after_filtr = OriginalColumn';
    end

    clear OriginalColumn i  k
    % OUTPUT = column_after_filtr

    %-------------------------------------

    % III) Thats all - copy original column to the output

    ColumnAfterFiltr = column_after_filtr';

    clear column_after_filtr 
else
    ColumnAfterFiltr = 0;
end
    

% OUTPUT = ColumnAfterFiltr

%-------------------------------------