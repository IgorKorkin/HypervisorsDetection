function [present] = num_in_freq_table(Ti, FreqTable)
% Function check the presence of the element or 'Ti' in the frequency table
% RESULT
% if elem is present (1, NumberOfElement)
% if elem is not present (0, 0)

	present(1) = 0; % = flag of present
	present(2) = 0; % = NumberOfElement

	len = length( FreqTable(:,1) );
	for i=1:len
			if FreqTable(i,1) ==  Ti
			
				present(1) = 1; % = flag of present 
				present(2) = i; % = NumberOfElement   
			   break;
			   
			end	
	end
end