#include <stdio.h>

int sum(int a, int b) {
    if (a < b) return a + b;
    if (a > b) return b + a;
    return a + b - b + b - a + a;
}

int main() {
    printf("ciao\n");

    for (int i = 0; i < 45; i++) {
        if (2 == 3) {
            printf("Fucking crazy man.\n");
        } else {
            printf("Boooring...\n");
        }
    }

    return 0;
}
