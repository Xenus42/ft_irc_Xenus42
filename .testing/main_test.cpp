// int main(void)
// {
//     printTitle("Testing Server orthodox canonical form");
// 	{
//         Server defaultServ;
//         Server copyServ(defaultServ);
//         Server assignateServ;

//         assignateServ = defaultServ;
//     }
//     nl(1);

//     printTitle("Testing Client orthodox canonical form");
//     {
//         Client defaultClient;
//         Client copyClient(defaultClient);
//         Client assignateClient;

//         assignateClient = defaultClient;
//     }
//     nl(1);

//     printTitle("Testing minimal Server");
//     {
//         Server server(6667, "douvres");

//         server.init();
//         server.run();
//         printLog("INFO", "Waiting for client...");git 
//     }
//     nl(1);

//     return (0);
// }

// int main(void)
// {
//     int serverFd = socket(AF_INET, SOCK_STREAM, 0);
//     if (serverFd < 0)
//         return (perror("socket"), 1);

//     sockaddr_in serverAddr;
//     std::memset(&serverAddr, 0, sizeof(serverAddr));
//     serverAddr.sin_family = AF_INET;
//     serverAddr.sin_port = htons(8080);
//     serverAddr.sin_addr.s_addr = INADDR_ANY;

//     if (bind(serverFd, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
//         return (perror("bind"), 1);

//     if (listen(serverFd, 10) < 0)
//         return (perror("listen"), 1);

//     std::cout << "[SERVER] Waiting for client...\n";

//     sockaddr_in clientAddr;
//     socklen_t len = sizeof(clientAddr);

//     int clientFd = accept(serverFd, (sockaddr *)&clientAddr, &len);
//     if (clientFd < 0)
//         return (perror("accept"), 1);

//     std::cout << "[SERVER] Client connected!\n";

//     // ================= ECHO LOOP =================
//     char buffer[1024];

//     while (true)
//     {
//         int bytes = recv(clientFd, buffer, sizeof(buffer) - 1, 0);

//         if (bytes <= 0)
//         {
//             std::cout << "[SERVER] Client disconnected\n";
//             break;
//         }

//         buffer[bytes] = '\0';

//         std::cout << "[RECEIVED] " << buffer;

//         send(clientFd, buffer, bytes, 0);
//     }

//     close(clientFd);
//     close(serverFd);
//     return 0;
// }