#include <iostream>
#include <bitset>
#include <iomanip>
#include <cmath>
#include <cstdint>

// Union q' representa el float en sus bits
union FloatBits {
    float value;
    uint32_t bits;
};

// Funcion --> muestra los bits de un numero flotante
void displayBits(float num) {
    FloatBits fb;
    fb.value = num;
    std::cout << "Bits: " << std::bitset<32>(fb.bits) << std::endl;
}

// Extrae las partes del numero flotante (signo, exponente, significando)
void extractParts(FloatBits fb, int &sign, int &exponent, uint32_t &mantissa) {
    sign = (fb.bits >> 31) & 1; // Extrae el bit de signo
    exponent = (fb.bits >> 23) & 0xFF; // Extrae el exponente
    mantissa = fb.bits & 0x7FFFFF; // Extrae el significando 
    
    if (exponent != 0) {
        mantissa |= (1 << 23);
    }
}

// Funcion -> realiza la multiplicacion de significandos y normaliza
void multiplyAndNormalize(uint32_t mant1, uint32_t mant2, int &expResult, uint64_t &mantResult) {
    // Multiplica los significandos
    mantResult = (uint64_t)mant1 * (uint64_t)mant2;

    // Si hay overflow en la multiplicacion, normaliza
    if (mantResult & (1ULL << 47)) {
        mantResult >>= 1; // Desplaza un bit a la derecha
        expResult++; // Ajusta el exponente
    }
}

// Funcion -> suma exponentes y ajusta bias
int sumExponents(int exp1, int exp2) {
    return exp1 + exp2 - 127; 
}

// Verifica underflow/overflow
bool checkOverflowUnderflow(int expResult) {
    if (expResult >= 255) {
        std::cout << "Overflow: Resultado es infinito\n";
        return true;
    }
    if (expResult <= 0) {
        std::cout << "Underflow: Resultado es 0\n";
        return true;
    }
    return false;
}

int main() {
    float num1, num2;

    std::cout << "Ingresa el primer valor en punto flotante: ";
    std::cin >> num1;
    std::cout << "Ingresa el segundo valor en punto flotante: ";
    std::cin >> num2;

    std::cout << "Bits del numero 1: ";
    displayBits(num1);
    std::cout << "Bits del numero 2: ";
    displayBits(num2);

    // Uniones para los numeros
    FloatBits fb1, fb2;
    fb1.value = num1;
    fb2.value = num2;

    // Extrae partes de los numeros flotantes
    int sign1, sign2, exp1, exp2;
    uint32_t mant1, mant2;

    extractParts(fb1, sign1, exp1, mant1);
    extractParts(fb2, sign2, exp2, mant2);

    if (num1 == 0.0f || num2 == 0.0f) {
        std::cout << "Resultado: 0.0 (multiplicacion x 0)\n";
        return 0;
    }

    int expResult = sumExponents(exp1, exp2);

    uint64_t mantResult;
    multiplyAndNormalize(mant1, mant2, expResult, mantResult);

    if (checkOverflowUnderflow(expResult)) {
        return 0;
    }

    int signResult = sign1 ^ sign2; 
    uint32_t finalResult = (signResult << 31) | ((expResult & 0xFF) << 23) | ((mantResult >> 24) & 0x7FFFFF);

    FloatBits result;
    result.bits = finalResult;
    std::cout << "Resultado implementado obtenido: " << result.value << std::endl;

    float expected = num1 * num2;
    std::cout << "Resultado esperado: " << expected << std::endl;

    return 0;
}
