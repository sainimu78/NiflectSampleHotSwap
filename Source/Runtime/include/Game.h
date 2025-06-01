#pragma once
#include "Actor.h"
#include "Niflect/Base/Array.h"
#include <atomic>
#include <thread>
#include <chrono>

class CGame
{
public:
    CGame();

public:
    void AddActor(const CSharedActor& actor)
    {
        m_vecActor.push_back(actor);
    }

public:
    RUNTIME_API void Start();
    RUNTIME_API void Stop();
    RUNTIME_API void Tick(double deltaTime);

private:
    void RunLoop();

private:
    Niflect::TArray<CSharedActor> m_vecActor;
    const std::chrono::microseconds m_tickInterval;
    std::atomic<bool> m_isRunning;
    std::thread m_thread;
};
using CSharedGame = Niflect::TSharedPtr<CGame>;

RUNTIME_API CSharedGame CreateGame();