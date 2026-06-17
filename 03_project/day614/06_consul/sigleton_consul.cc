#include <chrono>
#include <iostream>
#include <ppconsul/consul.h>
#include <ppconsul/agent.h>
#include <ppconsul/health.h>
#include <string>
#include <workflow/WFTask.h>
#include <workflow/WFTaskFactory.h>
#include <workflow/Workflow.h>

using namespace std;
using ppconsul::Consul;
using namespace ppconsul::agent;
using ppconsul::health::Health;
using namespace ppconsul::kw;

class ConsulManager
{
public:
    // 获取单例
    static ConsulManager &getInstance()
    {
        static ConsulManager instance;
        return instance;
    }
    // 注册，把微服务的信息记录到Consul注册中心里
    bool registerService(string id,string name,string ip,int port)
    {
        // 发送HTTP请求给Consul
        // 采用TTL健康检测机制，生存期为10秒
        // 如果10秒之内服务没有主动向Consul报平安，Consul就会认为该服务挂了，标记为不健康
        _agent.registerService(
            kw::id = id,
            kw::name = name,
            kw::address = ip,
            kw::port = port,
            kw::check = TtlCheck{std::chrono::seconds(10)}
        );
        return true;
    }
    // 心跳续期
    // 显示告诉Consul健康
    void servicePass(string service_id)
    {
        // 刷新Consul对应的service_id的TTL计时器，重新倒计时
        _agent.servicePass(service_id);
    }
    // 根据服务名查找ip+port
    string discoverService(string service_name)
    {
        // 向Consul获取该服务名下的所有通过了健康检测的实例列表
        // services是个容器
        auto services = _health.service(service_name);
        if(services.empty())
        {
            return "";
        }
        // 拿第一个实例
        auto &serviceInfo = std::get<1>(services.front());
        // 轮询负载均衡版（但没看懂）
        // size_t index = request_count.fetch_add(1) % services.size();
        // auto &serviceInfo = std::get<1>(services[index]);
        // 返回address:port
        return serviceInfo.address+":"+std::to_string(serviceInfo.port);
    }
    // 注销，当服务主动关闭时，通知Consul把自己从服务列表中删除
    void deregisterService(const string &service_id)
    {
        // 移除该节点
        _agent.deregisterService(service_id);
    }
    // 自动检测的回调函数
    static void timer_callback(WFTimerTask *task,string service_id)
    {
        if(task->get_state() != WFT_STATE_SUCCESS)
        {
            return;
        }
        // 向Consul报平安
        ConsulManager::getInstance().servicePass(service_id);
        WFTimerTask *next = WFTaskFactory::create_timer_task(
                "health-check",9,0,
                std::bind(timer_callback,std::placeholders::_1,service_id)
            );
        // 将定时器放在当前序列中
        series_of(task)->push_back(next);
    }
    // 自动检测
    void startHeartBeat(const string &service_id)
    {
        // 防止刚注册完还没到第一次定时器触发就超时
        servicePass(service_id);
        WFTimerTask *task = WFTaskFactory::create_timer_task(
            "health-check",9,0,
            std::bind(timer_callback,std::placeholders::_1,service_id)
        );
        task->start();
    }
private:
    // 构造函数
    ConsulManager()
    :_consul("http://127.0.0.1:8500",dc="dc1")
    ,_agent(_consul)
    ,_health(_consul)
    {
    }
    // 析构函数
    ~ConsulManager(){}
    // 删除复制类型函数
    ConsulManager(const ConsulManager&) = delete;
    ConsulManager &operator=(const ConsulManager&) = delete;
private:
    Consul _consul;
    Agent _agent;
    Health _health;
};

int main(int argc,char *argv[])
{
    ConsulManager::getInstance().registerService(
        "main-service-1", "main-service", "192.168.254.128", 6061);

    ConsulManager::getInstance().startHeartBeat("main-service-1");
    return 0;
}
