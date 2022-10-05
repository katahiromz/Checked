#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "Checked.hpp"

int main(void)
{
    CheckedArray<int, 6> array1;

    printf("0x%X\n", array1[0]);
    array1[0] = 10;
    printf("0x%X\n", array1[0]);

    CheckedVector<int> array2(10);
    array2[5] = 100;
    printf("0x%X\n", array2[0]);
    printf("0x%X\n", array2[5]);

    int array3[] = { 2, 3, 4 };
    CheckedArray<int, 3> array4(array3);
    array4[0] = array4[2];
    printf("0x%X\n", array4[0]);

    return 0;
}
