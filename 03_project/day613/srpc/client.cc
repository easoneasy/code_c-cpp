#include "example.srpc.h"
#include "workflow/WFFacilities.h"
#include <csignal>
#include <iostream>
using namespace std;
using namespace srpc;

static WFFacilities::WaitGroup wait_group(1);

void sig_handler(int signo)
{
	wait_group.done();
}

// 回调函数：收到RPC响应后才会执行
static void echo_done(EchoResponse *response, srpc::RPCContext *context)
{
    // 判断RPC请求是否成功
    if(!context->success())
    {
        cerr << "error code: " << context->get_error()
            << "error message: " << context->get_errmsg() << endl;
    }
    // 打印响应
    cout << "EchoResponse {message = " << response->message() << " }" << endl;
}

int main()
{
	// 注册信号处理函数
	signal(SIGINT,sig_handler);
	// 创建srpc客户端
	// srpc服务器的ip地址
	const char *ip = "127.0.0.1";
	// srpc服务器监听的端口
	unsigned short port = 1314;

	Example::SRPCClient client(ip, port);
	// 发送rpc请求

	EchoRequest req;
	req.set_message("Hello,srpc");
	req.set_name("Jo");
	// 像本地方法一样调用远程方法
	client.Echo(&req, echo_done);

	cout << "after client.Echo" << endl;
	// 主线程等待
	wait_group.wait();
	google::protobuf::ShutdownProtobufLibrary();
	return 0;
}
