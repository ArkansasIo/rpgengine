/* ArkLight Beyond - Game Time
 * Developer: Stephen
 * Time management with pause, speed control, and date tracking.
 */
#pragma once
#include <cstdint>
#include <string>

namespace ogb {
struct GameDate {
    int year = 2200;
    int month = 1;
    int day = 1;
    int hour = 0;

    void AdvanceDays(int days) {
        day += days;
        while (day > 30) { day -= 30; month++; }
        while (month > 12) { month -= 12; year++; }
    }

    std::string ToString() const {
        char buf[32];
        snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:00", year, month, day, hour);
        return buf;
    }
};

class GameTime {
public:
    float timeScale = 1.0f;
    bool paused = false;
    Tick currentTick = 0;
    GameDate date;

    void Update(float realDt) {
        if (paused) return;
        float gameDt = realDt * timeScale;
        accumulated += gameDt;
        while (accumulated >= DT) {
            accumulated -= DT;
            currentTick++;
            date.AdvanceDays(0); // tick-based advancement
        }
    }

    void SetSpeed(float s) { timeScale = s; }
    void TogglePause() { paused = !paused; }
    float GetTimeScale() const { return timeScale; }
    bool IsPaused() const { return paused; }

    // Time of day (0-24)
    float GetTimeOfDay() const { return (currentTick % TICKS_PER_SECOND) / (float)TICKS_PER_SECOND * 24.0f; }

private:
    float accumulated = 0;
};
} // namespace ogb
