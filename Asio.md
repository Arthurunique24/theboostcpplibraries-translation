# Сетевое программирование
Хотя Boost.Asio может обрабатывать любые типы данных асинхронно, его основное предназначение - сетевое программирование. Это из-за того, что в Boost.Asio сетевые функции были задолго до поддержки функционала других I/O объектов. Сетевые функции - идеальное применение для асинхронных операций, поскольку передача данных по сети может быть достаточно долгой. Из-за этого информация об успешном или неудачном завершении может не быть доступной сразу же, как начнут выполняться функции, которые отправляют и получают данные. 

Boost.Asio предоставляет много объектов ввода-вывода для разработки сетевых программ. [Пример 32.5](#Пример-325-Веб-клиент-с-boostasioiptcpsocket) использует класс `boost::asio::ip::tcp::socket` для установки соединения с другим компьютером. В этом примере посылается HTTP-запрос к веб-серверу, чтобы загрузить страницу.

### Пример 32.5. Веб-клиент с boost::asio::ip::tcp::socket
```c++
#include <boost/asio/io_service.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <array>
#include <string>
#include <iostream>

using namespace boost::asio;
using namespace boost::asio::ip;

io_service ioservice;
tcp::resolver resolv{ioservice};
tcp::socket tcp_socket{ioservice};
std::array<char, 4096> bytes;

void read_handler(const boost::system::error_code &ec,
  std::size_t bytes_transferred)
{
  if (!ec)
  {
    std::cout.write(bytes.data(), bytes_transferred);
    tcp_socket.async_read_some(buffer(bytes), read_handler);
  }
}

void connect_handler(const boost::system::error_code &ec)
{
  if (!ec)
  {
    std::string r =
      "GET / HTTP/1.1\r\nHost: theboostcpplibraries.com\r\n\r\n";
    write(tcp_socket, buffer(r));
    tcp_socket.async_read_some(buffer(bytes), read_handler);
  }
}

void resolve_handler(const boost::system::error_code &ec,
  tcp::resolver::iterator it)
{
  if (!ec)
    tcp_socket.async_connect(*it, connect_handler);
}

int main()
{
  tcp::resolver::query q{"theboostcpplibraries.com", "80"};
  resolv.async_resolve(q, resolve_handler);
  ioservice.run();
}
```
В [Примере 32.5](#Пример-325-Веб-клиент-с-boostasioiptcpsocket) используются три обработчика (handlers). `connect_handler()` и `read_handler()` вызываются, когда соединение установлено и данные получены. `resolve_handler()` используется для преобразования имени хоста в IP-адрес.

В обработчиках запускаются различные асинхронные операции из-за того, что данные могут быть получены только после установки соединения, которое появляется только после преобразования имени. В `resolve_handler()` итератор **it** используется с **tcp_socket** для установления соединения. В `connect_handler()`идёт обращение к **tcp_socket**, чтобы отправить HTTP-запрос и начать прием данных. Обработчики передаются как параметры функций, так как все операции являются асинхронными. В зависимости от типа операции могут понадобиться дополнительные параметры. Например, итератор **it** ссылается на конечную точку (endpoint), полученную из имени хоста. Массив **bytes** используется для хранения полученных данных.

В функции `main()` создаётся объект **q** типа `boost::asio::ip::tcp::resolver::query`, который представляет собой запрос для преобразователя имён хостов (name resolver) - объекта ввода-вывода, имеющего тип `boost::asio::ip::tcp::resolver`. Асинхронная операция начинается передачей объекта **q** в функцию `async_resolve()`. В [Примере 32.5](#Пример-325-Веб-клиент-с-boostasioiptcpsocket) используется хост `theboostcpplibraries.com`. Для передачи управления операционной системе после запуска асинхронной операции вызывается `run()` через объект сервиса I/O.

Когда `async_resolve()` заканчивает свою работу, вызывается `resolve_handler()`. Обработчик сначала проверяет, было ли преобразование имени успешным, в этом случае **ес** равен 0. Только после этого идёт обращение к сокету для установления соединения. Второй параметр функции - результат преобразования имени хоста в IP-адрес. Он имеет тип `boost::asio::ip::tcp::resolver::iterator` и обеспечивает адрес сервера для подключения.

Вызов `async_connect()` сопровождается вызовом обработчика `connect_handler()`. Снова сначала проверяется **ес**, чтобы выяснить, может ли быть установлено соединение.  При выполнении условия через сокет вызывается функция `async_read_some()`.  С помощью вызова этой функции начинается чтение данных. Полученные данные хранятся в массиве **bytes**, который передается в качестве первого параметра в `async_read_some()`.

Когда один или несколько байтов были получены и скопированы в **bytes**, вызывается `read_handler()`. Параметр **bytes_transferred** типа std::size_t содержит число байтов, которые были получены. Как обычно, обработчик должен сначала проверить, **ес** была ли асинхронная операция завершена успешно. Только в этом случае данные записываются в стандартный поток вывода.

Обратите внимание, что `read_handler()` снова вызывает `async_read_some()` после того, как данные были записаны в **std::cout**. Это необходимо, потому что нельзя быть уверенным, что вся страница была загружена и скопирована в **bytes** одной асинхронной операцией. Вызовы функции `async_read_some()`, которые, в свою очередь, сопровождаются вызовами `read_handler()`, заканчиваются только тогда, когда соединение закрывается. Это происходит тогда, когда веб-сервер закончил отправку. После этого условие для **ес** в `read_handler()` не выполняется, и с этого момента никакие дополнительные данные не записываются в **std::cout**. `async_read()` больше не вызывается. Поскольку нет никаких незавершенных асинхронных операций, программа завершает свою работу.

### Пример 32.6. Сервер, сообщающий время (time server), с boost::asio::ip::tcp::acceptor
```c++
#include <boost/asio/io_service.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <string>
#include <ctime>

using namespace boost::asio;
using namespace boost::asio::ip;

io_service ioservice;
tcp::endpoint tcp_endpoint{tcp::v4(), 2014};
tcp::acceptor tcp_acceptor{ioservice, tcp_endpoint};
tcp::socket tcp_socket{ioservice};
std::string data;

void write_handler(const boost::system::error_code &ec,
  std::size_t bytes_transferred)
{
  if (!ec)
    tcp_socket.shutdown(tcp::socket::shutdown_send);
}

void accept_handler(const boost::system::error_code &ec)
{
  if (!ec)
  {
    std::time_t now = std::time(nullptr);
    data = std::ctime(&now);
    async_write(tcp_socket, buffer(data), write_handler);
  }
}

int main()
{
  tcp_acceptor.listen();
  tcp_acceptor.async_accept(tcp_socket, accept_handler);
  ioservice.run();
}
```
В [Примере 32.6](#Пример-326-Сервер-времени-time-server-с-boostasioiptcpacceptor) вы можете связаться с сервером при помощи протокола telnet, чтобы получить текущее время. После этого сервер выключается.

Сервер использует объект ввода-вывода `boost::asio::ip::tcp::acceptor`, чтобы принять входящее соединение из другой программы. Вы должны инициализировать этот объект и предоставить информацию, какой протокол и порт использовать. В этом примере переменной **tcp_acceptor** с помощью **tcp_endpoint** типа `boost::asio::ip::tcp::endpoint` задаётся приём входящих соединений по протоколу IPv4 на порту 2014.

После инициализации акцептора (acceptor) вызывается `listen()`, чтобы подготовить его к принятию входящих соединений. Затем вызывается `async_accept()`, чтобы принять запрос на установление соединения от удаленного хоста. Сокет должен быть передан первым параметром в `async_accept()`. Он будет использоваться для отправки и приема данных на новом соединении.

Как только другая программа устанавливает соединение, вызывается `accept_handler()`. Если соединение было установлено успешно, текущее время отправляется с помощью функции `boost::asio::async_write()`. Эта функция записывает все данные в строку **data**. `boost::asio::ip::tcp::socket` также предоставляет функцию-член `async_write_some()`. Эта функция вызывает обработчик, когда был послан по крайней мере один байт. Затем обработчик должен проверить, сколько байт было отправлено и сколько еще нужно отправить. Затем он снова должен вызвать `async_write_some()`. Используя `boost::asio::async_write()`, можно избежать многократного вычисления числа байтов, которых нужно отправить, и вызова `async_write_some()`. Асинхронная операция, начатая этой функцией, завершается только тогда, когда были отправлены все байты в **data**.

После отправки данных вызывается `write_handler()`. Эта функция вызывает `shutdown()` с параметром  **boost::asio::ip::tcp::socket::shutdown_send**, который сообщает, что программа закончила отправку данных через сокет. Поскольку нет никаких незавершенных асинхронных операций, [Пример 32.6](#Пример-326-Сервер-времени-time-server-с-boostasioiptcpacceptor) завершается. Обратите внимание, что, хотя переменная **data** используется только в `accept_handler()`, она не может быть локальной переменной. **data** передается по ссылке из `boost::asio::buffer()` в `boost::asio::async_write()`. Когда `boost::asio::async_write()` и `accept_handler()` завершаются, асинхронная операция началась, но еще не завершилась. **data** должна существовать, пока асинхронная операция не завершена. Это гарантируется, если **data** является глобальной переменной.

# Сопрограммы (coroutines)
Начиная с версии 1.54.0, Boost.Asio поддерживает сопрограммы. Вы можете использовать Boost.Coroutine напрямую, но явная поддержка сопрограмм в Boost.Asio облегчает их использование.

Сопрограммы позволяют создать структуру, которая отражает реальную логику программы. Асинхронные операции не делят функции на части, так как нет никаких обработчиков, которые бы определили, что должно произойти, когда асинхронная операция завершается. Программа может использовать последовательную структуру вместо вызовов обработчиками друг друга.

### Пример 32.7. Сопрограммы с Boost.Asio
```c++
#include <boost/asio/io_service.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <list>
#include <string>
#include <ctime>

using namespace boost::asio;
using namespace boost::asio::ip;

io_service ioservice;
tcp::endpoint tcp_endpoint{tcp::v4(), 2014};
tcp::acceptor tcp_acceptor{ioservice, tcp_endpoint};
std::list<tcp::socket> tcp_sockets;

void do_write(tcp::socket &tcp_socket, yield_context yield)
{
  std::time_t now = std::time(nullptr);
  std::string data = std::ctime(&now);
  async_write(tcp_socket, buffer(data), yield);
  tcp_socket.shutdown(tcp::socket::shutdown_send);
}

void do_accept(yield_context yield)
{
  for (int i = 0; i < 2; ++i)
  {
    tcp_sockets.emplace_back(ioservice);
    tcp_acceptor.async_accept(tcp_sockets.back(), yield);
    spawn(ioservice, [](yield_context yield)
      { do_write(tcp_sockets.back(), yield); });
  }
}

int main()
{
  tcp_acceptor.listen();
  spawn(ioservice, do_accept);
  ioservice.run();
}
```
`boost::asio::spawn()` - функция, которую нужно вызывать для использования сопрограмм в Boost.Asio. Первый передаваемый параметр должен быть объектом сервиса I/O. Второй параметр - сопрограммная функция. В качестве единственного параметра она должна принимать объект типа `boost::asio::yield_context`. Также она не должна иметь возвращаемого значения. [Пример 32.7](#Пример-327-Сопрограммы-с-boostasio) использует `do_accept()` и `do_write()` как сопрограммы. Если сигнатура функции отличается, как в случае с `do_write()`, то нужно использовать адаптер, например, `std::bind`, или же лямбда-функции.

Вместо обработчика вы можете передать объект типа `boost::asio::yield_context` в асинхронную функцию. `do_accept()` и `do_write()` передают параметр **yield** в функции `async_accept()` и `async_write()` соответственно. Они по-прежнему запускают асинхронные операции, но теперь никакие обработчики не вызовутся при их завершении. Вместо этого восстанавливается состояние программы, когда были начаты асинхронные операции. Когда они завершатся, программа продолжит с того места, где она была прервана.

`do_accept()` содержит цикл `for`. Новый сокет передается `async_accept()` каждый раз при вызове функции. Когда клиент устанавливает соединение, в качестве сопрограммы с `boost::asio::spawn()` вызывается `do_write()` для отправки текущего времени клиенту.

Цикл `for` позволяет увидеть, что программа может обработать двух клиентов перед выходом. Так как пример основан на сопрограммах, в цикле `for` может быть реализовано повторное выполнение асинхронной операции. Это улучшает читаемость программы, так как не нужно отслеживать потенциальные вызовы обработчиков, чтобы выяснить, когда последняя асинхронная операция будет завершена. Если сервер, сообщающий время, должен поддерживать более двух клиентов, нужно модифицировать лишь цикл `for`.

# Платформо-зависимые объекты I/O
До сих пор все примеры, приведенные в этой главе, были независимыми от платформы. Объекты ввода-вывода, такие как `boost::asio::steady_timer` и `oost::asio::ip::tcp::socket`, поддерживаются на всех платформах. Тем не менее Boost.Asio также предоставляет платформо-зависимые объекты ввода-вывода, поскольку некоторые асинхронные операции доступны только на определенных платформах, например, Windows или Linux.

### Пример 32.8. Использование boost::asio::windows::object_handle
```c++
#include <boost/asio/io_service.hpp>
#include <boost/asio/windows/object_handle.hpp>
#include <boost/system/error_code.hpp>
#include <iostream>
#include <Windows.h>

using namespace boost::asio;
using namespace boost::system;

int main()
{
  io_service ioservice;

  HANDLE file_handle = CreateFileA(".", FILE_LIST_DIRECTORY,
    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
    OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);

  char buffer[1024];
  DWORD transferred;
  OVERLAPPED overlapped;
  ZeroMemory(&overlapped, sizeof(overlapped));
  overlapped.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
  ReadDirectoryChangesW(file_handle, buffer, sizeof(buffer), FALSE,
    FILE_NOTIFY_CHANGE_FILE_NAME, &transferred, &overlapped, NULL);

  windows::object_handle obj_handle{ioservice, overlapped.hEvent};
  obj_handle.async_wait([&buffer, &overlapped](const error_code &ec) {
    if (!ec)
    {
      DWORD transferred;
      GetOverlappedResult(overlapped.hEvent, &overlapped, &transferred,
        FALSE);
      auto notification = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer);
      std::wcout << notification->Action << '\n';
      std::streamsize size = notification->FileNameLength / sizeof(wchar_t);
      std::wcout.write(notification->FileName, size);
    }
  });

  ioservice.run();
}
```
В [Примере 32.8](#Пример-328-Использование-boostasiowindowsobject_handle) используется объект ввода-вывода `boost::asio::windows::object_handle`, который доступен только на Windows. Объект `boost::asio::windows::object_handle`, основанныый на Windows-функции `RegisterWaitForSingleObject()`, позволяет запускать асинхронные операции для дескриптора объекта (object handle). Все дескрипторы, которые принимает `RegisterWaitForSingleObject(),` могут использоваться с `boost::asio::windows::object_handle`. С помощью `async_wait()` возможно асинхронно ожидать, пока не изменится дескриптор объекта.

В [Примере 32.8](#Пример-328-Использование-boostasiowindowsobject_handle) инициализируется объект **obj_handle**, имеющий  тип `boost::asio::windows::object_handle`, с дескриптором, созданным с помощью Windows-функции `CreateEvent()`. Дескриптор является частью структуры `OVERLAPPED`, чей адрес передается Windows-функции `ReadDirectoryChangesW()`. Windows использует структуры `OVERLAPPED` для запуска асинхронных операций.

Функция `ReadDirectoryChangesW()` может быть использована для отслеживания каталога и ожидания изменений в нём. Чтобы вызов функции был асинхронный, нужно передать структуру `OVERLAPPED` в `ReadDirectoryChangesW()`. Чтобы сообщить о завершении асинхронной операции через Boost.Asio, обработчик событий сохраняется в структуре `OVERLAPPED` до того, как он передается `ReadDirectoryChangesW()`. Впоследствии этот обработчик события передается **obj_handle**. После того, как через объект **obj_handle** вызывется `async_wait()`, обработчик будет ожидать изменений в наблюдаемой директории и выполнится, когда они произойдут.

При запуске [Примера 32.8](#Пример-328-Использование-boostasiowindowsobject_handle) создайте новый файл в каталоге, из которого будет запущен пример. Программа обнаружит новый файл и напишет сообщение в стандартный поток вывода.

### Пример 32.9. Использование boost::asio::windows::overlapped_ptr
```c++
#include <boost/asio/io_service.hpp>
#include <boost/asio/windows/overlapped_ptr.hpp>
#include <boost/system/error_code.hpp>
#include <iostream>
#include <Windows.h>

using namespace boost::asio;
using namespace boost::system;

int main()
{
  io_service ioservice;

  HANDLE file_handle = CreateFileA(".", FILE_LIST_DIRECTORY,
    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
    OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);

  error_code ec;
  auto &io_service_impl = use_service<detail::io_service_impl>(ioservice);
  io_service_impl.register_handle(file_handle, ec);

  char buffer[1024];
  auto handler = [&buffer](const error_code &ec, std::size_t) {
    if (!ec)
    {
      auto notification =
        reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer);
      std::wcout << notification->Action << '\n';
      std::streamsize size = notification->FileNameLength / sizeof(wchar_t);
      std::wcout.write(notification->FileName, size);
    }
  };
  windows::overlapped_ptr overlapped{ioservice, handler};
  DWORD transferred;
  BOOL ok = ReadDirectoryChangesW(file_handle, buffer, sizeof(buffer),
    FALSE, FILE_NOTIFY_CHANGE_FILE_NAME, &transferred, overlapped.get(),
    NULL);
  int last_error = GetLastError();
  if (!ok && last_error != ERROR_IO_PENDING)
  {
    error_code ec{last_error, error::get_system_category()};
    overlapped.complete(ec, 0);
  }
  else
  {
    overlapped.release();
  }

  ioservice.run();
}
```
[Пример 32.9](#Пример-329-Использование-boostasiowindowsoverlapped_ptr), как и предыдущий, использует функцию `ReadDirectoryChangesW()`, чтобы отслеживать изменения в каталоге. На этот раз асинхронный вызов `ReadDirectoryChangesW()` не связан с обработчиком событий. В примере используется класс `boost::asio::windows::overlapped_ptr`, который в своей реализации использует структуру `OVERLAPPED`. `get()` возвращает указатель на внутреннюю структуру `OVERLAPPED`, который затем передается в `ReadDirectoryChangesW()`.

`boost::asio::windows::overlapped_ptr` - это объект I/O, у которого нет функции-члена, способной начать асинхронную операцию. Но она может быть запущена при передаче указателя на внутреннюю переменную `OVERLAPPED` в Windows-функцию. В качестве ещё одного параметра конструктор `boost::asio::windows::overlapped_ptr` ожидает обработчик, который будет вызван, когда асинхронная операция завершится.

В [Примере 32.9](#Пример-329-Использование-boostasiowindowsoverlapped_ptr) используется `boost::asio::use_service()`, чтобы получить ссылку на службу в объекте службы I/O **ioservice**. `boost::asio::use_service()` представляет собой шаблонную функцию. Тип службы ввода-вывода, который нужно извлечь, должен быть передан в качестве параметра шаблона. В данном примере передается `boost::asio::detail::io_service_impl`. Этот тип сервиса I/O самый низкоуровневый и наиболее близок к операционной системе. В операционной системе Windows `boost::asio::detail::io_service_impl` использует IOCP, а в Linux используется `epoll()`. `boost::asio::detail::io_service_impl` - это определение типа, который определяется как `boost::asio::detail::win_iocp_io_service` для Windows и `boost::asio::detail::task_io_service` для Linux.

`boost::asio::detail::win_iocp_io_service` предоставляет функцию-член `register_handle()`, чтобы связать дескриптор объекта с IOCP-дескриптором. `register_handle()` вызывает Windows-функцию `CreateIoCompletionPort()`. Этот вызов необходим для примера, чтобы он работал правильно. Дескриптор, который возвращается функцией `CreateFileA()`, может быть передан через объект **overlapped** в `ReadDirectoryChangesW()` только после того, как он будет связан с IOCP-дескриптором.

В [Примере 32.9](#Пример-329-Использование-boostasiowindowsoverlapped_ptr) проверяется, удачно ли завершилась функция `ReadDirectoryChangesW()`. Если `ReadDirectoryChangesW()` завершилась с ошибкой, через объект **overlapped** вызывается `complete()`, чтобы завершить асинхронную операцию. Параметры, передаваемые для функции `complete()` перенаправляются к обработчику.

Если `ReadDirectoryChangesW()` завершается успешно, вызывается `release()`. Далее асинхронная операция находится в ожидании. Она завершится только после завершения операции, которая была начата Windows-функцией `ReadDirectoryChangesW()`.

### Пример 32.10. Использование boost::asio::posix::stream_descriptor
```c++
#include <boost/asio/io_service.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/write.hpp>
#include <boost/system/error_code.hpp>
#include <iostream>
#include <unistd.h>

using namespace boost::asio;

int main()
{
  io_service ioservice;

  posix::stream_descriptor stream{ioservice, STDOUT_FILENO};
  auto handler = [](const boost::system::error_code&, std::size_t) {
    std::cout << ", world!\n";
  };
  async_write(stream, buffer("Hello"), handler);

  ioservice.run();
}
```
В [Примере 32.10](#Пример-3210-Использование-boostasioposixstream_descriptor) представляется объект I/O для POSIX платформ.

Чтобы начать асинхронную операцию в файле, нужно передать его файловый дескриптор при инициализации объекта `boost::asio::posix::stream_descriptor`. В данном примере объект **stream** связан с дескриптором файла `STDOUT_FILENO`. Это позволяет записать строку в стандартный поток вывода асинхронно.
