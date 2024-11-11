#include <gtest/gtest.h>

#include "common.h"
#include "NTWorker.h"

std::function<RobotPose()> CreateCallback(const std::vector<double>& pos_vals){
    return [pos_vals]() -> RobotPose {
        RobotPose ret_val;
        ret_val.global.T = {pos_vals[0], pos_vals[1], pos_vals[2]};
        return ret_val;
    };
}

TEST(NetworkTablesWorker, NTNoServer){
    SetupLogger("NetworkTablesWorker_NTNoServer");

    NTWorker nt_worker(2987);
    nt_worker.RegisterPoseCallback(CreateCallback({1, 2, 3}));
    nt_worker.Start();

    sleep(5);

    // Verify the worker never connected to the server
    ASSERT_FALSE(nt_worker.IsConnected());


    ASSERT_TRUE(nt_worker.Stop());
    AppLogger::Logger::Flush();
}

TEST(NetworkTablesWorker, NTServer){
    SetupLogger("NetworkTablesWorker_NTServer");

    // Create server
    nt::NetworkTableInstance inst = nt::NetworkTableInstance::Create();
    inst.StartServer("../test/networktables.json", "127.0.0.1", NT_DEFAULT_PORT3, NT_DEFAULT_PORT4);
    AppLogger::Logger::Log("Started NetworkTables test server");

    // Verify that nothing has been published to the topic yet
    std::shared_ptr<nt::NetworkTable> table = inst.GetTable("AMPL");
    nt::DoubleArraySubscriber position_sub = table->GetDoubleArrayTopic("position").Subscribe({});


    // Test that the worker can publish at least one message to the NetworkTables server
    std::vector<double> desired_pos({500, -500, 200});
    NTWorker nt_worker("127.0.0.1");
    nt_worker.RegisterPoseCallback(CreateCallback(desired_pos));
    nt_worker.Start();

    sleep(5);
    // Verify the server has one connection (the one worker)
    EXPECT_TRUE(inst.GetConnections().size() == 1);
    // Verify the worker thinks it is connected to the server
    EXPECT_TRUE(nt_worker.IsConnected());
    AppLogger::Logger::Log("Server and NTWorker agree the worker is connected to NetworkTables test server");

    // Verify that a message was published to the server
    std::vector<double> actual_pos = position_sub.Get(); // get the latest value
    ASSERT_TRUE(actual_pos.size() == 3);
    for (int i=0; i < 3; i++){
        ASSERT_TRUE(actual_pos[i] == desired_pos[i]);
    }
    AppLogger::Logger::Log("Server received data from NTWorker");

    ASSERT_TRUE(nt_worker.Stop());

    AppLogger::Logger::Flush();
}