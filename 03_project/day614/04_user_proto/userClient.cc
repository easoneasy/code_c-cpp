#include "userHandler.srpc.h"
#include "userHandler.pb.h"
#include "workflow/WFFacilities.h"
#include <signal.h>

using namespace srpc;
using namespace std;

static WFFacilities::WaitGroup wait_group(1);

void sig_handler(int signo)
{
    wait_group.done();
}

static void register_callback(RegisterResponse *response, srpc::RPCContext *context)
{
    // 1. 判断RPC请求是否成功
    if (!context->success()) {
        cerr << "error code: " << context->get_error()
        << ", error msg: " << context->get_errmsg() << endl;
    }
    // 2. 打印响应
    cout << "user_id = " << response->user_id()
    << "username = " << response->username()
    << "code = " << response->code();
}

static void login_callback(LoginResponse *response, srpc::RPCContext *context)
{
    // 1. 判断RPC请求是否成功
    if (!context->success()) {
        cerr << "error code: " << context->get_error()
        << ", error msg: " << context->get_errmsg() << endl;
    }
    // 2. 打印响应
    cout << "user_id = " << response->user_id()
    << "username = " << response->username()
    << "tokenType = " << response->tokentype()
    << "token = " << response->token()
    << "code = " << response->code()
    << "msg = " << response->msg();
}

static void userinfo_done(UserInfoResponse *response, srpc::RPCContext *context)
{
}

int main()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    // 注册信号处理函数
    signal(SIGINT, sig_handler);
    // 1. 创建srpc客户端
    const char* ip = "127.0.0.1"; // srpc服务器的IP地址
    unsigned short port = 1314; // srpc服务器监听的端口
    userHandler::SRPCClient client(ip, port);
    // 2. 创建SRPCClient任务
    SRPCClientTask* task =client.create_Login_task (login_callback);
    // 设置请求
    LoginRequest req;
    req.set_username("Jo");
    req.set_password("123");
    task->serialize_input(&req);

    // 3. 启动任务（也可以和其它任务一起编排，组成串行流或并行流）
    task->start();

    cout << "after client.Echo()" << endl;
    wait_group.wait();
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
