# Глава 19. Boost.MultiArray

[Boost.MultiArray](http://www.boost.org/doc/libs/1_62_0/libs/multi_array/doc/index.html) это библиотека, которая упрощает использование массивов с несколькими измерениями. Наиболее важным преимуществом является то, что многомерные массивы можно использовать как контейнеры из стандартной библиотеки. Например, есть функции, такие, как `begin()` и `end()`, которые позволяют получить доступ к элементам в многомерных массивов через итераторы. Итераторы проще в использовании, чем указатели, которые обычно используются с массивами Си, особенно с массивами, которые имеют много измерений.

<a name="example191"></a>
### Пример 19.1 Одномерный массив с `boost::multi_array`
```c++
#include <boost/multi_array.hpp>
#include <iostream>

int main()
{
  boost::multi_array<char, 1> a{boost::extents[6]};

  a[0] = 'B';
  a[1] = 'o';
  a[2] = 'o';
  a[3] = 's';
  a[4] = 't';
  a[5] = '\0';

  std::cout << a.origin() << '\n';
}
```
Boost.MultiArray предоставляет класс `boost::multi_array` для создания массивов. Это наиболее важный предоставляемый класс. Он определен в `boost/multi_array.hpp`.

`boost::multi_array` это шаблон, и он ожидает два параметра: первый параметр это тип элементов для хранения в массиве. Второй параметр определяет какой размер массива должен быть.

Второй параметр только задает число измерений, а не количество элементов в каждом измерении. Таким образом, в [Примере 19.1](#example191), **а** является одномерным массивом.

Число элементов в измерении устанавливается во время выполнения. [Пример 19.1](#example191) использует глобальный объект **boost::extents**, чтобы установить размеры измерений. Этот объект передается конструктору **a**.

Объект типа `boost::multi_array` может быть использован как обычный массив Си. К элементам получают доступ, передавая индекс к `operator[]`. [Пример 19.1](#example191) хранит пять букв и нулевой символ в **a** - одномерный массив с шестью элементами. `origin()` возвращает указатель на первый элемент. В этом примере используется этот указатель, чтобы написать слово, хранимое в массиве - **Boost**  - в стандартный вывод.

В отличие от контейнеров из стандартной библиотеки, `operator[]` проверяет является ли индекс действительным. Если индекс не является действительным, программа завершается с `std::abort()`. Если вы не хотите проверять действительность индексов, необходимо определить макрос `BOOST_DISABLE_ASSERTS` перед включением `boost/multi_array.hpp`.

<a name="example192"></a>
### Пример 19.2. Виды и подмножества двумерного массива 
```c++
#include <boost/multi_array.hpp>
#include <algorithm>
#include <iostream>
#include <cstring>

int main()
{
  boost::multi_array<char, 2> a{boost::extents[2][6]};

  typedef boost::multi_array<char, 2>::array_view<1>::type array_view;
  typedef boost::multi_array_types::index_range range;
  array_view view = a[boost::indices[0][range{0, 5}]];

  std::memcpy(view.origin(), "tsooB", 6);
  std::reverse(view.begin(), view.end());

  std::cout << view.origin() << '\n';

  boost::multi_array<char, 2>::reference subarray = a[1];
  std::memcpy(subarray.origin(), "C++", 4);

  std::cout << subarray.origin() << '\n';
}
```

[Пример 19.2](#example192) создает двумерный массив. Количество элементов в первом измерении устанавливается равным 2, а во втором измерении равным 6. Думайте о массиве в виде таблицы с двумя строками и шестью столбцами.

Первая строка таблицы будет содержать слово Boost. Так как только пять букв должны быть сохранены для этого слова, создается *вид(view)*, который охватывает ровно пять элементов массива.

Вид, который основан на классе `boost::multi_array::array_view`, позволяет получить доступ к части массива и рассматривать ту часть, как будто это отдельный массив.

`boost::multi_array::array_view` это шаблон, который ожидает число измерений в качестве параметра шаблона. В [Примере 19.2](#example192) число измерений для вида равно 1. Поскольку массив **а** имеет два измерения, одно измерение игнорируется. Чтобы сохранить слово Boost, одномерного массива достаточно; больше измерений будет сбивать с толку.

Как и в случае `boost::multi_array`, число измерений передается в качестве параметра шаблона, а размер каждого измерения устанавливается во время выполнения. Тем не менее, с `boost::multi_array::array_view` это не будет сделано с помощью **boost::extents**. Вместо этого, это будет сделано с использованием **boost::indices**, который является еще одним глобальным объектом предоставляемым Boost.MultiArray.

Как с **boost::extents**, индексы должны быть переданы **boost::indices**. В то время как только цифры могут быть переданы для **boost::extents**, **boost::indices** принимает также диапазоны. Они определяются с использованием `boost::multi_array_types::index_range`.

В [Примере 19.2](#example192), первый параметр передаваемый к **boost::indices** не диапазон, а число 0. Когда передано число, вы не можете использовать `boost::multi_array_types::index_range`. В примере, вид будет принимать первое измерение **a** - один с индексом 0.

Для второго параметра, `boost::multi_array_types::index_range` используется для определения диапазона. Передавая 0 и 5 в конструктор, стали доступными первые пять элементов первой размерности. Диапазон начинается с индекса 0 и заканчивается на индексе 5 - за исключением элемента с индексом 5. Шестой элемент в первом измерении игнорируется.

Таким образом, **вид(view)** является одномерным массивом, состоящим из пяти элементов - первые пять элементов в первом ряду **а**. Когда **вид(view)** доступен для копирования строки с `std::memcpy()` и смены элементов местами с `std::reverse()`, связь не имеет значения. После создания вида(view), он действует как независимый массив с пятью элементами.

Когда `operator[]` вызывается для массива типа `boost::multi_array`, возвращаемое значение зависит от числа измерений. В [Примере 19.1](#example191), оператор возвращает `char` элементы, поскольку массив является одномерным.

В [Примере 19.2](#example192) **a** - двумерный массив. Таким образом, `operator[]` возвращает подмножество, а не `char` элемент. Поскольку тип подмножества не публичный, нужно использова `boost::multi_array::reference`. Этот тип не идентичен `boost::multi_array::array_view`, даже если подмножество ведет себя как вид(view). Вид(View) должен быть определен в явном виде, и может охватывать произвольные части массива, в то время как `operator[]` автоматически возвращает подмножество, и охватывает все элементы в каждом измерении.

<a name="example193"></a>
### Пример 19.3. Обертывание массива Си с `boost::multi_array_ref`
```c++
#include <boost/multi_array.hpp>
#include <algorithm>
#include <iostream>
#include <cstring>

int main()
{
  char c[12] =
  {
    't', 's', 'o', 'o', 'B', '\0',
    'C', '+', '+', '\0', '\0', '\0'
  };

  boost::multi_array_ref<char, 2> a{c, boost::extents[2][6]};

  typedef boost::multi_array<char, 2>::array_view<1>::type array_view;
  typedef boost::multi_array_types::index_range range;
  array_view view = a[boost::indices[0][range{0, 5}]];

  std::reverse(view.begin(), view.end());
  std::cout << view.origin() << '\n';

  boost::multi_array<char, 2>::reference subarray = a[1];
  std::cout << subarray.origin() << '\n';
}
```

Класс `boost::multi_array_ref` оборачивает существующий массив Си. В [Примере 19.3](#example193), **a** обеспечивает тот же самый интерфейс, что и `boost::multi_array`, но без выделения памяти. С `boost::multi_array_ref`, массив Cи можно рассматривать как многомерный массив типа `boost::multi_array`, независимо от того, какую размерность он имеет. Массив Си просто должен быть добавлен в качестве дополнительного параметра конструктору.

Boost.MultiArray также предоставляет класс `boost::const_multi_array_ref`, который обрабатывает массив Cи как постоянный многомерный массив.