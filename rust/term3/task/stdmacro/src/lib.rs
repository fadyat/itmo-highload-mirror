#![forbid(unsafe_code)]

#[macro_export]
macro_rules! deque {
    () => {
        ::std::collections::VecDeque::new()
    };
    ($elem:expr; $n:expr) => {{
        let v = ::std::iter::repeat($elem).take($n).collect::<::std::vec::Vec<_>>();
        ::std::collections::VecDeque::from(v)
    }};
    ($($x:expr),+ $(,)?) => {{
        ::std::collections::VecDeque::from(::std::vec![$($x),+])
    }};
}

#[macro_export]
macro_rules! sorted_vec {
    () => {
        ::std::vec::Vec::new()
    };
    ($($x:expr),+ $(,)?) => {{
        let mut v = ::std::vec![$($x),+];
        v.sort();
        v
    }};
}

#[macro_export]
macro_rules! map {
    () => {
        ::std::collections::HashMap::new()
    };
    ($($k:expr => $v:expr),+ $(,)?) => {{
        let mut pairs = ::std::vec![$(($k, $v)),+];
        let mut map = ::std::collections::HashMap::with_capacity(pairs.len());
        for (k, v) in pairs {
            map.insert(k, v);
        }
        map
    }};
}
