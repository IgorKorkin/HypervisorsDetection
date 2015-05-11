function [ number_of_layers ] = one_column( OriginalColumn )

JumpValueInTicks = 300;

number_of_layers(1) = calc_layers_with_jump(OriginalColumn, 2, JumpValueInTicks);
number_of_layers(2) = calc_layers_with_jump(OriginalColumn, 5, JumpValueInTicks);
number_of_layers(3) = calc_layers_with_jump(OriginalColumn, 10, JumpValueInTicks);
number_of_layers(4) = calc_layers_with_jump(OriginalColumn, 15, JumpValueInTicks);

number_of_layers = number_of_layers';

end

