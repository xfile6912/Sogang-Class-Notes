# User Program(2)
------
### 개발 목표
-	Create, Remove, Open, Close, Filesize, Read, Write, Seek, Tell의 파일 시스템과 관련된 시스템 콜을 구현한다.
-	하나의 Process가 Critical section안에 있을 때, 다른 프로세스들은 Critical section에 못 들어가도록 하는 Synchronization을 구현한다.
-------
### System Call
- Create: 파일 이름과, 파일의 initial size를 통해 새로운 파일을 생성하게 된다. 파일 생성에 성공하게 되면 true를 반환하고, 파일 생성에 실패하게 되면 false를 반환한다.
- Remove: 파일 이름을 통해 해당 파일의 열림, 닫힘 여부에 관계없이 파일을 삭제하게 된다. 파일 삭제에 성공하면 true를 반환하고, 파일 삭제에 실패하면 false를 반환한다.
- Open: 파일 이름을 통해 해당 파일을 열게 된다. 파일 open에 성공하면 open한 파일에 대한 file descriptor를 지정하여 지정한 file descriptor를 반환하고, 실패시에는 -1을 반환한다.
- Close: file descriptor에 해당하는 파일을 닫게 된다.
- Filesize: file descriptor에 해당하는 파일의 size(byte size)를 반환한다.
- Read: file descriptor에 해당하는 파일을 size만큼 buffer로 읽어온다. Read에 성공하면 읽은 바이트 수를 반환하고, 실패하면 -1을 반환하며 file descriptor가 0인 경우에는 키보드로부터 읽어온다.
- Write: buffer로부터 size만큼을, file descriptor에 해당하는 파일에 쓰게 된다. 파일에 실제로 쓴 바이트 수를 반환하며 file descriptor가 1인 경우에는 console에 쓰게 된다.
- Seek: file descriptor에 해당하는 파일의 읽거나 쓸 다음 위치를 position으로 변경하게 된다.
- Tell: file descriptor에 해당하는 파일의 읽거나 쓸 다음 위치를 반환하게 된다.
-------
### 수행 결과
![2](https://user-images.githubusercontent.com/57051773/120828516-95ddb280-c597-11eb-9617-4ec962dcb132.jpg)
![3](https://user-images.githubusercontent.com/57051773/120828523-97a77600-c597-11eb-8094-5b2cbfe17ed6.jpg)
