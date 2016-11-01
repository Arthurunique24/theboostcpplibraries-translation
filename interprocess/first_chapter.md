# Общая память #

Общая память, как правило, самая быстрая форма межпроцессного взаимодействия. Это обеспечивает область памяти, которая разделяется 
между процессами. Один процесс может записывать данные в этой области, а другой процесс может читать его.

В Boost.Interprocess класс `boost::interprocess::shared_memory_object` используется для представления общей памяти. Включите в проект 
заголовочный файл `boost/interprocess/shared_memory_object.hpp` для использования этого класса.

##### Пример 33.1. Создание общей памяти
```c++
#include <boost/interprocess/shared_memory_object.hpp>
#include <iostream>

using namespace boost::interprocess;

int main()
{
  shared_memory_object shdmem{open_or_create, "Boost", read_write};
  shdmem.truncate(1024);
  std::cout << shdmem.get_name() << '\n';
  offset_t size;
  if (shdmem.get_size(size))
    std::cout << size << '\n';
}
```

Конструктор `boost::interprocess::shared_memory_object` принимает три параметра. Первый параметр определяет, должна ли быть создана общая память или открыта. [Пример 33.1](#Пример-331-Создание-общей-памяти) 
обрабатывает оба варианта. `boost::interprocess::open_or_create` откроет общую память, если она уже существует или создаст ее в обратном  случае. 


