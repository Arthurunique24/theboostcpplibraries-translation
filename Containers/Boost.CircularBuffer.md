# Глава 16. Boost.CircularBuffer

Библиотека Boost.CircularBuffer обеспечивает *кольцевой буфер*, который является контейнером со следующими двумя фундаментальными свойствами:
- Вместимость кольцевого буфера постоянная и установлена вами. Вместимость не изменяется автоматически, когда вы вызываете функцию, такую как `push_back()`. Только вы можете изменить размер кольцевого буфера. Размер кольцевого буфера не может превысить ёмкость, которую вы установили.
- Несмотря на постоянную ёмкость, вы можете вызвать `push_back()` так часто, как вам хочется, чтобы вставлять элементы в кольцевой буфер. Если максимальный размер был достигнут, и кольцевой буфер полон, элементы заменяются новыми.

Использовать кольцевой буфер имеет смысл, когда объем доступной памяти ограничен, и вам необходимо, препятствовать тому, чтобы контейнер стал произвольно большим. Другим примером может служить непрерывный поток данных, в котором старые данные становится неважными, когда новые данные становятся доступными. Память выделенная на старые данные автоматически снова используется путем перезаписи этих данных.

Чтобы использовать кольцевой буфер из Boost.CircularBuffer, включите заголовочный файл `boost/circular_buffer.hpp`. Этот файл определяет класс `boost::circular_buffer`.

<a name="example161">
### Пример 16.1. Использование `boost::circular_buffer`
```c++
#include <boost/circular_buffer.hpp>
#include <iostream>

int main()
{
  typedef boost::circular_buffer<int> circular_buffer;
  circular_buffer cb{3};

  std::cout << cb.capacity() << '\n';
  std::cout << cb.size() << '\n';

  cb.push_back(0);
  cb.push_back(1);
  cb.push_back(2);

  std::cout << cb.size() << '\n';

  cb.push_back(3);
  cb.push_back(4);
  cb.push_back(5);

  std::cout << cb.size() << '\n';

  for (int i : cb)
    std::cout << i << '\n';
}
```

