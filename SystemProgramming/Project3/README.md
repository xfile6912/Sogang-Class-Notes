# SIC/XE Machine3(Linking, Loading 기능)
------
### 구현 명령(Shell)
-	progaddr [address] : loader 또는 run 명령어르 수행할 때 시작하는 주소를 지정
-	loader [object filename][object filename2] [...] : filename1, 2, ...에 해당하는 파일으 읽어서 linking 작업을 수해 후 가사 메모리에 그 결과를 기록
-	bp [address]: breakpoint르 지정
-	bp clear: 존재하는 breakpoint를 전부 삭제
-	bp: 존재하는 breakpoint르 전부 화면에 출력
-	run: 메모리에 load된 프로그램의 breakpoint까지 실행, breakpoint가 없으면 끝까지 실행
------
### 수행 결과
<img width="635" alt="스크린샷 2021-06-30 오후 5 39 47" src="https://user-images.githubusercontent.com/57051773/123930108-64a3a700-d9ca-11eb-9f9a-9804f07f42a1.png">
<img width="638" alt="스크린샷 2021-06-30 오후 5 41 13" src="https://user-images.githubusercontent.com/57051773/123930116-65d4d400-d9ca-11eb-8f9a-1ca10d884120.png">
