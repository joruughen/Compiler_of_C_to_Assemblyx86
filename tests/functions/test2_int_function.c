int increment(int x) {
    x = x + 1;
    return x;
}
int main() {
    int a;
    a = 10;
    a = increment(a);
    printf("Value of a after increment: %d\n", a);
    return 0;
}
