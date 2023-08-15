from fxpmath import Fxp
from pick import pick

action, index = pick(['Get info on fixed-point representation',
               'Convert a number to fixed-point precision',
               'Convert binary fixed-point representation to decimal'], 
               'What do you want to do?')
print(index)
# Get user input
if index == 0:
    fractional_bits = int(input("Enter the number of fractional bits: "))
    integer_bits = int(input("Enter the number of integer bits: "))
    fixed_point_num = Fxp(0.5, signed=True, n_word=integer_bits+fractional_bits, n_frac=fractional_bits)
    # Print fixed-point properties
    fixed_point_num.info(verbose=3)
elif index == 1:
    number = float(input("Enter a number: "))
    integer_bits = int(input("Enter the number of integer bits: "))
    fractional_bits = int(input("Enter the number of fractional bits: "))

    fixed_point_num = Fxp(number, signed=True, n_word=integer_bits+fractional_bits, n_frac=fractional_bits)
    fixed_point_str = fixed_point_num.bin()
    print(f"Fixed-point representation: {fixed_point_str}")
elif index == 2:
    fixed_point_str = input("Enter a fixed-point representation: ")
    fractional_bits = int(input("Enter the number of fractional bits: "))
    integer_bits = int(input("Enter the number of integer bits: "))
    # Truncate the string to the correct length
    fixed_point_str = fixed_point_str[-(integer_bits+fractional_bits+1):]
    # Zero pad the string to the correct length
    fixed_point_str = fixed_point_str.zfill(integer_bits+fractional_bits+1)
    fixed_point_num = Fxp("0b"+fixed_point_str, signed=True, n_word=integer_bits+fractional_bits+1, n_frac=fractional_bits)
    print(f"Decimal representation: {fixed_point_num.astype(float)}")