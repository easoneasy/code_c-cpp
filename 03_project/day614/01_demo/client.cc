#include "example.pb.h"
#include "example.srpc.h"
#include "workflow/WFFacilities.h"
#include <csignal>
#include <srpc/rpc_context.h>

using namespace srpc;
using namespace std;

static WFFacilities::WaitGroup wait_group(1);

void sig_handler(int signo)
{
	wait_group.done();
}

static void echo_done(EchoResponse *response, srpc::RPCContext *context)
{
}

int main()
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	// 注册信号处理函数
	signal(SIGINT,sig_handler);
	// 创建srpc客户端
	const char *ip = "127.0.0.1";
	unsigned short port = 1314;
	Example::SRPCClient client(ip,port);
	// 发送rpc请求
	EchoRequest req;
	req.set_message("hello,srpc");
	req.set_name("Jo");

	// 同步调用
	EchoResponse resp;
	RPCSyncContext ctx;
	client.Echo(&req,&resp,&ctx);

	if(!ctx.success)
	{
	    // 发送失败
		cerr << "error code :" << ctx.error
		<< " , error msg :" << ctx.errmsg << endl;
	}else{
	    cout << "EchoResponse { message = " << resp.message() << " }" << endl;
	}
	cout << "after client.Echo()" << endl;
	// 主线程等待
	wait_group.wait();
	google::protobuf::ShutdownProtobufLibrary();
	return 0;
}
