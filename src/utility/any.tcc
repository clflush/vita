/**
 *  \file
 *  \remark This file is part of VITA.
 *
 *  \copyright Copyright (C) 2014-2017 EOS di Manlio Morini.
 *
 *  \license
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this file,
 *  You can obtain one at http://mozilla.org/MPL/2.0/
 */

#if !defined(VITA_ANY_H)
#  error "Don't include this file directly, include the specific .h instead"
#endif

#if !defined(VITA_ANY_TCC)
#define      VITA_ANY_TCC

namespace detail {
namespace any_ {

// Function pointer table.
struct fxn_ptr_table
{
  const std::type_info &(*get_type)();
  void (*static_delete)(void **) noexcept;
  void (*destruct)(void **);
  void (*clone)(void *const *, void **);
  void (*move)(void *const *, void **);
  std::size_t (*hash)(void *const *);
  std::istream &(*stream_in)(std::istream &, void **);
  std::ostream &(*stream_out)(std::ostream &, void *const *);
};

// Static functions for small value-types
template<> struct fxns<true>
{
  template<class T>
  struct type
  {
    static constexpr const std::type_info &get_type() { return typeid(T); }

    static void static_delete(void **x) noexcept
    { reinterpret_cast<T *>(x)->~T(); }

    static void destruct(void **x) { reinterpret_cast<T *>(x)->~T(); }

    static void clone(void *const *src, void **dest)
    {
      new (dest) T(*reinterpret_cast<const T *>(src));
    }

    static void move(void *const *src, void **dest)
    {
      *reinterpret_cast<T *>(dest) = *reinterpret_cast<const T *>(src);
    }

    static std::size_t hash(void *const *obj)
    {
      return std::hash<T>()(*reinterpret_cast<const T *>(obj));
    }

    static std::istream &stream_in(std::istream &i, void **obj)
    {
      i >> *reinterpret_cast<T *>(obj);
      return i;
    }

    static std::ostream &stream_out(std::ostream &o, void *const *obj)
    {
      o << *reinterpret_cast<const T *>(obj);
      return o;
    }
  };  // struct type
};  // struct fxns<true>

// Static functions for big value-types (bigger than a `void *`)
template<>
struct fxns<false>
{
  template<class T>
  struct type
  {
    static constexpr const std::type_info &get_type() { return typeid(T); }

    static void static_delete(void **x) noexcept
    {
      // destruct and free memory
      delete (*reinterpret_cast<T **>(x));
    }

    static void destruct(void **x)
    {
      // destruct only, we'll reuse memory
      (*reinterpret_cast<T **>(x))->~T();
    }

    static void clone(void *const *src, void **dest)
    {
      *dest = new T(**reinterpret_cast<T *const *>(src));
    }

    static void move(void *const *src, void **dest)
    {
      **reinterpret_cast<T **>(dest) = **reinterpret_cast<T *const *>(src);
    }

    static std::size_t hash(void *const *obj)
    {
      return std::hash<T>()(**reinterpret_cast<T *const *>(obj));
    }

    static std::istream &stream_in(std::istream &i, void **obj)
    {
      i >> **reinterpret_cast<T **>(obj);
      return i;
    }

    static std::ostream &stream_out(std::ostream &o, void *const *obj)
    {
      o << **reinterpret_cast<T *const *>(obj);
      return o;
    }
  };  // struct type
};  // struct fxns<false>

template<class T>
struct get_table
{
  static constexpr bool is_small = sizeof(T) <= sizeof(void *);
  using is_small_t = std::integral_constant<bool, is_small>;

  static fxn_ptr_table *get() noexcept
  {
    static fxn_ptr_table static_table =
    {
      fxns<is_small>::template type<T>::get_type,
      fxns<is_small>::template type<T>::static_delete,
      fxns<is_small>::template type<T>::destruct,
      fxns<is_small>::template type<T>::clone,
      fxns<is_small>::template type<T>::move,
      fxns<is_small>::template type<T>::hash,
      fxns<is_small>::template type<T>::stream_in,
      fxns<is_small>::template type<T>::stream_out
    };

    return &static_table;
  }
};  // class get_table


inline std::istream &operator>>(std::istream &i, empty &)
{
  // If this assertion fires you tried to insert from a std::istream
  // into an empty `hold_any` instance. This simply can't work, because
  // there is no way to figure out what type to extract from the
  // stream.
  // The only way to make this work is to assign an arbitrary
  // value of the required type to the hold_any instance you want to
  // stream to. This assignment has to be executed before the actual
  // call to the operator>>().
  assert(false);

  return i;
}

inline std::ostream &operator<<(std::ostream &o, const empty &)
{
  return o;
}

template<> constexpr const std::type_info &fxns<true>::type<empty>::get_type()
{
  return typeid(void);
}

}}  // namespace detail::any_

///
/// Constructs an empty object.
///
/// \post `this->empty()`
///
inline any::any() noexcept
  : table(detail::any_::get_table<detail::any_::empty>::get())
{
}

///
/// Copy constructor that copies content of `x` into new instance.
///
/// \param[in] x content to be copied `x`
///
/// Content is equivalent in both type and value to the content of `x` or empty
/// if `x` is empty.
///
inline any::any(const any &x)
  : table(detail::any_::get_table<detail::any_::empty>::get())
{
  assign(x);
}