`boost::circular_buffer` представляет собой шаблон, и должен быть инициализирован с типом. Например, кольцевой буфер **cb** в [Примере 16.1](#example161) хранит числа типа `int`.

Емкость кольцевого буфера задается при создании экземпляра класса, а не с помощью параметра шаблона. Конструктор по умолчанию `boost::circular_buffer` создает буфер с ёмкостью 0. Другой конструктор позволяет установить ёмкость. В [Примере 16.1](#example161), буфер **cb** имеет емкость трех элементов.

Емкость кольцевого буфера может быть запрошена с помощью вызова `capacity()`. В [Примере 16.1](#example161), `capacity()` вернет 3.

Емкость не эквивалентна числу хранимых элементов. В то время как возвращаемое значение `capacity()` константа, `size()` возвращает число элементов в буфере, которое может быть различным. Возвращаемое значение `size()` будет всегда между 0 и ёмкостью кольцевого буфера.

[Пример 16.1](#example161) при первом вызове `size()` вернул 0, поскольку буфер не содержал в себе каких-либо данных. После того, как `push_back()` вызвали три раза, буфер стал содержать три элемента, и второй вызов `size()` вернул 3. Вызвав `push_back()` снова не приводит к росту буфера. Три новых числа перезаписали предыдущие три. Таким образом `size()` вернул 3, когда его вызвали в третий раз.

В качестве проверки, в конце сохраненные числа записываются в стандартный вывод [Примера 16.1](#example161). Вывод содержит числа 3, 4 и 5, так как ранее сохраненные числа были перезаписаны.

<a name="example162">
### Пример 16.2. Различные функции в `boost::circular_buffer`
```c++
#include <boost/circular_buffer.hpp>
#include <iostream>

int main()
{
  typedef boost::circular_buffer<int> circular_buffer;
  circular_buffer cb{3};

  cb.push_back(0);
  cb.push_back(1);
  cb.push_back(2);
  cb.push_back(3);

  std::cout << std::boolalpha << cb.is_linearized() << '\n';

  circular_buffer::array_range ar1, ar2;

  ar1 = cb.array_one();
  ar2 = cb.array_two();
  std::cout << ar1.second << ";" << ar2.second << '\n';

  for (int i : cb)
    std::cout << i << '\n';

  cb.linearize();

  ar1 = cb.array_one();
  ar2 = cb.array_two();
  std::cout << ar1.second << ";" << ar2.second << '\n';
}
```

[Пример 16.2](#example162) использует функции  `is_linearized()`, `array_one()`, `array_two()` и `linearize()`, которые не существуют в других контейнерах. Эти функции разъясняют свойства кольцевого буфера.

Кольцевой буфер по сути сравним с `std::vector`. Поскольку начало и конец чётко определены, вектор может быть обработан как обычный массив Си. То есть выделяется непрерывный блок памяти, а первый и последний элементы всегда находятся на самом низком и высоком адресе выделенной памяти соответственно. Однако кольцевой буфер не дает такой гарантии.

Говорить о конце и начале кольцевого буфера может покзазаться странным, однако они действительно существуют. Элементы могут быть доступны через итераторы, и `boost::circular_buffer` предоставляет функции, такие как `begin()` и `end()`. Используя итераторы вы не должны быть обеспокоены положением начала и конца, но ситуация может стать немного более сложной, если получать доступ к элементам используя обычные указатели без использования `is_linearized()`, `array_one()`, `array_two()` и `linearize()`.

Функция `is_linearized()` возвращает `true`, если начало кольцевого буфера находится на самом нижнем адресе выделенной памяти. В этом случае все элементы в буфере хранятся последовательно от начала до конца при увеличении адреса памяти, и элементы могут быть доступны, как обычный массив Си.

Если `is_linearized()` возвращает `false`, то это значит, что начало кольцевого буфера находится не в самом нижнем адресе выделенной памяти, такой случай показан в [Примере 16.2](#example162). В то время как первые три элемента 0, 1 и 2 хранятся именно в таком порядке, вызвав `push_back()` в четвертый раз число 3 перепишет число 0. Поскольку 3 является последним элементом, добавленным вызовом `push_back()`, то это теперь новый конец кольцевого буфера. Началом теперь является элемент с номером 1, который хранится на более высоком адресе выделенной памяти. Это означает, что элементы больше не хранятся последовательно по увеличению адреса памяти.

Если конец кольцевого буфера находится на более низком адресе выделенной памяти, чем начало, то элементы могут быть доступны через два обычных массива Си. Для того, чтобы избежать необходимости вычислить позицию в памяти и размер каждого массива, `boost::circular_buffer` предоставляет функции `array_one()` и `array_two()`.

И `array_one()`, и `array_two()` вернут `std::pair`, где первый элемент будет указателем на соответствующий массив, а второй элемент будет размером. `array_one()` получает доступ к массиву в начале кольцевого буфера, а `array_two()` получает доступ к массиву в конце буфера.

Если кольцевой буфер линейный и `is_linearized()` возвращает `true`, `array_two()` может быть так же вызван. Однако, так как существует только один массив в буфере, второй массив не содержит элементов.

Для упрощения, чтобы считать кольцевой буфер подобно обычному массиву Си, вы можете принудительно переставлять элементы посредством вызова `linearize()`. После завершения, вы можете получить доступ ко всем элементам, полученных с помощью `array_one()`, и вам не нужно использовать `array_two()`.

Boost.CircularBuffer предлагает дополнительный класс под названием `boost::circular_buffer_space_optimized`. Этот класс также определен в файле `boost/circular_buffer.hpp`. Хотя этот класс используется таким же образом как и `boost::circular_buffer`, он не резервирует память в экземпляре. Скорее, память выделяется динамически, когда добавляются элементы, до тех пор пока не будет достигнута ёмкость. Удаление элементов освобождает память. `boost::circular_buffer_space_optimized` управляет памятью более эффективно, и, поэтому может быть лучшим выбором в некоторых сценариях. Например, это может быть хорошим выбором, если вам нужен кольцевой буфер с большой ёмкостью, но ваша программа не всегда использует полный буфер.
