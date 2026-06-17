#include "example.srpc.h"
#include "workflow/WFFacilities.h"
#include <csignal>
#include <srpc/rpc_define.h>

using namespace srpc;
using namespace std;

static WFFacilities::WaitGroup wait_group(1);

void sig_handler(int signo)
{
    wait_group.done();
}

class ExampleService : public Example::Service
{
    public:
        // 实现远程调用Echo
        void Echo(EchoRequest *request, EchoResponse *response, srpc::RPCContext *ctx) override
        {
            // 解析请求
            cout << "EchoRequest { message = " << request->message()
            << " , name = " << request->name() << " }" << endl;
            // 处理业务逻辑
            sleep(3);
            // 生成响应
            response->set_message("Hi"+request->name());
        }
};

int main()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    // 注册信号处理函数
    signal(SIGINT,sig_handler);

    // 使用默认参数创建srpc服务器
    SRPCServer server;
    // 注册服务
    // 一个SRPCServer可以注册多个Service
    ExampleService service;
    server.add_service(&service);

    // 启动SRPCServer
    if(server.start(1314) == 0)
    {
        wait_group.wait();
        server.stop();
    }else{
        cerr << "Error : Server Start Failed" << endl;
    }

    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
