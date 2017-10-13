Frozen
######

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

    using namespace frozen::string_literals; // for the ""_s operator

    constexpr frozen::unordered_map<frozen::string, int, 2> olaf = {
        {"19"_s, 19},
        {"31"_s, 31},
    };
    constexpr auto val = olaf.at("19"_s);

Extending
---------

Just like the regular C++14 container, you can specialize the hash function,
the key equality comparator for ``unordered_*`` containers, and the comparison
functions for the ordered version.

It's also possible to specialize the ``frozen::elsa`` structure used for
hashing. Note that two hashing algorithm are used: one for initial hashing and
one to resolve conflicts.

.. code:: C++

    template <class T> struct elsa {

      // used for first hash try
      constexpr std::size_t operator()(T const &value) const;

      // incase of conflict, this one is used to resolve conflicts, iterating over the seed
      constexpr std::size_t operator()(T const &value, std::size_t seed) const;
    };

Troubleshooting
---------------

If you hit a message like this:

.. code::

    [...]
    note: constexpr evaluation hit maximum step limit; possible infinite loop?

Then either you've got a very big container and you should increase Clang's
thresholds, using ``-fconstexpr-steps=1000000000`` for instance, or the hash
functions used by frozen do not suit your data, and you should change them, as
in the following:

.. code:: c++

    struct olaf {

      constexpr std::size_t operator()(frozen::string const &value) const { return value.size; }

      constexpr std::size_t operator()(frozen::string const &value, std::size_t seed) const { return seed ^ value[0];}
    };

    constexpr frozen::unordered_set<frozen::string, 2, olaf/*custom hash*/> hans = { "a"_s, "b"_s };

Credits
-------

The perfect hashing is strongly inspired by the blog post `Throw away the keys:
Easy, Minimal Perfect Hashing <http://stevehanov.ca/blog/index.php?id=119>`_.

Thanks a lot to Jérôme Dumesnil for his high-quality reviews!

Contact
-------

Serge sans Paille ``<sguelton@quarkslab.com>``

