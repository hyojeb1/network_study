//채팅서버.cpp 시작
#include "../StaticLib/pch.h"
#include <set>
using std::set;
using boost::asio::ip::tcp;

class Session;

class ChatRoom
{
public:
    void Join(shared_ptr<Session> session)
    {
        m_participants.insert(session);
        cout << "[Room] 유저 입장 (현재 " << m_participants.size() <<"명)" << endl;
    }

    void Leave(shared_ptr<Session> session)
    {
        m_participants.erase(session);
        cout << "[Room] 유저 퇴장 \n";
    }

    void Broadcast(const string& msg);
    

private:
    set<shared_ptr<Session>> m_participants;
};




class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(tcp::socket socket, ChatRoom& room)
        : ID(nextID++), m_Socket(std::move(socket)), m_Room(room)
    {
        std::cout << "[시스템] "<< ID << "번 세션 활성화 \n";
    }

    void Start()
    {
        m_Room.Join(shared_from_this());
        DoRead();
    }

    void Deliver(const string& msg)
    {
        auto self = shared_from_this();
        boost::asio::async_write(m_Socket, boost::asio::buffer(msg),
            [self](boost::system::error_code ec, size_t /*length*/) {
                if (ec) { /* 에러 처리 */ }
            });
    }
    
    void ClearMsg()
    {
        memset(m_Data, 0, sizeof(m_Data));
    }

private:
    void DoRead()
    {
        auto self = shared_from_this();

        m_Socket.async_read_some(
            boost::asio::buffer(m_Data),
            [this, self](boost::system::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    string msg(m_Data, length);
                    string formattedMsg = "Player" + std::to_string(ID) + " Says \"" + m_Data + "\"";
                    m_Room.Broadcast(formattedMsg);
                    //DoWrite();
                    DoRead();
                }
                else
                {
                    m_Room.Leave(shared_from_this());
                    // Client disconnected
                }
            }
        );

    }

    void DoWrite()
    {
        auto self = shared_from_this();



        string res = "Player" + std::to_string(ID) + " Says \"" + m_Data + "\"";



        boost::asio::async_write(
            m_Socket,
            boost::asio::buffer(res.c_str(), /*length*/ res.length()),
            [this, self](boost::system::error_code ec, std::size_t /*len*/)
            {
                if (!ec)
                {
                    DoRead(); // 계속 읽기
                }
            }
        );
    }

    tcp::socket m_Socket;
    ChatRoom& m_Room;
    char m_Data[1024]; // 나는 이제 이걸 임시 버퍼로 이용할꺼임
    int ID;
    static int nextID;
};
int Session::nextID = 0;


void ChatRoom::Broadcast(const string& msg) 
{
    for (auto& participant : m_participants)
    {
        participant->Deliver(msg);
        participant->ClearMsg();
    }
}

class Server
{
public:
    Server(boost::asio::io_context& io, uint16_t port)
        : m_Acceptor(io, tcp::endpoint(tcp::v4(), port))
    {
        DoAccept();
    }

private:
    void DoAccept()
    {
        m_Acceptor.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket)
            {


                if (!ec)
                {
                    std::make_shared<Session>(std::move(socket), m_Room)->Start();
                }



                DoAccept(); // 계속 accept
            }
        );
    }

    tcp::acceptor m_Acceptor;
    ChatRoom m_Room;
};




int main()
{
    std::cout << "[시스템] 채팅서버\n";


    try
    {
        boost::asio::io_context io;
        Server server(io, 7777);

        std::cout << "[Async Echo] Server listening on port 7777...\n";
        io.run(); // 이벤트 루프 시작
    }
    catch (const std::exception& e)
    {
        std::cerr << "Fatal error: " << e.what() << "\n";
    }
}

//채팅서버.cpp 끝