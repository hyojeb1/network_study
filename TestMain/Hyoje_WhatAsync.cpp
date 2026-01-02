/*
* File: Hyoje_WhatAsync_Cook.cpp
*
* 요리 시스템을 통한 비동기 패턴 학습
* 시나리오: 고기 굽기
* * [비동기 흐름]
* 1. 할 일 등록 (PostTask) -> 밑간하기, 양파 썰기, 고기 굽기
* 2. 다른 작업 가능 (Non-blocking)
* 3. 조건 충족 (CheckCondition) -> 여기서는 키보드 입력(0, 1, 2)으로 완료 신호를 시뮬레이션
* 4. 콜백 실행 (ProcessCompletedTasks) -> 요리 완료 로그 출력
*/


// -----------------------------
// 인클루드
// -----------------------------
#include <iostream>
#include <Windows.h>
#include <functional>
#include <vector>
#include <algorithm>
#include <string>

// -----------------------------
// using
// -----------------------------
using std::cout;
using std::endl;
using std::string;
using std::function;
using std::vector;




// -----------------------------
//  클래스
// -----------------------------

struct Task
{
    int id;
    string name;
    function<void()> callback;
    bool done;

    static int nextId;

    Task(string n, function<void()>cb , bool bb = false): id(nextId++) , name(n), callback(cb) , done(bb)
    {
        cout << "[System] Task Registered (ID: " << id << ") : " << name << endl;
    }
    ~ Task() = default;
};

int Task::nextId = 0;


#ifndef MACRO
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

#endif // MACRO



// -----------------------------
//  전역 변수
// -----------------------------

static vector<Task> g_tasks;




// -----------------------------
//  전역 함수
// -----------------------------

static void PostTask(string name, function<void()> cb)
{
    g_tasks.push_back( Task{ name,cb} );
}

static bool CheckCondition(const Task& task)
{
    if (KEY_UP(VK_0 + task.id))
    {
        return true;
    }

    return false;
}


static void ManagerUpdate()
{
    for (auto& task : g_tasks)
    {
        if (task.done) continue;

        if (CheckCondition(task))
        {
            task.done = true;
            cout << "  -> [Event] Key Input Detected! Task " << task.id << " condition met." << endl;
        }
    }
}

static void ProcessCompletedTasks()
{
    for (auto it = g_tasks.begin(); it != g_tasks.end(); )
    {
        if (it->done)
        {
            cout << "[Callback] Executing Task " << it->id << " (" << it->name << ")..." << endl;

            it->callback();// 실행
            cout << "----------------------------------------" << endl;
            it = g_tasks.erase(it);// 할 일 제거
        }
        else
        {
            ++it;
        }
    }
}


static void RunLoop()
{
    cout << "\n[System] Loop Started. Waiting for input (Press 0, 1, 2...)..." << endl;

    while (true)
    {
        ManagerUpdate();         // 1. 상태 체크 (패킷 왔니? 요리 다 됐니?)
        ProcessCompletedTasks(); // 2. 완료된 일 처리 (채팅 띄우기, 요리 내놓기)

        if (g_tasks.empty())
        {
            cout << "[System] All tasks finished. Loop End." << endl;
            break; // 모든 작업 끝남
        }

        Sleep(10); // CPU 점유율 낮춤 (매우 중요)
    }
}

// -----------------------------
//  메인
// -----------------------------

int main()
{
    cout << "=== Cooking Async System Start ===" << endl;

    // 1. 밑간 요청 (ID 0 할당됨 -> 숫자키 0 누르면 완료)
    PostTask("Season with Salts", []() {
        cout << "   => (Result) Sprinkle~ Sprinkle~ Salt added! (Salty +10)" << endl;
        });

    // 2. 야채 썰기 요청 (ID 1 할당됨 -> 숫자키 1 누르면 완료)
    PostTask("Chop Onions", []() {
        cout << "   => (Result) Chop chop chop! Onions are ready." << endl;
        });

    // 3. 고기 굽기 요청 (ID 2 할당됨 -> 숫자키 2 누르면 완료)
    PostTask("Grill Meat", []() {
        cout << "   => (Result) Sizzle~ Steak is Medium Rare." << endl;
        });

    // 루프 실행 (이 함수는 모든 작업이 끝날 때까지 리턴하지 않음)
    RunLoop();

    cout << "=== Kitchen Closed ===" << endl;

	return 0;
}