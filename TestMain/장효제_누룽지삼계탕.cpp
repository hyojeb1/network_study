/*
* File: 장효제_누룽지삼계탕.cpp
* 
* [1] 한번만 받게 하고 싶지 않다.
* while문이 꼭 필요할까?
* 일단 지금 내가 가장 잘 이해하고 있는 구조(인풋시스템)로는 바꿨는데... 그다음을 모르겠네;;;
*
* [2] 쓰레드적 사고를 한번 해볼까?
* 나는 한명의 매니저, 한명의 요리사가 있다고 가정함
* 매니저 쓰레드: 인풋 받음
* 요리사 쓰레드: 요리함 
* 쓰레드 중에서 가장 모던한 Fucture 클래스 이용 -> 모던은 참 쉽다!
* 
* [3] 약점이 좀 있음
* CPU가 불타오른다! (Busy Waiting 문제) :CookLoop를 보면 루프 안에 Sleep이 없어.
* 데이터 동기화의 헛점 (g_menu는 안전할까?)
*/

// -----------------------------
// 인클루드
// -----------------------------

#include <stdio.h>
#include <iostream>
#include <Windows.h>
#include <mutex>
#include <thread>
#include <future>


// -----------------------------
// 인풋
// -----------------------------

enum VK_CHAR
{
    VK_0 = 0x30,
    VK_1, VK_2, VK_3, VK_4, VK_5, VK_6, VK_7, VK_8, VK_9,

    VK_A = 0x41,
    VK_B, VK_C, VK_D, VK_E, VK_F, VK_G, VK_H, VK_I, VK_J, VK_K, VK_L, VK_M, VK_N,
    VK_O, VK_P, VK_Q, VK_R, VK_S, VK_T, VK_U, VK_V, VK_W, VK_X, VK_Y, VK_Z,
};

#define KEY_UP(key)			((GetAsyncKeyState(key)&0x0001))	
#define KEY_DOWN(key)		((GetAsyncKeyState(key)&0x8000))	


// -----------------------------
// 상태 
// -----------------------------
std::atomic<bool> g_cooked = false;  
std::atomic<bool> g_esc = true;

//#include "magic_enum.hpp" //매직 이넘이 없기 때문에 아래의 컨버젼 함수 존재
enum Menu
{
    k누룽지삼계탕,
    k뼈해장국,
    k설렁탕,
};
void (*g_callback)(Menu menu) = 0;
Menu g_menu;
std::string MenuToString(Menu menu)
{
    switch (menu)
    {
    case k누룽지삼계탕: return "누룽지삼계탕";
    case k뼈해장국: return "뼈해장국";
    case k설렁탕: return "설렁탕";
    default:  return "Unknown";
    }
}


// -----------------------------
// 삐삐 
// -----------------------------
void Beeper(Menu menu)
{
    //printf("삐삐 울림! %s 완성\n", MenuToString(menu));
    std::cout << "삐삐 울림! " << MenuToString(menu) << " 완성\n";
}

// -----------------------------
// 주문 
// -----------------------------
void Order(void (*cb)(Menu))
{
    g_callback = cb;
    g_cooked = false;
}

// -----------------------------
// 매장 주인 
// -----------------------------
void Manager_Update(void)
{
    if (KEY_UP(VK_ESCAPE))
    {
        std::cout << "오늘 문 닫습니다." << std::endl;
        g_esc = false;
    }

    if (KEY_UP(VK_1))
    {
        g_cooked = true;
        g_menu = k누룽지삼계탕;
    }

    if (KEY_UP(VK_2))
    {
        g_cooked = true;
        g_menu = k뼈해장국;
    }

    if (KEY_UP(VK_3))
    {
        g_cooked = true;
        g_menu = k설렁탕;
    }
}

// -----------------------------
// 계속 도는 구조 (Cook's Loop)
// -----------------------------
void CookLoop(void)
{


    while (g_esc)
    {
        // 매장 주인이 상태를 관리
        //Manager_Update();
        //... ManagerLoop()로 이동...

        // 요리가 끝났으면 삐삐 울림
        if (g_cooked && g_callback)
        {
            g_callback(g_menu);
            g_cooked = false;
        }
    }
}

// -----------------------------
// 계속 도는 구조 (Manager's Loop)
// -----------------------------
void ManagerLoop(void)
{

    while (g_esc)
    {
        // 매장 주인이 상태를 관리
        Manager_Update();

        //// 요리가 끝났으면 삐삐 울림
        //if (g_cooked && g_callback)
        //{
        //    g_callback(g_menu);
        //    g_cooked = false;
        //}
        // ... 

        Sleep(500);
        printf("다른 일 하는 중...\n");
    }
}




// -----------------------------
// Demo
// -----------------------------
int main(void)
{
    printf("손님 : 주문함\n");
    Order(Beeper);

    std::future<void> Cook = std::async(std::launch::async, CookLoop);
    std::future<void> Manager = std::async(std::launch::async, ManagerLoop);

    Cook.get();
    Manager.get();


    printf("손님 : 식사 시작\n");
    return 0;
}


///장효제_누룽지삼계탕.cpp의 끝