/*
 * modulos.c
 *
 *  Created on: 24 oct. 2024
 *      Author: nicotina
 */

#include "modulos.h"


// Implementación de la función mod
int mod(int a, int n) {
    int res = a % n;
    if (res < 0) res += n;
    return res;
}



