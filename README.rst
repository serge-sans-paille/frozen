Frozen
######

.. image:: https://travis-ci.org/serge-sans-paille/frozen.svg?branch=master
   :target: https://travis-ci.org/serge-sans-paille/frozen

Header-only, immutable (a.ka. frozen), ``constexpr``-compatible versions of
``std::set``, ``std::unordered_set``, ``std::map`` and ``std::unordered_map``.

The ``unordered_*`` containers are guaranteed *perfect* (a.k.a. no hash
collision) and the extra storage is linear with respect to the number of keys.

Once initialized, the containers cannot be updated, and in exchange, lookups
are faster. And initialization is free when ``constexpr`` is used :-).


Installation
------------

Just copy the ``include/frozen`` directory somewhere and points to it using the ``-I`` flag.

Requirements
------------

A C++ compiler that supports C++14. Clang version 5 is a good pick, GCC version
6 lags behind in terms of ``constexpr`` compilation time. At least on my setup.

Note that gcc 5 isn't supported. (Here's an `old compat branch`_ where a small amount of stuff was ported.)

.. _old compat branch: https://github.com/cbeck88/frozen/tree/gcc5-support

Usage
-----

Compiled with ``-std=c++14`` flag:

.. code:: C++

    #include <frozen/set.h>

    constexpr frozen::set<int, 4> some_ints = {1,2,3,5};

    constexpr bool letitgo = some_ints.count(8);

    extern int n;
    bool letitgoooooo = some_ints.count(n);


As the constructor and some methods are ``constexpr``, it's also possible to write weird stuff like:

.. code:: C++

    #include <frozen/set.h>

    template<std::size_t N>
    std::enable_if_t< frozen::set<int, 3>{{1,11,111}}.count(N), int> foo();

String support is built-in through a lightweight wrapper:

.. code:: C++

    #include <frozen/unordered_map.h>
    #include <frozen/string.h>

    constexpr frozen::unordered_map<frozen::string, int, 2> olaf = {
        {"19", 19},
        {"31", 31},
    };
    constexpr auto val = olaf.at("19");

You may also prefer a slightly more DRY initialization syntax:

.. code:: C++

    #include <frozen/set.h>

    constexpr auto some_ints = frozen::make_set<int>({1,2,3,5});

There are similar ``make_X`` functions for all frozen containers.

Extending
---------

Just like the regular C++14 container, you can specialize the hash function,
the key equality comparator for ``unordered_*`` containers, and the comparison
functions for the ordered version.

It's also possible to specialize the ``frozen::elsa`` structure used for
hashing. Note that unlike `std::hash`, the hasher also takes a seed in addition
to the value being hashed.

.. code:: C++

    template <class T> struct elsa {
      // in case of collisions, different seeds are tried
      constexpr std::size_t operator()(T const &value, std::size_t seed) const;
    };

Ideally, the hash function should have nice statistical properties like *pairwise-independence*:

If ``x`` and ``y`` are different values, the chance that ``elsa<T>{}(x, seed) == elsa<T>{}(y, seed)``
should be very low for a random value of ``seed``.

Note that frozen always ultimately produces a perfect hash function, and you will always have ``O(1)``
lookup with frozen. It's just that if the input hasher performs poorly, the search will take longer and
your project will take longer to compile.

Troubleshooting
---------------

If you hit a message like this:

.. code:: none

    [...]
    note: constexpr evaluation hit maximum step limit; possible infinite loop?

Then either you've got a very big container and you should increase Clang's
thresholds, using ``-fconstexpr-steps=1000000000`` for instance, or the hash
functions used by frozen do not suit your data, and you should change them, as
in the following:

.. code:: c++

    struct olaf {
      constexpr std::size_t operator()(frozen::string const &value, std::size_t seed) const { return seed ^ value[0];}
    };

    constexpr frozen::unordered_set<frozen::string, 2, olaf/*custom hash*/> hans = { "a", "b" };

Credits
-------

The perfect hashing is strongly inspired by the blog post `Throw away the keys:
Easy, Minimal Perfect Hashing <http://stevehanov.ca/blog/index.php?id=119>`_.

Thanks a lot to Jérôme Dumesnil for his high-quality reviews!

Contact
-------

Serge sans Paille ``<sguelton@quarkslab.com>``

