#include "example.srpc.h"
#include "workflow/WFFacilities.h"
#include <csignal>
#include <cstddef>
#include <srpc/rpc_define.h>
#include <wfrest/HttpMsg.h>
#include <workflow/HttpMessage.h>
#include <workflow/WFGlobal.h>
#include <workflow/WFTask.h>
#include <workflow/WFTaskFactory.h>
#include <workflow/Workflow.h>

using namespace srpc;
using namespace std;
using namespace protocol;

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
		// 解析请求，打印请求信息
		cout << "EchoRequest{ message=" << request->message()
             << ", name=" << request->name() << " }" << endl;
        // 处理业务逻辑（获取sogou首页）
        WFHttpTask *task = WFTaskFactory::create_http_task
            ("http://www.sogou.com",3,3,[response](WFHttpTask *task)
                {
                    int state = task->get_state();
                    if(state != WFT_STATE_SUCCESS)
                    {
                        response->set_message(string("Error:")+WFGlobal::get_error_string(state,task->get_error()));
                        return;
                    }
                    // http任务成功
                    const void *body;
                    size_t size;
                    HttpResponse *resp = task->get_resp();
                    resp->get_parsed_body(&body, &size);
                    // 给客户端返回sougou首页信息
                    response->set_message((const char*)body);
                });
        // 通过上下文获取处理RPC请求的任务序列
        SeriesWork *series = ctx->get_series();
        series->push_back(task);
	}
};

int main()
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	// 注册信号处理函数
	signal(SIGINT,sig_handler);
	// 使用默认参数，创建SRPC服务器
	SRPCServer server;
	// 注册服务
	ExampleService service;
	server.add_service(&service);

	// 启动SRPCServer
	if(server.start(1314) == 0)
	{
	    // 成功
		wait_group.wait();
		server.stop();
	}else {
	    cerr << "Error : Server Start Failed" << endl;
}

	google::protobuf::ShutdownProtobufLibrary();
	return 0;
}
