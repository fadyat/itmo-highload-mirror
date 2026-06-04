# Напишите программу, которая строит суффиксный массив для заданной строки длиной не больше 50 символов.
#
# Input: <строка> (латинские буквы в нижнем регистре)
#
# Output: <n1> <n2> <n3> ... <nL> значения суффиксного массива через пробел
def suffix_array(s: str) -> list[int]:
    suffixes = [(s[i:], i) for i in range(len(s))]
    suffixes.sort()
    return [index for _, index in suffixes]


# Напишите программу, которая строит преобразование Барроуза-Уилера для заданной строки длиной не больше 50 символов.
#
# Input: <строка> (латинские буквы в нижнем регистре)
#
# Output: <строка> (преобразованная строка, в качестве символа окончания строки используйте $)
def burrows_wheeler_transform(s: str) -> str:
    s += '$'
    suffixes = [(s[i:], i) for i in range(len(s))]
    suffixes.sort()
    return ''.join(s[index - 1] for _, index in suffixes)


def main():
    s = input()
    print(*suffix_array(s))
    print(burrows_wheeler_transform(s))


if __name__ == '__main__':
    main()
