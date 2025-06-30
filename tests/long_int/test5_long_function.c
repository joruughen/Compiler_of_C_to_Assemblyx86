long int multiply(long int a, long int b) {
    return a * b;
}
int main() {
    long int result;
    result = multiply(100000L, 200000L);
    printf("Resultado de 100000L * 200000L = %ld\n", result);
    return 0;
}
