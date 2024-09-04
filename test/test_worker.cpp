#include <gtest/gtest.h>
#include <iostream>
#include <utility>

#include "common.h"
#include "Worker.h"

class SleepyWorker: public Worker{
public:
    explicit SleepyWorker(std::string name): Worker(std::move(name)){}
    void Execute() override{sleep(1);}
};

TEST(Worker, ImmediateStop){
    SetupLogger("Worker_ImmediateStop");
    Worker w("Worker_ImmediateStop_t");

    w.Start();
    ASSERT_TRUE(w.Stop());
}

TEST(Worker, StartStop){
    SetupLogger("Worker_StartStop");
    Worker w("Worker_StartStop_t");

    w.Start();
    sleep(5);
    ASSERT_TRUE(w.Stop());
}

TEST(Worker, HighFreq){
    SetupLogger("Worker_HighFreq");
    Worker w("Worker_HighFreq_t", 100000);

    w.Start();
    sleep(5);
    ASSERT_TRUE(w.Stop());
}

TEST(Worker, SlowWorker){
    SetupLogger("Worker_SlowWorker");
    SleepyWorker w("Worker_SlowWorker_t");

    w.Start();
    sleep(5);
    ASSERT_TRUE(w.Stop());

}