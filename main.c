#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <fcntl.h> 

int main() {
    while(1) {
        char *args[64];
        char *token;
        char input[256];
        char cwd[1024];
        char *outfile = NULL;
        
        getcwd(cwd, sizeof(cwd));
        printf("%s > ", cwd);
        
        // 출력 후 즉시 보이게 flush
        fflush(stdout);
        
        // fgets(버퍼, 최대 읽기 비트, 스트림) => 한 줄 읽기
        if (fgets(input, sizeof(input), stdin) == NULL) {
            continue;
        } 

        // strcspn(문자열, 문자열) => 첫 번째 문자열에서 두 번째 문자열을 인덱스를 찾음
        // \0은 문자열의 끝을 나타내는 NULL 문자("\0"는 1바이트를 차지하는 하나의 문자로 봄)
        input[strcspn(input, "\n")] = '\0';

        // strcmp와 다르게 \n를 길이에 포함하지 않고 비교
        if (strncmp(input, "exit", 4) == 0) {
            break;
        }

        if (strlen(input) == 0) {
            continue;
        }

        int i = 0;
        // strtok(문자열, 문자열) => 첫 번째 문자열에서 두 번째 인자인 구분자 문자열을 찾은 뒤 \0(문자열의 끝을 나타내는 NULL 문자)으로 바꿈 그리고 앞의 토큰을 반환함
        // 예) "ls -al" => "ls\0-al" => ls 반환
        token = strtok(input, " ");
        while(token != NULL) {
            args[i++] = token;
            // strtok의 첫 번째 인자에 NULL을 넣으면 이전 작업을 그대로 진행함
            // strtok의 첫 번째 인자에 새로운 문자열을 넣으면 새로운 작업을 진행함
            // 그 다음 토큰 반환 "-al"
            token = strtok(NULL, " ");
        }

        // 끝 표시(execvp는 NULL을 만날 때까지 인자를 읽음)
        args[i] = NULL;

        if (i == 0) {
            continue;
        }

        // {echo, "\"Hello, World!\"", ">", "hello.txt", NULL, ??, ??, ??}
        for (int i = 0; args[i] != NULL; i++) {
            if (strcmp(args[i], ">") == 0) {
                args[i] = NULL;
                outfile = args[i + 1];
                break;
            }
        }

        if (strcmp(args[0], "cd") == 0) {
            if (args[1] == NULL) {
                printf("cd directory");
            } else {
                if (chdir(args[1]) != 0) {
                    perror("cd");
                }
            }

            continue;
        }

        pid_t pid = fork(); // 현재(부모 = 쉘) 프로세스 복사

        // pid == 0 => 자식 프로세스
        // pid > 0 => 부모 프로세스
        if (pid < 0) {
            perror("fork failed");
            continue;
        }

        if (pid == 0) {
            if (outfile != NULL) {
                // 파일 디스크럽터(FD)
                // 프로세스가 시작되면 아래의 것들이 기본적으로 적용
                // FD	의미
                // 0	stdin
                // 1	stdout
                // 2	stderr
                int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);

                if (fd < 0) {
                    perror("open");
                    exit(1);
                }

                // FD 덮어쓰기
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }
            
            // 현재 프로세스를 다른 프로그램으로 메모리 이미지 교체
            // execlp(ls처럼 파일명만으로 가능) <=> execl(/bin/ls처럼 절대/상대 경로 필요)
            // execlp(실행할 프로그램 이름, 프로그램 이름, NULL 고정)
            execvp(args[0], args);

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