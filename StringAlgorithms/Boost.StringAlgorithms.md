# Глава 5. Boost.StringAlgorithms
[Boost.StringAlgorithms](http://www.boost.org/libs/algorithm/string) - библиотека, предоставляющая множество автономных функций для обработки строк. Строки могут быть типа `std::string`, `std::wstring` или любого другого экземпляра шаблонного класса `std::basic_string`. Это включает в себя строковые классы `std::u16string` и `std::u32string` введеные с C++11.

Функции классифицированы в разных заголовочных файлах. Например, функции преобразования из верхнего регистра в нижний регистр определены в `boost/algorithm/string/case_conv.hpp`. Поскольку Boost.StringAlgorithms состоит больше чем из 20 различных категорий и стольких же заголовочных файлов, `boost/algorithm/string.hpp` выступает в качестве общего заголовка, включая все другие заголовочные файлы для удобства.

#### Пример 5.1. Преобразование строк к верхнему регистру

```    
#include <boost/algorithm/string.hpp>
#include <string>
#include <iostream>

using namespace boost::algorithm;

int main()
{
  std::string s = "Boost C++ Libraries";
  std::cout << to_upper_copy(s) << '\n';
}
```

Функция `boost::algorithm::to_upper_copy()` преобразовывает строку в верхний регистр и `boost::algorithm::to_lower_copy()` преобразовывает строку в нижний регистр. Обе функции возвращают копию строки ввода, преобразованной в указанном случае. Чтобы преобразовать строку на месте, используйте функции `boost::algorithm::to_upper()` или `boost::algorithm::to_lower()`.

Пример 5.1 преобразовывает строку “Boost C++ Libraries” в верхний регистр с помощью `boost::algorithm::to_upper_copy()`. Пример выписывает "BOOST C++ LIBRARIES" в стандартный вывод.

Функции от Boost.StringAlgorithms рассматривают наборы параметров. Функции как `boost::algorithm::to_upper_copy()` используют глобальный набор параметров, если в качестве параметра явно не передан никакой другой набор параметров.

#### Пример 5.2. Преобразование строки к верхнему регистру с набором параметров

```
#include <boost/algorithm/string.hpp>
#include <string>
#include <locale>
#include <iostream>

using namespace boost::algorithm;

int main()
{
  std::string s = "Boost C++ k\xfct\xfcphaneleri";
  std::string upper_case1 = to_upper_copy(s);
  std::string upper_case2 = to_upper_copy(s, std::locale{"Turkish"});
  std::locale::global(std::locale{"Turkish"});
  std::cout << upper_case1 << '\n';
  std::cout << upper_case2 << '\n';
}
```
