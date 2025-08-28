# DevLib 🚀

**C++ 개발의 편의와 생산성을 위한 범용 클래스 라이브러리**


  **DevLib**는 반복적인 코드를 표준화하고 생산성을 높이기 위해 개발한 C++ 기반 범용 클래스 라이브러리입니다
 파일 입출력, 네트워크, 스레드, 이미지/영상 처리, 그래픽 등 다양한 기능들을 모듈화하여 Windows와 Linux 환경에서 동일한 인터페이스로 활용할 수 있도록 설계되었습니다.

## ✨ 주요 특징

* **크로스플랫폼 지원**: Windows와 Linux 환경에서 동일한 API를 제공하여 코드 재사용성을 극대화합니다.
* **객체지향(OOP) 설계**: 모든 기능은 독립적인 클래스 형태로 제공되어 유지보수가 용이합니다.
* **모듈화 구조**: 기능별로 디렉토리를 분리한 모듈화 설계를 통해 필요한 부분만 가져다 쓰기 편리합니다.
* **복잡성 추상화**: 소켓 프로그래밍, FFmpeg/Live555 라이브러리, 스레드 관리 등 복잡한 저수준(low-level) API를 직관적인 C++ 클래스로 래핑하여 사용하기 쉽게 만들었습니다.

## 📂 아키텍처 및 모듈

DevLib는 기능별로 다음과 같은 모듈로 구성되어 있습니다.

* `/Base`: 스레드, 타이머, 이벤트, 동기화 등 필수 구성요소
* `/GL`: OpenGL 버텍스, 텍스처 등 3D 렌더링 지원
* `/GUI`: 크로스플랫폼 윈도우 및 이벤트 처리
* `/Graphics`: 이미지 로딩, 변환, 필터링 
* `/IO`: 파일 입출력, TCP/UDP 소켓, 시리얼 통신 
* `/MultiMedia`: FFmpeg 기반 인코딩/디코딩, RTSP 스트리밍, USB 카메라 지원 
* `/Routine`: 타일맵, 메시지 통신 등 확장 도구
* `/Utility`: 설정 파일 파싱, 문자열 처리 등 
* `/Visualization`: 데이터 2D/3D 가시화 도구 

## 🛠️ 시작하기

### 요구사항

* C++ 17 컴파일러.
* CMake

### 빌드 방법

```bash
# 1. 저장소 복제
git clone [https://github.com/HyunhuePark/DevLib.git](https://github.com/HyunhuePark/DevLib.git)
cd DevLib

# 2. 빌드 디렉토리 생성 및 빌드
mkdir build
cd build
cmake ..
make
```

## 💡 사용 예제

DevLib를 사용하면 복잡한 작업을 얼마나 간단하게 처리할 수 있는지 보여주는 몇 가지 예제입니다.

### 1. Callback

`std::function` 바인딩의 복잡함을 `EnableCallback` 매크로 한 줄로 추상화하여 코드 가독성과 생산성을 크게 향상시켰습니다.

```cpp
#include <Base/DevLibCallback.hpp>

// 콜백으로 호출될 함수들
void OnCallback0() { /* ... */ }
void OnCallback1(const std::string& str) { /* ... */ }

class CCallbackTest
{
public:
    void Call0() const { if(m_callbackCallaback0) m_callbackCallaback0(); }
    void Call1() const { if(m_callbackCallaback1) m_callbackCallaback1("CCallbackTest::Call1"); }

private:
    // 콜백 인터페이스를 매크로 한 줄로 추가
    EnableCallback(Callaback0);
    EnableCallback(Callaback1, const std::string&);
};

int main()
{
    CCallbackTest test;

    // 콜백 함수 등록
    test.RegisterCallbackCallaback0(OnCallback0);
    test.RegisterCallbackCallaback1(OnCallback1);

    // 함수 호출
    test.Call0();
    test.Call1();
}
```

### 2. Thread & ThreadPool

`CThread`는 스레드의 생명주기를 안전하게 관리하며, `CThreadPool`은 작업 큐 기반으로 스레드를 재사용하여 불필요한 오버헤드를 제거합니다.

```cpp
#include <Base/CThreadPool.hpp>
#include <string>

// 스레드에서 실행될 작업 함수
void WorkThread(const std::string& threadName)
{
    // ... 작업 수행 ...
    DevLib::CThread::SleepMicrosecond(10000);
}

int main()
{
    DevLib::CThreadPool pool;
    pool.Create(10); // 10개의 스레드 풀 생성

    for(int i = 0; i < 100; ++i)
    {
        std::string name = "Thread " + std::to_string(i);
        pool.RunThread(WorkThread, name); // 작업 큐에 작업 추가
    }
}
```

### 3. Socket (TCP Echo Server)

`bind`, `listen`, `accept` 등 복잡한 소켓 프로그래밍 과정을 캡슐화하고, 개발자는 이벤트 콜백에만 집중할 수 있도록 설계되었습니다.

```cpp
#include <IO/CTcpServer.hpp>

// 데이터 수신 시 호출될 콜백 (echo)
void OnData(const DevLib::IO::CSocket& sock, const std::string& ip, uint16_t port, void* pData, int32_t size)
{
    sock.Send(pData, size);
}

int main()
{
    DevLib::IO::CTcpServer server;

    // 콜백 등록
    server.RegisterCallbackReceiveData(OnData);

    // 9926 포트에서 서버 시작
    server.CreateTcpServer(9926);

    while (true)
    {
        DevLib::CThread::SleepMillisecond(1000);
    }
}
```

### 4. MultiMedia (RTSP Server)

FFmpeg와 Live555의 복잡한 API를 래핑하여, 단 몇 줄의 코드로 H.265/264 실시간 스트리밍 서버를 구축할 수 있습니다.

```cpp
#include <MultiMedia/CVideoStreamer.hpp>
#include <Graphics/CImage.hpp>

int main()
{
    // RTSP 서버 생성
    DevLib::MultiMedia::CVideoStreamer streamer("Example RTSP Server");
    streamer.CreateStreamer(60000, 1920, 1080, 4096, 30, 15, DevLib::MultiMedia::CodecType::H265_NVENC);

    DevLib::Graphics::CImage image;
    image.Create(1920, 1080); // 전송할 이미지 생성

    while (true)
    {
        // ... 이미지 내용 업데이트 ...
        streamer.StreamImage(image); // 이미지 전송
        DevLib::CThread::SleepMillisecond(30);
    }
}
```

## 📜 라이선스

이 프로젝트는 [[LICENSE](include/3rd/LICENSE)] 따릅니다.

## 📧 연락처

박현휴 – parkhyunhyu@gmail.com
