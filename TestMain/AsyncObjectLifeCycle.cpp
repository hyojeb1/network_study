#include <iostream>
#include <string>
#include <functional>

std::function<void()> g_callback;


class Foo : public std::enable_shared_from_this<Foo>
{
public:
    Foo()
    {
        std::cout << "Foo created\n";

        m_str = "Hello, World!";
    }

    ~Foo()
    {
        std::cout << "Foo destroyed\n";
    }

    void Register()
    {
        auto self = shared_from_this();
        std::cout << "1. " << self.use_count() <<"\n";
        g_callback = [this, self]()
            {
                DoSomething();
            };
    }

    void DoSomething()
    {
        std::cout << "Doing something\n";

        std::cout << m_str << "\n";
    }


private:

    std::string m_str;
};

int main()
{
    {
        auto f = std::make_shared<Foo>();
        f->Register();
        std::cout << "2. " << f.use_count() << "\n";
    }

    std::cout << "Calling callback...\n";


    g_callback();

    return 0;
}



//
//#include <iostream>
//#include <string>
//#include <functional>
//#include <memory> // 스마트 포인터 필수 헤더
//
//// 전역 콜백 저장소
//std::function<void()> g_callback;
//
//class Foo : public std::enable_shared_from_this<Foo>
//{
//public:
//    Foo()
//    {
//        std::cout << "[Foo] 생성됨 (Address: " << this << ")\n";
//        m_str = "Hello, World!";
//    }
//
//    ~Foo()
//    {
//        std::cout << "[Foo] 소멸됨 (Address: " << this << ")\n";
//    }
//
//    void Register()
//    {
//        // [Idiom 핵심 1] weak_ptr 생성
//        // shared_from_this()로 현재 객체의 관리 블록을 공유하는 weak_ptr을 만듭니다.
//        // 이 시점에서는 참조 카운트(Strong Ref Count)가 증가하지 않습니다.
//        std::weak_ptr<Foo> weak_self = shared_from_this();
//
//        std::cout << "   -> Register 내부: ref count = " << weak_self.use_count() << "\n";
//
//        // [Idiom 핵심 2] 람다에 weak_ptr을 값으로 캡처
//        // 'this' 포인터를 직접 캡처하지 않고 weak_ptr만 가져갑니다.
//        g_callback = [weak_self]()
//            {
//                // [Idiom 핵심 3] 사용 시점에 lock()으로 유효성 검사
//                // 객체가 살아있다면 shared_ptr을 반환하고 카운트 +1, 죽었다면 nullptr 반환
//                if (auto strong_self = weak_self.lock())
//                {
//                    std::cout << "   -> Callback 실행 중 (객체 생존): ref count = " << strong_self.use_count() << "\n";
//                    strong_self->DoSomething();
//                }
//                else
//                {
//                    std::cout << "   -> Callback 실행 중 (객체 소멸됨): 접근 불가\n";
//                }
//            };
//    }
//
//    void DoSomething()
//    {
//        std::cout << "   -> DoSomething 실행: " << m_str << "\n";
//    }
//
//private:
//    std::string m_str;
//};
//
//int main()
//{
//    std::cout << "--- 1. Scope 시작 ---\n";
//    {
//        // shared_ptr 생성 (Count: 1)
//        auto f = std::make_shared<Foo>();
//        std::cout << "   -> 생성 직후: ref count = " << f.use_count() << "\n";
//
//        // 콜백 등록 (Weak Ptr 캡처이므로 Count는 여전히 1)
//        f->Register();
//        std::cout << "   -> 등록 직후: ref count = " << f.use_count() << "\n";
//    }
//    // Scope 종료 -> f 소멸 -> Count 0 -> 객체 메모리 해제
//    std::cout << "--- 2. Scope 종료 (f 변수 소멸) ---\n\n";
//
//    std::cout << "--- 3. Callback 호출 시도 ---\n";
//    // 이미 객체는 소멸되었으므로, weak_ptr.lock()은 실패해야 함
//    if (g_callback)
//    {
//        g_callback();
//    }
//
//    return 0;
//}