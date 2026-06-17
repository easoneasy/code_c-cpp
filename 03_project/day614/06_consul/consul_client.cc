#include <chrono>
#include <csignal>
#include <functional>
#include <iostream>
#include <ppconsul/consul.h>
#include <sys/poll.h>
#include <wfrest/HttpMsg.h>
#include <wfrest/HttpServer.h>
#include <workflow/WFFacilities.h>
#include <ppconsul/agent.h>
#include <workflow/WFTask.h>
#include <workflow/WFTaskFactory.h>
#include <workflow/Workflow.h>

using namespace std;
using namespace wfrest;
using  ppconsul::Consul;
using namespace ppconsul::agent;

WFFacilities::WaitGroup waitGroup(1);

void sig_handler(int signo)
{
    waitGroup.done();
}

void timer_callback(WFTimerTask *task,Agent &agent)
{
    if(task->get_state() != WFT_STATE_SUCCESS)
    {
        return;
    }
    // 健康检查
    agent.servicePass("my-service-1");
    WFTimerTask *next = WFTaskFactory::create_timer_task(
        "health-check",
        9,0,
        std::bind(timer_callback,placeholders::_1,std::ref(agent))
    );
    series_of(task)->push_back(next);
}

int main(int argc,char *argv[])
{
    signal(SIGINT,sig_handler);

    HttpServer server;
    server.GET("/*",[](const HttpReq *req,HttpResp *resp)
        {
            resp->String("Hello World");
        });
    if(server.start(8888) == 0)
    {
        // 启动成功
        // 注册服务的信息
        // Consul服务运行在127.0.0.1:8500上
        Consul consul("http://127.0.0.1:8500",ppconsul::kw::dc = "dc1");
        // 拿到Consul的管理接口
        Agent agent(consul);
        // 向Consul注册 实例id、服务名、IP、端口号
        agent.registerService(
            // 服务名和id
            // Consul查询时一般使用name
            // id表示具体实例
            kw::id = "my-server-1",
            kw::name = "my-service",
            kw::address = "192.168.254.128",
            kw::port = 8888,
            // Ttl : time to live
            // 健康检查，10秒内必须汇报一次存活状态，否则服务失效
            kw::check = TtlCheck{std::chrono::seconds(10)}
        );
        // 检查健康
        // 告诉consul还活着
        agent.servicePass("my-service-1");
        // 创建定时器，定时发送心跳，9秒执行一次
        WFTimerTask *task = WFTaskFactory::create_timer_task(
                "health-check",
                9,
                0,
                std::bind(timer_callback,placeholders::_1,std::ref(agent))
            );
        task->start();

        waitGroup.wait();
        // 取消定时器任务
        WFTaskFactory::cancel_by_name("health-check");
        server.stop();
    }else{
        cerr << "Error : Server start Failed" << endl;
        exit(1);
    }

    return 0;
}
