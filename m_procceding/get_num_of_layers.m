function [ res ] = get_num_of_layers( OriginalColumn , FrequencyProcent)
% Return [ A B ]
% A - FrequencyProcent (INOUT const)
% B - number of layers (OUT)

column_after_filtartion = filtr_one_column(OriginalColumn, FrequencyProcent);
table_after_filtartion = get_frequency_table(column_after_filtartion);

res = [ FrequencyProcent, length(table_after_filtartion(:,1)) ];
end

