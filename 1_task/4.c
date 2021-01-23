#include <stdint.h>
#include <stdio.h>

/*
 *  Закодируем наши символы в двоичном виде.
 *  Наш алфавит [0,1..9,A,B..Z,a,b..z] мощностью 62.
 *  Бдуем хранить его в обратном пордке при кодировании
 *  (потому что так проще кодировать)
 *  тогда единицы в соответствующих позициях означают наличие
 *  соответствующего символа из алфавита.
 *  Это позволяет выделять ровно 8 байт на каждое множество
 *  (что и требовалось в задании)
 */
uint64_t
encode_symbol(char symbol)
{
    uint64_t one = 1;
    uint64_t symbol_difference = 0;
    uint64_t big_letters_start = 10;
    uint64_t small_letters_start = 36;
    if (symbol>='0' && symbol <='9')
    {
        symbol_difference = (symbol - '0');
    } else
    if (symbol>='A' && symbol<='Z')
    {
        symbol_difference = (symbol - 'A' + big_letters_start);
    } else
    if (symbol>='a' && symbol<='z')
    {
        symbol_difference = (symbol - 'a' + small_letters_start);
    }
    return one << symbol_difference;
}

/*
 * Декодируем в соответствии с порядком кодирования
 * Проходимся по всем цифрам, затем по буквам обоих регистров
 * Смотрим является ли единицей соответствующий бит
 * Если да, то добавляем в строку результата букву(цифру)
 */
void
decode_array(uint64_t array, char decoded[])
{
    int32_t index = 0;
    int8_t numbers_count = 10;
    int8_t letters_count = 26;
    uint64_t mask = 1;
    for(int8_t i = 0; i < numbers_count; ++i) { //all numbers
        if (mask & array)
        {
            decoded[index] = '0' + i;
            ++index;
        }
        mask <<= 1;
    }
    for(int8_t i = 0; i < letters_count; ++i) { // big letters
        if (mask & array)
        {
            decoded[index] = 'A' + i;
            ++index;
        }
        mask <<= 1;
    }
    for(int8_t i = 0; i < letters_count; ++i) { // small letters
        if (mask & array)
        {
            decoded[index] = 'a' + i;
            ++index;
        }
        mask <<= 1;
    }
    decoded[index] = '\0';
}

int
main()
{
    char symbol='a';
    char result[62];
    uint64_t result_array = 0;
    uint64_t current_array = 0;
    while((symbol>='0' && symbol <='9') || (symbol>='a' && symbol<='z') ||
    (symbol>='A' && symbol<='Z') || symbol == '|' || symbol=='&' || symbol=='^' || symbol=='~')
    {
        symbol = getchar();
        switch (symbol) {
            case '|':
                result_array |= current_array;
                current_array = 0;
                break;
            case '&':
                result_array &= current_array;
                current_array = 0;
                break;
            case '^':
                result_array ^= current_array;
                current_array = 0;
                break;
            case '~':
                result_array = ~result_array;
                break;
            default:
                current_array |= encode_symbol(symbol);
                break;
        }
    }
    decode_array(result_array, result);
    printf("%s", result);
}
