#include "fileHandler.pb.h"
#include "fileHandler.srpc.h"
#include "workflow/WFFacilities.h"
#include <csignal>

using namespace srpc;
using namespace std;

static WFFacilities::WaitGroup wait_group(1);

void sig_handler(int signo)
{
	wait_group.done();
}

static void fileinfo_callback(fileListResponse *response, srpc::RPCContext *context)
{
    // 1. 判断RPC请求是否成功
    if (!context->success()) {
        cerr << "error code: " << context->get_error()
        << ", error msg: " << context->get_errmsg() << endl;
    }

    // 2、打印响应
    for(const auto &file : response->files())
    {
        cout << "fileId = " << file.fileid() << endl;
        cout << "filename = " << file.filename() << endl;
        cout << "filesize = " << file.filesize() << endl;
        cout << "createAt = " << file.createdat() << endl;
        cout << "updateAt = " << file.updatedat() << endl;
    }


}

int main()
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	// 注册信号处理函数
	signal(SIGINT,sig_handler);
    // 1. 创建srpc客户端
    const char* ip = "127.0.0.1"; // srpc服务器的IP地址
    unsigned short port = 8002; // srpc服务器监听的端口
    fileHandler::SRPCClient client(ip, port);
    // 2. 创建SRPCClient任务
    SRPCClientTask* task =client.create_fileInfo_task (fileinfo_callback);
    // 设置请求
    fileInfoRequest req;
    req.set_uid(1);
    task->serialize_input(&req);

    // 3. 启动任务（也可以和其它任务一起编排，组成串行流或并行流）
    task->start();

	wait_group.wait();
	google::protobuf::ShutdownProtobufLibrary();
	return 0;
}
