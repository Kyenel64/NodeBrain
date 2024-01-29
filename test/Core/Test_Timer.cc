#include <gtest/gtest.h>
#include <NodeBrain/NodeBrain.h>

TEST(Timer, GetElapsedMilliseconds) 
{
    NodeBrain::Timer timer;
	timer.EndTimer();
    EXPECT_GE(timer.GetElapsedMilliseconds(), 0.0f);
}