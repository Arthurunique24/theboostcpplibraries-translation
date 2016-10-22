# Сетевое программирование
Даже если Boost.Asio может обрабатывать любые виды данных асинхронно, в основном он используется для сетевого программирования. Это потому, что Boost.Asio поддерживал сетевые функции задолго до того, как были добавлены объекты ввода-вывода. Сетевые функции - идеальное использование для асинхронных операций, потому что передача данных через интернет может занять много времени. Это означает, что информация об успешном завершении передачи или об ошибке могут не быть доступными сразу же, как выполнятся функции, которые отправляют и получают данные.

Boost.Asio предоставляет много объектов ввода-вывода, чтобы разрабатывать сетевые программы. Пример 32.5 использует класс `boost::asio::ip::tcp::socket`, чтобы установить соединение с другим компьютером. Этот пример посылает HTTP-запрос к веб-серверу, чтобы загрузить страницу.

##### Пример 32.5. Веб-клиент с `boost::asio::ip::tcp::socket`
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
Пример 32.5 использует три обработчика: `connect_handler()` и `read_handler()` вызываются, когда соединение установлено и данные получены. `resolve_handler()` используется для разрешения имён.

Поскольку данные могут быть получены только после того, как соединение было установлено, а так же связь может быть установлена только после того, как имя было разрешено, различные асинхронные операции запускаются в обработчиках. В функции `resolve_handler()` итератор **it**, который указывает на конечную точку разрешенной из названия, используется с **tcp_socket** для установления соединения. В connect_handler (), tcp_socket доступен, чтобы отправить запрос HTTP и начать прием данных. Так как все операции являются асинхронными, обработчики передаются в соответствующие функции. В зависимости от операций, дополнительные параметры, возможно, должны быть переданы. Например, итераторы это относится к конечной точке от разрешенного имени. Массив байтов используется для хранения полученных данных.
