#include "../solution/shared_ptr.hpp"
#include "../solution/shared_ptr.hpp"

#include <type_traits>
#include <string>
#include <functional>

#include "simple_test.h"


////////////////////////////////////////////////////////////////////////////////
// incomplete


TEST(incomplete, void_empty) {
    cls10::shared_ptr<void> u;
    EXPECT_EQ(nullptr, u.get());
}

TEST(incomplete, unknown_empty) {
    struct Foo;
    cls10::shared_ptr<Foo> u;
    EXPECT_EQ(nullptr, u.get());
}

// быстрая (и в общем случае недостаточная) проверка на то, определён ли уже тип
template<class T, int Query> constexpr bool is_complete_type_v = requires { sizeof(T); };
// Query нужно для того, чтобы различать места кода, в котором тип ещё не определён / уже определён
// иначе это будет обращение к одной и той же константе

struct DefinedLater;
static_assert(!is_complete_type_v<DefinedLater, __LINE__>);
// IDE в этом месте может напрасно подумать, что static_assert ошибся
// потому что DefinedLater определён в этой единице трансляции - но ниже!

cls10::shared_ptr<DefinedLater> make_defined_later(int& dtor_count);

TEST(incomplete, defined_later) {
    static_assert(!is_complete_type_v<DefinedLater, __LINE__>);
    int dtor_count = 0;
    {
        auto u = make_defined_later(dtor_count);
        EXPECT_EQ(0, dtor_count);
        EXPECT_NE(nullptr, u.get());

        auto v = u;
        EXPECT_EQ(0, dtor_count);
        EXPECT_EQ(u.get(), v.get());
    }  // умный указатель корректно удаляет объект, потому что запомнил его (см. ниже)
    EXPECT_EQ(1, dtor_count);
}

struct DefinedLater : Object {
    using Object::Object;
};
static_assert(is_complete_type_v<DefinedLater, __LINE__>);

cls10::shared_ptr<DefinedLater> make_defined_later(int& dtor_count) {
    // умный указатель запоминает, как удалять объект, в том месте, где деструктор доступен.
    return cls10::shared_ptr<DefinedLater>{new DefinedLater{dtor_count}};
}

////////////////////////////////////////////////////////////////////////////////

TESTING_MAIN()
