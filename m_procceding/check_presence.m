% Функция определяем присутствует ли элемент исходного стобца (T_i) в
% отфильтрованном вариационном ряду (var_ryad)
% Если элемент присутствует возвращается 1, в противном случае 0.

function [Is_present] = check_presence(Ti, FreqTable)

    Is_present = 0;
   
    len = length(FreqTable);
    for i=1:len
            if FreqTable(i) ==  Ti
               Is_present = 1;
               break;
            end
end

