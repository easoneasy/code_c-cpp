#include "example.srpc.h"
#include "workflow/WFFacilities.h"
#include <csignal>

using namespace std;
using namespace srpc;

static WFFacilities::WaitGroup wait_group(1);

void sig_handler(int signo)
{
	wait_group.done();
}

class ExampleServiceImpl : public Example::Service
{
public:
    // 实现远程 过程调用Echo
	void Echo(EchoRequest *request, EchoResponse *response, srpc::RPCContext *ctx) override
	{
	    // 解析请求，仅打印请求信息
		cout << "EchoRequest { message=" << request->message()
		<< ",name = " << request->name() << " }" << endl;
		// 处理业务逻辑
		sleep(3);
		// 生成响应
		response->set_message("Hi" + request->name());
	}
};

int main()
{
    // 注册信号处理函数
    signal(SIGINT,sig_handler);
	// 使用默认参数创建srpc服务器
	SRPCServer server;
	// 注册服务
	// 一个srpcserver可以注册多个service，一个service可以包含多个rpc
	ExampleServiceImpl service;
	server.add_service(&service);

	// 启动SRPCServer
	if(server.start(1314) == 0)
	{
        wait_group.wait();
        server.stop();
	}else {
	    cerr << "Error : Server Start Failed" << endl;
	}

	google::protobuf::ShutdownProtobufLibrary();
	return 0;
}
