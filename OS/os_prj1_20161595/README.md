# User Program
------
### 개발 목표
-	User Program을 구동할 수 있는 Pintos 환경을 구축한다.
-	halt, wait, exit, execute, read(stdin), write(stdout), fibonacci, max_of_four_int의 system call 기능을 구현한다.
-	User program이 잘못된 Memory 영역(Kernel address space, Unmapped virtual memory, Null pointer)에 접근하지 않도록 하는 기능을 구현한다.
-------
### System Call
- Halt: shutdown_power_off()함수에 의해 Pintos를 종료하게 된다.
- Exit: current user program을 종료하게 되고, 종료 상태인 exit status를 커널로 반환하게 된다. 
- Exec: file 이름과 argument들을 통해 새로운 process를 만들어 수행하게 된다.
- Wait: 자식 프로세스가 끝날 때까지 부모 프로세스가 기다리게 된다.
- Read: STDIN으로부터 입력을 받아 버퍼에 저장 후, 저장한 바이트 수를 반환하게 된다.
- Write: 버퍼에 있는 내용을 size만큼 STDOUT으로 출력하게 되고 성공 시 size를 반환하고 실패 시 -1을 반환한다.
- Fibonacci: argument로 들어온 수에 대한 피보나치 수를 반환하게 된다.
- Max_of_four_int: argument로 들어온 4개의 정수 중에서 가장 큰 수를 반환하게 된다.
-------
### 수행 결과
- Fibonacci 및 max_of_four_int 시스템 콜 수행 결과
![1](https://user-images.githubusercontent.com/57051773/120827798-c6711c80-c596-11eb-89e5-02db88cfaed8.jpg)
