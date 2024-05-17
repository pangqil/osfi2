[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/uSvFxfBr)
[![Open in Visual Studio Code](https://classroom.github.com/assets/open-in-vscode-718a45dd9cf7e7f842a935f5ebbe5719a5e09af4491e668f4dbf3b35d5cca122.svg)](https://classroom.github.com/online_ide?assignment_repo_id=15007683&assignment_repo_type=AssignmentRepo)
# Multi-thread를 사용하여 Gaussian Elimination 방식의 연립방정식 풀기
This example project is written in C, and tested with make and bash scripts.

### The assignment
- $n \times n$ 행렬 이진 파일과 $n$ 벡터 이진 파일에서 double 크기의 데이터를 읽어 행렬 A와 벡터 B에 저장한 뒤, $AX = B$를 푸는 $n$ 벡터 $X$를 구해 $n$ 벡터 이진 파일에 저장하는 프로그램을 완성한다. 프로그램에서 이 세 파일의 이름은 인자로 받으며 argv[1]과 argv[2]는 입력으로 각각 $n \times n$ 행렬 이진 파일명과 $n$ 벡터 이진 파일명에 해당하고 배열 $A$와 $B$에 로드된다. argv[3]은 답 벡터 $X$를 저장할 파일 이름이다. argv[4]는 작업을 수행할 thread의 개수이다. 행렬 $A$는 일차원 배열 또는 이차원 배열과 같이 자유롭게 구성할 수 있다. 현재 부분적으로 실행되는 프로그램이 hw3.c로 제공되는데 이는 과제 1에 제공되는 hw1.c와 동일하다.

- 연립방정식을 풀기 위한 과정은 과제1의 명세서를 참고한다.
- POSIX API의 일부를 wrapper 함수의 형태로 wrapper.c에서 제공하므로, 가급적 제공되는 이 함수들을 이용한다.
- Pthread_create()를 이용하여 작업 스레드를 생성한다. 작업 스레드는 자신에게 할당된 작업을 수행한다. 스레드들 간에 작업을 공평하게 나누는 방법은 프로세스들이 작업을 나누는 것과 동일하므로 과제2의 명세서를 참고한다. 메인 스레드는 작업을 맡지않고 작업 스레드들의 작업이 모두 끝나기를 기다린다.
- 작업 스레드들은 전역 변수를 공유하기 때문에 과제2에서 사용했던 공유메모리는 필요하지 않다. 
- Gaussian elimination은 한 단계의 작업이 모두 끝나야 다음 단계로 넘어갈 수 있다. 예를 들어 행렬 A를 upper triangular matrix로 만들 때 대각선에 위치한 원소 A[i][i]의 아래쪽 원소들 (A[i+1][i] ~ A[n-1][i])을 모두 0으로 만드는 작업이 끝난 뒤에야 원소 A[i+1][i+1]의 아래쪽 원소들을 0으로 만드는 작업을 시작할 수 있다. 자신이 맡은 일을 먼저 끝낸 자식 프로세스들은 다른 자식 프로세스들이 작업을 완료할 때까지 기다려야 한다. 모든 자식 프로세스들이 일을 끝냈으면 다 같이 다음 작업을 시작한다. 이와 같이 모든 작업 프로세스들이 기다려야 하는 시점을 정해주는 기능이 POSIX API에서는 barrier라는 이름으로 제공된다. Barrier에 관련된 함수들은 다음과 같다.
  + Pthread_barrierattr_init() : barrier에 사용할 특성을 초기화한다.
  + Pthread_barrierattr_setpshared() : barrier를 여러 프로세스가 사용할지 정한다.
  + Pthread_barrier_init() : barrier 변수를 초기화한다. 여기에 barrier 특성과 같이 동작할 프로세스의 개수를 넣는다.
    * 과제 2에서는 barrier 변수를 공유메모리에 선언했지만 작업 스레드들은 전역 변수에 접근할 수 있으므로 전역 변수로 선언한다.
  + pthread_barrier_wait() : 앞에서 정한 개수의 프로세스들이 여기서 동기화한다.
  + pthread_barrier_destroy() : barrier 변수를 종료한다.
  
- Gaussian elimination에서 barrier를 제대로 설정하는 것은 어느 정도 고민이 필요하지만 두 번째 과제를 성공했다면 세 번째 과제에서는 두 번째 과제의 내용을 그대로 이용할 수 있다.

- 프로그램을 모두 작성했거나 수정했으면 다음과 같이 컴파일한다.

`make all`

- 제대로 컴파일되면 hw3, hwdiff, showdata 실행 파일이 만들어진다. hw3는 다음과 같이 실행할 수 있다. 여기서 자식 프로세스를 3개 생성시켰다.

`./hw3 A2.dat B2.dat X2.dat 3`

- 프로그램이 정상적으로 종료되면 쉘에게 0을 반환하고 이진 파일 X2.dat를 생성한다. hw3이 4개의 인자를 받지 못하면 다음 에러 메시지를 표준에러로 출력하며 1을 반환한다.

`Usage: ./hw3 <mat> <invec> <outvec> <np>`

- 이진 행렬 A2.dat의 내용을 확인하려면 첫 번째 인자로 m을 넣어 다음과 같이 실행한다.

`./showdata m A2.dat`

- 이진 벡터 X2.dat의 내용을 확인하려면 파일 이름을 인자로 넣어 다음과 같이 실행한다.

`./showdata X2.dat`

- showdata로 확인하면 A2.dat와 B2.dat를 입력으로 받아 구한 연립방정식의 해 X2.dat는 같이 제공되는 파일 X2a.dat와 같다. 하지만, 다음과 같이 Linux의 diff로 동일 내용인지 체크하면 다르다고 나온다.

`diff X.dat Xa.dat`

`Binary files X2.dat and X2a.dat differ`

- 이는 실수 연산 (floating point operation)의 round-off로 인해 발생하는 아주 작은 값의 오차 때문이다. 이 문제 때문에 결과 파일과 정답을 비교하기 위해 diff 대신 hwdiff를 다음과 같이 사용한다. diff와 마찬가지로 hwdiff는 두 파일의 내용이 거의 같을 때 아무런 메시지를 출력하지 않으며 쉘에 0을 반환한다. 여기서 세 번째 인자로 어느 정도까지의 오차를 인정하는지 입력한다. 예를 들어, 두 값의 차이가 0.0001보다 작다고 인정할 경우에는 다음과 같이 실행한다.

`./hwdiff X2.dat X2a.dat 0.0001`

- diff와 마찬가지로 hwdiff는 실행결과 아무런 메시지가 출력되지 않으면 두 파일이 같다는 것을 의미한다.

### Setup command
N/A

### Run command to test program
- GitHub에서 과제가 제대로 수행되는지 테스트를 할 때는 아래의 명령을 사용한다. 과제를 최종 제출하기 전에 자신의 로컬 리눅스 컴퓨터에서 이 명령을 실행하여 제대로 수행되는지 확인할 수 있다. 이를 위해서 먼저 이 repository의 파일들을 모두 다운로드 받아 다음을 순서대로 실행하면 된다.

`make test ARG=1`

`make test ARG=2`

`make test ARG=3`

`make test ARG=4`

`make test ARG=5`

`make test ARG=6`

`make test ARG=7`

`make test ARG=8`

### Notes
- `gcc` can be used to compile and link C applications for use with existing test harnesses or C testing frameworks.
