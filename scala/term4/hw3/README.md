# HW3: Implicit & Typeclass

## Оглавление

* [Организационные моменты](#организационные-моменты)
    * [Дедлайн](#дедлайн)
    * [Настройка проекта](#настройка-проекта)
    * [Команды доступные в sbt shell](#команды-доступные-в-sbt-shell)
    * [Автоматическая проверка ДЗ](#автоматическая-проверка-дз)
        * [Ограничения](#ограничения)
* [**Задание 1: Расширение**](#задание-1-расширение)
* [**Задание 2: Implicit**](#задание-2-implicit)
* [**Задание 3: Typeclass**](#задание-3-typeclass)
* [Структура проекта](#структура-проекта)
* [Code style](#code-style)

## Организационные моменты

### Дедлайн

**Выдано: 29.03.2026, дедлайн 06.04.2026 18:59 по МСК**

### Настройка проекта

Прочитать гайд по настройке проекта вы можете в
`practice-01` - https://gitlab.education.tbank.ru/itmo-spring-scala-2026/practice-forks/practice-run-20260228121822/-/blob/master/practice/practice-01/README.md

Крайне рекомендуем настроить по гайду, чтобы на сохранение файла выполнялось форматирование, а на `ctrl + alt + L`(
`cmd + alt + L` на Mac) вызывало **авто форматирование с оптимизацией импортов**.

### Команды доступные в sbt shell

- `compile` - компилирует проект (без тестов);
- `Test / compile` - компилирует проект и компилирует тесты (без запуска);
- `test` - запускает все тесты;
- `scalafix` - запускает scalafix: дополнительные правила линтера и `rewrite` (ваш код автоматически **может** быть
  переписан, если он не соответствует правилам);
- `scalafmtAll` - выполняет автоматическое форматирование кода;

## Автоматическая проверка ДЗ

### Обратите внимание, что вам доступно 2 репозитория с ДЗ:

- **upstream** https://gitlab.education.tbank.ru/itmo-spring-scala-2026/homeworks/hw3-typeclass
- **fork** `https://gitlab.education.tbank.ru/itmo-spring-scala-2026/homeworks-forks/<student>/hw3-typeclass`

В **upstream** репозитории находится исходный код ДЗ, а так же вы можете открыть issue или MR с исправлением условий ДЗ,
если заметите ошибку. За каждое исправление или issue даются доп баллы (сколько баллов
указано [тут](https://gitlab.education.tbank.ru/itmo-spring-scala-2026/lecture-materials/course-info/-/blob/master/README.md)).
**fork** репозиторий индивидуален для каждого студента (у вас есть доступ только до своего репозитория, форки других
студентов вы не видите). В fork репозитории вы выполняете своё домашнее задание.

### Данное задание сдаётся автоматически:

1. Пушите ваше решение в ветку `hw` в вашем репозитории (репозиторий студента находится в его приватной группе, например
   `https://gitlab.education.tbank.ru/itmo-spring-scala-2026/homework-forks/e.n.konovalov/hw3-typeclass`);
2. Создаёте Merge Request в ветку `master`;
3. Пайплайн в MR должен успешно пройти (стать зелённым);
4. После этого в течение 15 минут:
    - если ваша работа принята MR будет закрыт и будет оставлен комментарий, что работа принята;
    - если в работе есть проблемы или ошибки, будет оставлен комментарий, что работа не принята и комментарий с
      описанием проблемы; после исправления проблемы запуште правки в ту же ветку, пайплайн в MR перезапустится
      автоматически;
    - если в течение 15 минут не появился комментарий, то перезапустите пайплайн в MR и подождите ещё 15 минут; если
      после этого ничего не случилось - напишите преподавателю.

**Не создавайте новый MR если текущий был закрыт** - переоткройте текущий (в MR нажмите `reopen`)

### Ограничения:

- у вас нет доступа до `master` ветки (ни в upstream, ни в fork)
- у вас нет прав на `push` и создание веток в upstream репозитории
- вам запрещено менять `.gitlab-ci.yml`
- вам запрещено менять файлы:
    - `.gitlab-ci.yml`
    - `.scalafmt.conf`
    - `.scalafix.conf`
    - `.jvmopts`
    - `.sbtopts`
    - `build.sbt`
    - `**/test/**` (любые тесты)
    - `project/**`
    - `library-source/**` (**любые файлы в папке** `library-source`)

## Implicit & Typeclass

Вам дана базовая реализация комплексных чисел.

```scala
final case class ComplexNumber(real: Double, imaginary: Double) {
  def *(other: ComplexNumber): ComplexNumber =
    ComplexNumber(
      (real * other.real) - (imaginary * other.imaginary),
      (real * other.imaginary) + (imaginary * other.real)
    )

  def +(other: ComplexNumber): ComplexNumber =
    ComplexNumber(
      real + other.real,
      imaginary + other.imaginary
    )

  def ~=(other: ComplexNumber): Boolean = (real - other.real).abs < 1e-6 && (imaginary - other.imaginary).abs < 1e-6
}
```

Ваша задача расширить реализацию дополнительным функционалом. Данный код находится в модуле `library-source`, и вам
запрещено его менять. Вы должны писать расширение для
[`ComplexNumber`](./library-source/src/main/scala/itmo/scala/ComplexNumber.scala) в модуле `hw-source` (тем самым мы
имитируем ситуацию, когда мы хотим расширить функционал типа к которому у нас нет доступа).

### Задание 1: Расширение

#### Часть 1: Операции

Добавьте для `ComplexNumber` операции:

- Вычитание `-`
- Деление `/` (Note: здесь предполагается что может быть брошено исключение, для этого вызовите
  `DivisionByZeroException.raise`)

#### Часть 2: Интеграция с `Numeric`

Добавьте для `ComplexNumber` возможность использовать все методы `ComplexNumber` с числовыми типами `Numeric` и
наоборот: соответствующие методы числовых типов с `ComplexNumber`.

#### Часть 3: Синтаксический сахар

Добавьте для `ComplexNumber` возможность создавать с помощью записи, напоминающей алгебраическое представление
комплексных чисел `z = a + b.i`.

### Задание 2: Implicit

В модуле `library-source` определён за вас тайпкласс `Show` который умеет преобразовывать инстансы типа `A` в строку.

```scala
trait Show[A] {
  def show(value: A): String
}

object Show {
  def apply[A](implicit show: Show[A]): Show[A] = show

  def show[A: Show](value: A): String = Show[A].show(value)

  implicit val showInt    : Show[Int]     = _.toString
  implicit val showString : Show[String]  = identity
  implicit val showBoolean: Show[Boolean] = _.toString
  implicit val showDouble : Show[Double]  = _.toString
  implicit val showChar   : Show[Char]    = _.toString
  implicit val showLong   : Show[Long]    = _.toString + "L"
  implicit val showFloat  : Show[Float]   = _.toString
  implicit val showShort  : Show[Short]   = _.toString
  implicit val showByte   : Show[Byte]    = _.toString
  implicit val showUnit   : Show[Unit]    = _ => "()"

  implicit def showOption[A: Show]: Show[Option[A]] = {
    case Some(value) => s"Some(${Show[A].show(value)})"
    case None        => "None"
  }

  implicit def showList[A: Show]: Show[List[A]] =
    _.map(Show[A].show).mkString("[", ", ", "]")
}
```

Вам необходимо создать инстанс тайпкласса `Show` для `ComplexNumber`

### Задание 3: Typeclass

Вам необходимо создать свой тайпкласс `Combine` который умеет выполнять одну операцию -
`def combine(left: A, right: A): A`, после чего:

1. Реализовать методы:
   ```scala
   def combine[T: Combine](left: T, right: T): T = ???
   def combineAll[T: Combine](head: T, tail: T*): T = ???
   def maybeCombine[T: Combine](value: T, valueOpt: Option[T]): T = ???
   ```
2. Реализовать инстансы тайпкласса `Combine` для:
    - `String`
    - `Int`
    - `ComplexNumber`

## Структура проекта:

```yaml
├── project
  │   ├── build.properties          // версия sbt
  │   ├── Dependencies.scala        // описание зависимостей
  │   └── plugins.sbt               // плагины для sbt
  ├── library-source/src            // исходники "библиотеки"
  ├── hw-source/src
  │   ├── main/scala/itmo/scala     // здесь вы пишете код
  │   └── test/scala/itmo/scala     // здесь написаны тесты которые вам можно только смотреть
...
  ├── .gitlab-ci.yml                // конфигурация пайплайна
  ├── .scalafix.conf                // конфиг для scalafix (линтер)
  ├── .scalafmt.conf                // конфиг для scalafmt (форматер)
  ├── build.sbt                     // структура и сборка проекта
  └── README.md                     // условия ДЗ
```

## Code Style:

Мы последовательно вводим список запрещенных механик, которыми нельзя пользоваться при написании кода, и рекомендаций по
code style. За нарушения мы оставляем за собой право **снижать оценку**.

* Переменные, классы и функции должны иметь осмысленные названия;
* Использовать `var` запрещается (используйте `val`);
* Использование `this` запрещается (используйте `self`, если надо);
* Использование `super` запрещается;
* Использование `null` запрещается (используйте `Option`);
* Использование `return` запрещается;
* Использование `System.<any function>` (например `System.exit`) запрещается;
* Использовать циклы запрещается (используйте `for comprehension`, `tailRec`)
* Использование java коллекций запрещается (используйте scala коллекции);
* Использование scala mutable коллекций запрещается (используйте scala immutable коллекции);
* Использовать `throw` запрещено;
* Использовать неявные преобразования запрещено;
* Касты или проверки на типы с помощью методов из Java вроде `asInstanceOf` запрещаются;
