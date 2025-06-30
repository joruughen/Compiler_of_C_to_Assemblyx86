int main() {
    int i, j, count;
    count = 0;
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 3; j++) {
            count = count + 1;
        }
    }
    printf("Count: %d\n", count);
    return 0;
}
