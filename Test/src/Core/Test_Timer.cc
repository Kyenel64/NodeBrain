#include <gtest/gtest.h>
#include <NodeBrain/NodeBrain.h>

TEST(Timer, GetElapsedMicroseconds) 
{
    NodeBrain::Timer timer;
    timer.EndTimer();
    EXPECT_EQ(timer.GetElapsedMicroseconds(), timer.GetElapsedMilliseconds() / 1000.0f);
    EXPECT_EQ(timer.GetElapsedMicroseconds(), timer.GetElapsedSeconds() / 1000000);
}