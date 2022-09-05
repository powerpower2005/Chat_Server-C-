# Chat_Server C++

## C++ WIN 소켓으로 만드는 채팅서버


### 실행방법

#### Chat_Server\ChattingServer\Debug\의 Client.exe와 Server.exe를 통해서 실행



















## 실행 중 나오는 에러들

### 1. addr.sin_addr.s_addr = inet_addr("127.0.0.1");
에서 inet_addr을 사용할 때 Error가 뜸 => 문자열을 직접적으로 포맷에 맞게 변형시켜주는 메서드인데... 오류가 뜸

=> SDL 검사를 하지 않는 것으로 해결

### 2. git에 커밋할 때 자꾸 VC.opendb Error가 뜬다.

=> .gitignore 파일 작성
=> 깃이 뭔가 안올라간다?? ignore 파일을 작성해보자
https://github.com/github/gitignore/blob/main/VisualStudio.gitignore



### 추가하고 싶은 기능
Linux 에서 돌리기
서버 - 클라이언트 닉네임 목록
클라이언트 - 서버가 가지고 있는 닉네임 설정 금지
GUI로 만들기
