# Threads
------
### 개발 목표
-	busy-waiting으로 구현되어 비효율적인 Alarm Clock을 busy-waiting을 사용하지 않는 방법으로 작동할 수 있도록 구현한다.
-	threads의 priority를 고려하여 scheduling할 수 있도록 Priority Scheduling을 구현한다.
-	Priority scheduling을 하게 되면 priority가 낮은 process가 scheduling되지 않는 starvation이 발생할 수 있기 때문에 이를 막기 위한 aging을 구현한다.
-------
### 수행 결과
- priority-lifo 테스트 결과

![4](https://user-images.githubusercontent.com/57051773/120829257-2a481500-c598-11eb-9c07-a6b83454a545.jpg)
- make check 결과

![5](https://user-images.githubusercontent.com/57051773/120829266-2c11d880-c598-11eb-9171-d7af5c19b18b.jpg)
