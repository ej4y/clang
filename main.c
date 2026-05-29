#include <stdio.h>

int main() {
    char input[256];

    while(1) {
        printf("> ");

        // 한 줄 읽기
        fgets(input, sizeof(input), stdin); // 버퍼, 최대 읽기 비트, 스트림

        printf("You typed: %s\n", input);
    }

    return 0;
}