///
/// \param[in] x any will contain object `x`
///
/// Makes a copy of `x`, so that the initial content of the new instance is
/// equivalent in both type and value to `x`.
///
template<class T>
inline any::any(const T &x)
  : any(x,  // a tag dispatch solution
        static_cast<typename detail::any_::get_table<T>::is_small_t *>(nullptr))
{
}

template<class T>
any::any(const T &x, std::true_type *)
  : table(detail::any_::get_table<T>::get())
{
  new (&object) T(x);
}

template<class T>
any::any(const T &x, std::false_type *)
  : table(detail::any_::get_table<T>::get()), object(new T(x))
{
}

///
/// Releases any and all resources used in management of instance.
///
inline any::~any()
{
  table->static_delete(&object);
}

inline any &any::assign(const any &x)
{
  if (&x != this)
  {
    // Are we copying between the same type?
    if (table == x.table)  // if so, we can avoid reallocation
      table->move(&x.object, &object);
    else
    {
      reset();
      x.table->clone(&x.object, &object);
      table = x.table;
    }
  }

  return *this;
}

template<class T>
any &any::assign(const T &x)
{
  auto *x_table(detail::any_::get_table<T>::get());

  // Are we copying between the same type?
  if (table == x_table)
  {
    // If so, we can avoid deallocating and re-use memory.
    table->destruct(&object);    // first destruct the old content
    if (detail::any_::get_table<T>::is_small)
      new (&object) T(x);  // create copy on-top of object pointer itself
    else
      new (object) T(x);   // create copy on-top of old version
  }
  else
  {
    if (detail::any_::get_table<T>::is_small)
    {
      // Create copy on-top of object pointer itself.
      table->destruct(&object);  // first destruct the old content
      new (&object) T(x);
    }
    else
    {
      reset();  // first delete the old content
      object = new T(x);
    }

    table = x_table;      // update table pointer
  }

  return *this;
}

///
/// Assigns the type and values of `rhs`.
///
/// \param[in] rhs new any value
/// \return        `this` object after assignment
///
/// Forwards `rhs`, discarding previous content, so that the new content of is
/// equivalent in both type and value to `rhs` before forward.
///
template <class T>
any &any::operator=(T &&rhs)
{
  return assign(std::forward<T>(rhs));
}

///
/// Queries the contained type.
///
/// \return the typeid of the contained value if instance is non-empty,
///         otherwise typeid(void).
///
/// \note
/// Useful for querying against types known either at compile time or only
/// at runtime.
///
inline const std::type_info &any::type() const noexcept
{
  return table->get_type();
}

///
/// Checks whether the object contains a value.
///
/// \return `true` if instance contains a value, otherwise `false`.
///
inline bool any::has_value() const noexcept
{
  return table != detail::any_::get_table<detail::any_::empty>::get();
}

///
/// Resets the content of the any.
///
inline void any::reset() noexcept
{
  if (has_value())
  {
    table->static_delete(&object);
    table = detail::any_::get_table<detail::any_::empty>::get();
    object = nullptr;
  }
}

// This function has been added in the assumption that the embedded
// type has a corresponding operator defined.
inline std::istream &operator>>(std::istream& i, any &obj)
{
  return obj.table->stream_in(i, &obj.object);
}

// This function has been added in the assumption that the embedded
// type has a corresponding operator defined.
inline std::ostream &operator<<(std::ostream& o, const any &obj)
{
  return obj.table->stream_out(o, &obj.object);
}

///
/// Custom keyword cast for extracting a value of a given type from any.
///
/// \param[in] operand a pointer to same `any`
/// \return            a similarly qualified pointer to the value content if
///                    successful, otherwise null is returned
///
template<class T>
T *any_cast(any *operand) noexcept
{
  if (operand && operand->type() == typeid(T))
    return detail::any_::get_table<T>::is_small ?
      reinterpret_cast<T *>(&operand->object) :
      reinterpret_cast<T *>(operand->object);

  return nullptr;
}

///
/// Custom keyword cast for extracting a value of a given type from any.
///
/// \param[in] operand a pointer to some `any`
/// \return            a similarly qualified pointer to the value content if
///                    successful, otherwise null is returned
///
template<class T>
const T *any_cast(const any *operand) noexcept
{
  return any_cast<T>(const_cast<any *>(operand));
}

///
/// Custom keyword cast for extracting a value of a given type from any.
///
/// \param[in] operand an any
/// \return            the value contained in `operand`
///
template<class T>
T any_cast(any &operand)
{
  using U = std::remove_cv_t<std::remove_reference_t<T>>;

  auto *result = any_cast<U>(&operand);
  if (!result)
    throw bad_any_cast(operand.type(), typeid(T));

  return static_cast<T>(*result);
}

///
/// Custom keyword cast for extracting a value of a given type from any.
///
/// \param[in] operand an any
/// \return            a constant reference to the value contained in `operand`
///
template<class T>
T any_cast(const any &operand)
{
  return any_cast<T>(const_cast<any &>(operand));
}

#endif  // include guard
