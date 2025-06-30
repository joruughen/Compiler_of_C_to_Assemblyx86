int main() {
    unsigned int x;
    int y;
    x = 100U;
    y = 50;
    x = x + y;
    printf("Result of x + y: %u\n", x);
    x = -100U;
    printf("Result of x after negation: %u\n", x);
    return 0;
}
