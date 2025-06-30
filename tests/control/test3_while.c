int main() {
    int x, sum;
    x = 0;
    sum = 0;
    while (x < 10) {
        sum = sum + x;
        x = x + 1;
        printf("x: %d, sum: %d\n", x, sum);
    }
    return 0;
}
