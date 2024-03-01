#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int a, int b)
{
    printf("Enter multiplicand:\n");
    scanf_s("%d", &a);
    printf("Enter multiplier:\n");
    scanf_s("%d", &b);
    int arr[33]; //массив с ответом в доп коде
    int arrA[33]; //массив множимого в доп коде
    int arrB[33]; //массив множителя в доп коде
    int arrA2[33]; //массив отрицательного значения множимого в доп коде
    int carry;
    //Инициализация всех массивов нулями
    for (int i = 0; i < 33; i++) {
        arr[i] = 0;
        arrB[i] = 0;
        arrA[i] = 0;
        arrA2[i] = 0;
    }
    //Добавление множителя b в массив и преобразование b в доп код
    int count = 31;
    int n = abs(b);
    while (n) {
        arrB[count] = n % 2;
        n /= 2;
        count--;
    }
    //Если b отрицательное
    if (b < 0) {
        for (int i = 0; i < 33; i++) {
            arrB[i] = arrB[i] + 1;
            if (arrB[i] == 2) {
                arrB[i] = 0;
            }
        }
        arrB[32] = arrB[32] + 1;
        if (arrB[32] == 2) {
            carry = 1;
            arrB[32] = 0;
            int count = 31;
            while (carry == 1) {
                if (arrB[count] = arrB[count] + carry > 1) {
                    arrB[count] = 0;
                    carry = 1;
                }
                else {
                    arrB[count] = arrB[count] + carry;
                    carry = 0;
                }
                count--;
            }
        }
    }
    //Инициализация нулями
    for (int i = 0; i < 16; i++) {
        arrB[i] = 0;
    }
    count = 31;
    //Заполнение правых битов итогового массива множителем b
    while (count > 15) {
        arr[count] = arrB[count];
        count--;
    }
    //Добавление множимого a в массив и преобразование a в доп код
    n = a;
    count = 15;
    while (n) {
        arr[count] = 0;
        arrA[count] = n % 2;
        n /= 2;
        count--;
    }
    //Если a отрицательное
    if (a < 0) {
        for (int i = 0; i < 33; i++) {
            arrA[i] = arrA[i] + 1;
            if (arrA[i] == 2) {
                arrA[i] = 0;
            }
        }
        arrA[32] = arrA[32] + 1;
        if (arrA[32] == 2) {
            carry = 1;
            arrA[32] = 0;
            int count = 31;
            while (carry == 1) {
                if (arrA[count] = arrA[count] + carry > 1) {
                    arrA[count] = 0;
                    carry = 1;
                }
                else {
                    arrA[count] = arrA[count] + carry;
                    carry = 0;
                }
                count--;
            }
        }
    }
    //Дополнительный код для отрицательного множимого
    for (int i = 0; i < 33; i++) {
        arrA2[i] = arrA[i] + 1;
        if (arrA2[i] == 2) {
            arrA2[i] = 0;
        }
    }
    arrA2[32] = arrA2[32] + 1;
    if (arrA2[32] == 2) {
        carry = 1;
        arrA2[32] = 0;
        int count = 31;
        while (carry == 1) {
            if (arrA2[count] = arrA2[count] + carry > 1) {
                arrA2[count] = 0;
                carry = 1;
            }
            else {
                arrA2[count] = arrA2[count] + carry;
                carry = 0;
            }
            count--;
        }
    }

    printf("Multiplier B:\n");
    //Вывести значение множителя b
    for (int i = 0; i < 33; i++) {
        printf("%d", arrB[i]);
    }
    printf("\n");
    printf("Multiplicand A:\n");
    //Вывести значение множителя a
    for (int i = 0; i < 33; i++) {
        printf("%d", arrA[i]);
    }
    printf("\n");

    printf("Negative multiplicand A:\n");
    //Вывести отрицательное значение множителя а
    for (int i = 0; i < 33; i++) {
        printf("%d", arrA2[i]);
    }
    printf("\n\nBooth's algorithm:\n");
    //Алгоритм Бута
    for (int i = 0; i < 16; i++) {
        carry = 0;
        //Значение двух правых битов 00 или 11, сдвиг вправо 
        if (arr[32] == arr[31]) {
            for (int i = 32; i > 1; i--) {
                arr[i] = arr[i - 1];
            }
            arr[0] = arr[1];
        }
        else {
            //Значение двух правых битов 10, вычитание и сдвиг вправо
            if (arr[32] == 0) {
                for (int i = 0; i < 33; i++) {
                    printf("%d", arrA2[i]);
                }
                printf("-");
                printf("\n");
                carry = 0;
                for (int i = 32; i >= 0; i--) {
                    if (arr[i] + arrA2[i] + carry > 2) {
                        arr[i] = 1;
                        carry = 1;
                    }
                    else if (arr[i] + arrA2[i] + carry > 1) {
                        arr[i] = 0;
                        carry = 1;
                    }
                    else {
                        arr[i] = arr[i] + arrA2[i] + carry;
                        carry = 0;
                    }
                }
                for (int i = 0; i < 33; i++) {
                    printf("%d", arr[i]);
                }
                printf("\n");
            }
            //Значение двух правых битов 01, сложение и сдвиг вправо
            else if (arr[32] == 1) {
                for (int i = 0; i < 33; i++) {
                    printf("%d", arrA[i]);
                }
                printf("+");
                printf("\n");
                carry = 0;
                for (int i = 32; i >= 0; i--) {
                    if (arr[i] + arrA[i] + carry > 2) {
                        arr[i] = 1;
                        carry = 1;
                    }
                    else if (arr[i] + arrA[i] + carry > 1) {
                        arr[i] = 0;
                        carry = 1;
                    }
                    else {
                        arr[i] = arr[i] + arrA[i] + carry;
                        carry = 0;
                    }
                }
                for (int i = 0; i < 33; i++) {
                    printf("%d", arr[i]);
                }
                printf("\n");
            }
            for (int i = 32; i > 1; i--) {
                arr[i] = arr[i - 1];
            }
            arr[0] = arr[1];
        }
        for (int i = 0; i < 33; i++) {
            printf("%d", arr[i]);
        }
        printf("\n");
    }
    for (int i = 32; i > 1; i--) {
        arr[i] = arr[i - 1];
    }
    arr[0] = arr[1];
    for (int i = 0; i < 33; i++) {
        printf("%d", arr[i]);
    }
    printf(" <- is final result\n");

    //Преобразовать результат в десятичное число
    int decimal = 0;
    int go = 0;
    for (int h = 0; h <= 32; h++) {
        //Преобразовать результат в десятичное число, если число отрицательное
        if ((a < 0 & b>0) | (b < 0 & a>0)) {
            if (arr[h] + 1 == 2) {
                arr[h] = 0;
            }
            else {
                arr[h] = 1;
            }
        }
        if (arr[h] == 1) {
            decimal = decimal + pow(2, abs(h - 32));
        }
    }
    if ((a < 0 & b>0) | (b < 0 & a>0)) {
        int temp = 0;
        decimal = decimal + 1;
        temp = decimal;
        decimal = decimal - 2 * temp;
    }
    printf("Result in decimal form: %i", decimal);
    return decimal;
}
