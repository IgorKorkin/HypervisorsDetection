function [number_of_layer] = calc_layers_with_jump(OriginalColumn , FrequencyProcent, JumpValueInTicks)

column_after_filtartion = filtr_one_column(OriginalColumn, FrequencyProcent);

if length(column_after_filtartion) > 1    
   
  first_order_difference = diff(column_after_filtartion);
  
  fprintf('FrequencyProcent = %i || max(abs(first_order_difference)) = %i \n', FrequencyProcent, max(abs(first_order_difference)) )
    
  i_start = 1;   
    
  i=2;
  jumps_count = 0;
  number_of_layer = 0;
    while (i<length(first_order_difference))   
         if ( JumpValueInTicks < abs( first_order_difference(i) ) ) 
%              && ( abs( diff_line(i) ) < 4000) 
             i_finish = i-1;
             one_line = column_after_filtartion(i_start:i_finish); % New sample = from the begining to the fump value
             koef = (i_finish-i_start)/length(first_order_difference); % весовой коэфициент
            
              freq_table = get_frequency_table(one_line);
    
            if number_of_layer < length(freq_table(:,1))
               number_of_layer = length(freq_table(:,1));
            end
         
%              mom4_column = mom4_column + ( sum((one_line - mean(one_line)).^4)./length(one_line) ) * koef; % центральный момент 4-го порядка                      
%              disp_column = disp_column + ( std(one_line, 1)^2 ) * koef; 
            
             i = i + 2;
             i_start = i;
             jumps_count = jumps_count + 1;
%            fprintf('\nНайден скачок № %i\n', i );
         end;
         i = i + 1;
    end;
%     fprintf('\n\\____________/\n');
    
%     fprintf('%i ', num_jumps );

    i_finish = length(first_order_difference) + 1;
    one_line = column_after_filtartion(i_start:i_finish); % организуем выборку с начала(**условно) и до разрыва
    koef = (i_finish-i_start)/length(first_order_difference); % весовой коэфициент
    
    freq_table = get_frequency_table(one_line);
    
    if number_of_layer < length(freq_table(:,1))
       number_of_layer = length(freq_table(:,1));
    end
    
%     mom4_column = mom4_column + ( sum((one_line - mean(one_line)).^4)./length(one_line) ) * koef; % центральный момент 4-го порядка                  
%     disp_column = disp_column + ( std(one_line, 1)^2 ) * koef; 
%     fprintf('\nУровень фильт. по частотности=%0.2f\nУровень величины скачка=%f\n', threshold_frequence_in, threshold_jump_in );
%     fprintf('Среднее арифм.=%0.2f\nДисперсия=%0.2f\n', res(1), res(2));
else
    number_of_layer = 0;
%     fprintf('\nУровень фильт.=%f\n скачки не найдены\n', threshold);
end

% res = [disp_column, mom4_column];