def binary_inverse(num):
    if num <= 0:
        raise ValueError("Input number should be greater than 0")

    result = ""
    fraction = 1 / num

    while len(result) < 20:
        fraction *= 2
        if fraction >= 1:
            result += "1"
            fraction -= 1
        else:
            result += "0"

    return "0." + result

if __name__ == "__main__":
    n = input("Enter a number:")
    print(binary_inverse(int(n)))