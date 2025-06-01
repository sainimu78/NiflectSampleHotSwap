#include "Game.h"

CGame::CGame()
    : m_tickInterval(std::chrono::milliseconds(1000))
    , m_isRunning(false)
{
}
void CGame::Start()
{
    ASSERT(!m_isRunning);
    m_isRunning = true;
    m_thread = std::thread(&CGame::RunLoop, this);
}
void CGame::Stop()
{
    m_isRunning = false;
    if (m_thread.joinable())
        m_thread.join();
}
void CGame::Tick(double deltaTime)
{
    static int g_cnt = 0;
    printf("Tick, %d\n", g_cnt++);
}
void CGame::RunLoop()
{
    using clock = std::chrono::steady_clock;
    using namespace std::chrono;
    using fsec = std::chrono::duration<double>;

    constexpr int MAX_FRAME_SKIP = 5; // 防螺旋死亡
    const auto& TICK_INTERVAL = m_tickInterval;
    const double TIME_STEP = static_cast<double>(TICK_INTERVAL.count()) / 1000000.0;

    auto previousTime = clock::now();
    auto accumulatedTime = 0us;

    while (m_isRunning)
    {
        // 1. 计算时间增量（使用double秒提高精度）
        auto currentTime = clock::now();
        auto frameTime = duration_cast<microseconds>(currentTime - previousTime);
        previousTime = currentTime;
        accumulatedTime += frameTime;

        // 2. 带保护的固定步长更新
        int framesUpdated = 0;
        while (accumulatedTime >= TICK_INTERVAL && framesUpdated < MAX_FRAME_SKIP)
        {
            this->Tick(TIME_STEP);
            accumulatedTime -= TICK_INTERVAL;
            framesUpdated++;
        }

        // 3. 计算插值因子（0.0-1.0）
        double interpolation = std::min(std::max(static_cast<double>(accumulatedTime.count()) / static_cast<double>(m_tickInterval.count()), 0.0), 1.0);

        // 4. 渲染（使用插值平滑画面）
        //this->RenderWithInterpolation(interpolation);

        // 5. 增强型帧率控制
        auto nextFrameTime = previousTime + TICK_INTERVAL - accumulatedTime;
        auto sleepDuration = nextFrameTime - clock::now();
        if (sleepDuration > 2ms)
        {
            std::this_thread::sleep_for(sleepDuration - 1ms); // 预留1ms缓冲
        }
        else if (sleepDuration > 0us)
        {
            // 精确等待最后时刻
            while (clock::now() < nextFrameTime)
                std::this_thread::yield();
        }
    }
}

CSharedGame CreateGame()
{
    return Niflect::MakeShared<CGame>();
}