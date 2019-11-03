typedef struct X {
	int a;
	int b;
} test;

typedef struct A {};
typedef struct {};
typedef struct {} B;
typedef struct {} C1, C2, C3;
typedef struct L {} LC1, LC2, LC3;

typedef struct Y {
	int a;
	int b;
} type1, type2, type3;

typedef class  class1  class1;
typedef struct struct1 struct1;
typedef union  union1  union1;
typedef enum   enum1   enum1;

/*Typedef tests*/
typedef const int& new_int;

namespace test
{
	typedef  int new_int;
}

typedef const volatile test::new_int* const_int;
typedef const std::big::name::space::lol::string& more_str;
typedef const unsigned int* intintint;
typedef volatile const unsigned long long int* wtf_int;
typedef volatile const unsigned long long int* qw, qe, qf, qr;

template <typename T>
class apples_t
{

};

typedef apples_t<int> new_apples_int;

apples_t<int> apple_var;

template< class T>
struct add_const {
	typedef const T type;
};

template <typename T>
class base_apple_t {
	T data;
};



template <typename T, typename A>
class apple_t : public base_apple_t<T>, private base_color_t<A> {
	T value;
	A avalue;
};

template <typename T>
class base_color_t {
	T data_color;
};