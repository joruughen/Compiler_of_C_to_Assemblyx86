int main() {
    int x, y, z;
    x = 10;
    y = 20;
    if (x < y) {
        z = 0;
        while (z < 5) {
            x = x + 1;
            z = z + 1;
            while (z < 5) {
                x = x + 1;
                z = z + 1;
            }
        }
    }

    printf("x: %d, y: %d, z: %d\n", x, y, z);
    return 0;
}
