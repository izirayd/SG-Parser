/* init var with use template */
template<class T> constexpr T pi = T(3.1415926535897932385L);


template < int i = 3 > 4 > // syntax error
class X { /* ... */ };

template<int i = (3 > 4) > // OK
class Y { /* ... */ };

template <class T = float> struct B {};
template <template <class TT = float> class T> struct A {
	inline void f();
	inline void g();
};
template <template <class TT> class T> void A<T>::f() {
	T<> t; // error - TT has no default template argument
}
template <template <class TT = char> class T> void A<T>::g() {
	T<> t; // OK - T<char>
}

template <class T>
class char_traits
{

};

template <class T>
class allocator
{

};

template<class T> struct string {
	template<class T2> int compare(const T2&);
	template<class T2> string(const string<T2>& s) { /* ... */ }
};

template<class T>
template<class T2>
int string<T>::compare(const T2& s) {}

namespace std {
	template <class element_t, class traits_t = char_traits<element_t>, class allocator_t = allocator<element_t>>
	class basic_string {
		class a {};
		template <class test_t> class b {};
	};
}

template <class T>
class lobster_t
{
	using loblob = T;
};

template <class noob_t, class loblob = lobster_t<noob_t>::loblob>
class fruit
{
public:
	inline T data;
};

class apple_t
{
};

/*
  компилятор Visual stidio не может это спарсить лол
*/
//template <typename T>
//class a_t
//{
//public:
//	class null_1_t {
//	public:
//		class b_t
//		{
//		public:
//			T data;
//		};
//	};
//
//	class c_t
//	{
//	public:
//		null_1_t::b_t b;
//	};
//};


template <typename T>
class a_t
{
public:

	class b_t
	{
	public:
		T data;
	};

	class c_t
	{
	public:
		b_t b;
	};
};

void main()
{
	fruit<apple_t> apple;
}