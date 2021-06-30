# SIC/XE Machine1(Shell, Memory Space, Opcode Table)
------
### 구현 명령(Shell)
-	h[elp] : Shell에서 실행 가능한 모든 명령어들의 리스트를 화면에 출력
-	d[ir] : 현재 directory에 있는 파일들을 출력
- q[uit] : Shell을 종료
- hi[story] : 현재까지 사용한 명령어들으 순서대로 번호와 함께 출력
- du[mp] [start, end] : 할당되어 있는 메모리의 내용으 출력(메모리 주소, 메모리내용(16진수); 메모리 내용(ASCII, 20~7E))
- e[dit] address, value : 메모리의 address번지 값을 value에 지정된 값으로 변경
- f[ill] start, end, value : 메모리의 start번지부 end번지 까지의 값을 value에 지정된 값으로 변경
- reset : 메모리 전체를 전부 0을 변경
- opcode mnemonic: 명령어에 해당하는 opcode를 출력
- opcodelist: opcode hash table의 내용을 출력
------
### 수행 결과
<img width="551" alt="1" src="https://user-images.githubusercontent.com/57051773/123925706-21dfd000-d9c6-11eb-83d9-80296bf3b4c6.png">
<img width="552" alt="2" src="https://user-images.githubusercontent.com/57051773/123925716-23a99380-d9c6-11eb-8ef1-f5c5285d7890.png">
<img width="552" alt="3" src="https://user-images.githubusercontent.com/57051773/123925731-26a48400-d9c6-11eb-9bdc-21ea1b61abdc.png">
