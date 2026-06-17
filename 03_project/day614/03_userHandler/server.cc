#include "example.srpc.h"
#include "CryptoUtil.h"
#include "workflow/WFFacilities.h"
#include <csignal>
#include <iostream>
#include <srpc/rpc_define.h>
#include <workflow/MySQLMessage.h>
#include <workflow/MySQLResult.h>
#include <workflow/WFTask.h>
#include <workflow/WFTaskFactory.h>
#include <workflow/Workflow.h>
#include <workflow/mysql_types.h>

using namespace srpc;
using namespace std;
using namespace protocol;

static const string DatabaseURL = "mysql://root:123456@localhost/CloudDisk";
static int retry_max = 3;
static WFFacilities::WaitGroup wait_group(1);

void sig_handler(int signo)
{
    wait_group.done();
}


class UserserviceService : public Userservice::Service
{
    public:

        void Register(RegisterRequest *request, RegisterResponse *response, srpc::RPCContext *ctx) override
        {
            // 解析请求
            string username = request->username();
            string password = request->password();
            // 处理业务逻辑
            // 创建MySQL任务
            WFMySQLTask *mysqlTask = WFTaskFactory::create_mysql_task(DatabaseURL,retry_max,
            [response,username](WFMySQLTask *task)
            {
                // 判断任务是否成功
                int state = task->get_state();
                if(state != WFT_STATE_SUCCESS)
                {
                    // mysql任务失败，返回错误代码500
                    response->set_code(500);
                    cerr << WFGlobal::get_error_string(state, task->get_error()) << endl;
                    return ;
                }
                // 判断返回包的类型 检查SQL是否执行失败
                MySQLResponse *resp = task->get_resp();
                if(resp->get_packet_type() == MYSQL_PACKET_ERROR)
                {
                    response->set_code(409);

                    cerr << "error_code: " << resp->get_error_code()
                            << ", error_msg: " << resp->get_error_msg() << endl;
                    return;
                }
                // 处理结果集
                MySQLResultCursor cursor(resp);
                if(cursor.get_cursor_status() == MYSQL_STATUS_OK && cursor.get_affected_rows() == 1)
                {
                    // 处理成功
                    int user_id = cursor.get_insert_id(); // 获取插入记录的id
                    // 返回给client
                    response->set_user_id(user_id);
                    response->set_username(username);
                    response->set_code(0);
                    cout << "[server] : MySQL success!" << endl;
                }
            });
            // 设置任务：指定SQL语句
            MySQLRequest *req = mysqlTask->get_req();
            // 随机生成盐值
            string salt = CryptoUtil::generate_salt();
            // 将盐值与密码哈希
            string hashcode = CryptoUtil::hash_password(password, salt);
            // 存入数据库，写一个sql语句
            string sql = "insert into tbl_user (username,password,salt) values ('"
                + username +"','"
                + hashcode + "','"
                + salt +"');";
            req->set_query(sql);
            // 添加到任务序列中
            // 通过上下文获取处理RPC请求的任务序列
            SeriesWork *series = ctx->get_series();
            series->push_back(mysqlTask);
        }

        void Login(LoginRequest *request, LoginResponse *response, srpc::RPCContext *ctx) override
        {
            // TODO: fill server logic here
        }

        void UserInfo(UserInfoRequest *request, UserInfoResponse *response, srpc::RPCContext *ctx) override
        {
            // TODO: fill server logic here
        }
};

int main()
{
    signal(SIGINT, sig_handler);
    srand(time(NULL)); // 设置随机种子

    GOOGLE_PROTOBUF_VERIFY_VERSION;
    // 注册信号处理函数
    signal(SIGINT, sig_handler);
    // 使用默认参数，创建srpc服务器
    SRPCServer server;
    // 注册服务
    UserserviceService service;
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
