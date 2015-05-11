% Расчёт среднего арифметического и дисперсии по выбранному столбцу с
% учётом фильтарции по вар.ряду и разрывов 
% Просьба деда от 24 мая 2011
%

% Пример использования:
%
% 1) Входные значения:
% TV3(:,3) - столбец значений времени трассы, переданный из Excel
% 0.02 - пороговое значение уровня частостности
% 1000 - пороговое значение уровня разрыва (скачка)
% 
% 2) Вызов функции:
% mean_disp_column_jump(TR3(:,1), 0.02, 1000);
% 
% 3) Возвращаемые данные - дисперсия и момент 4-ого порядка для столбца
% 
% В результате работы функции в области вывода Matlab будут 
% выведены рассчитанные среднее арифметическое и дисперсия

function [res] = mom4_disp_column_jump(T_column_in, threshold_frequence_in, threshold_jump_in)

% 1) Входной столбец T_column передаётся через Excel
T_column = T_column_in;

% 3) Фильтрация столбца с учётом заданного уровня фильтрации 
% threshold_in = 0.05;
threshold_frequence = threshold_frequence_in;
T_column_filtr = column_filtr(T_column, threshold_frequence);
threshold_jump = threshold_jump_in;
mom4_column = 0;    
disp_column = 0;
 
if length(T_column_filtr) > 1    
    % 4) Определяем присутствует ли разрыв и для каждого участка рассчитываем
    % статистики
   
    diff_line = diff(T_column_filtr);
       
  
   fprintf('%i \n', max(abs(diff_line)) )
   
        
    i_start = 1;   
    
%     plot (T_column_filtr, '.r');
%     hold on 
%     plot (diff_line, 'o');

%      расчёт статистик
%     fprintf('\n/``````````````\\\n');
    i=2;
    num_jumps = 0;
    while (i<length(diff_line))   
         if ( threshold_jump < abs( diff_line(i) ) ) 
%              && ( abs( diff_line(i) ) < 4000) 
             i_finish = i-1;
             one_line = T_column_filtr(i_start:i_finish); % организуем выборку с начала(**условно) и до разрыва
             koef = (i_finish-i_start)/length(diff_line); % весовой коэфициент
             mom4_column = mom4_column + ( sum((one_line - mean(one_line)).^4)./length(one_line) ) * koef; % центральный момент 4-го порядка                      
             disp_column = disp_column + ( std(one_line, 1)^2 ) * koef; 
             i = i + 2;
             i_start = i;
             num_jumps = num_jumps + 1;
%            fprintf('\nНайден скачок № %i\n', i );
         end;
         i = i + 1;
    end;
%     fprintf('\n\\____________/\n');
    
%     fprintf('%i ', num_jumps );

    i_finish = length(diff_line) + 1;
    one_line = T_column_filtr(i_start:i_finish); % организуем выборку с начала(**условно) и до разрыва
    koef = (i_finish-i_start)/length(diff_line); % весовой коэфициент
    mom4_column = mom4_column + ( sum((one_line - mean(one_line)).^4)./length(one_line) ) * koef; % центральный момент 4-го порядка                  
    disp_column = disp_column + ( std(one_line, 1)^2 ) * koef; 
%     fprintf('\nУровень фильт. по частотности=%0.2f\nУровень величины скачка=%f\n', threshold_frequence_in, threshold_jump_in );
%     fprintf('Среднее арифм.=%0.2f\nДисперсия=%0.2f\n', res(1), res(2));
else
%     fprintf('\nУровень фильт.=%f\n скачки не найдены\n', threshold);
end

res = [disp_column, mom4_column];