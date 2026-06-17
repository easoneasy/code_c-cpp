#include "example.pb.h"
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

// 回调函数，收到RPC响应之后才会执行
static void echo_callback(EchoResponse *response, srpc::RPCContext *context)
{
    // 判断RPC请求是否成功
    if(!context->success())
    {
        cerr << "error code : " << context->get_error()
            << "error msg : " << context->get_errmsg() << endl;
    }
    // 打印响应
    cout << "EchoResponse { message = " << response->message() << " }" << endl;
}

int main()
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	// 注册信号处理函数
	signal(SIGINT,sig_handler);

	// 创建srpc客户端
	// srpc服务器的ip地址
	const char *ip = "127.0.0.1";
	unsigned short port = 1314;
	Example::SRPCClient client(ip,port);
	// 创建SRPCClient任务
	SRPCClientTask *task = client.create_Echo_task(echo_callback);
	// 设置请求
	EchoRequest req;
	req.set_message("Hello , srpc");
	req.set_name("Jo");

	task->serialize_input(&req);
	// 启动任务
	task->start();
	// 主线程等待
	wait_group.wait();
	google::protobuf::ShutdownProtobufLibrary();
	return 0;
}
