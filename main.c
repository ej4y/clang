#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h> 

int main() {
    char input[256];
    
    while(1) {
        printf("> ");

        // 출력 후 즉시 보이게 flush
        fflush(stdout);

        // fgets(버퍼, 최대 읽기 비트, 스트림) => 한 줄 읽기
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        } 

        input[strcspn(input, "\n")] = '\0';

        // strcmp와 다르게 \n를 길이에 포함하지 않고 비교
        if (strncmp(input, "exit", 4) == 0) {
            break;
        }

        if (strlen(input) == 0) {
            break;
        }

        pid_t pid = fork(); // 현재 프로세스 복사

        // pid == 0 => child process
        // pid > 0 => parent process
        if (pid < 0) {
            perror("fork failed");
            continue;
        }

        if (pid == 0) {
            // 현재 프로세스를 다른 프로그램으로 교체.
            // execlp(ls처럼 파일명만으로 가능) <=> execl(/bin/ls처럼 절대/상대 경로 필요)
            // execlp(실행할 프로그램 이름, 프로그램 이름, NULL 고정)
            execlp(input, input, NULL);

            // execlp 실패 시에만 아래에 도달
            perror("command failed");
            return 1;
        } else {
            // 부모는 child 끝날 때까지 대기
            wait(NULL);
        }
    }

    printf("shell terminated\n");

    return 0;
}