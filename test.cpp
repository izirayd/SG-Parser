template <class T>
class char_traits
{

};

template <class T>
class allocator
{

};

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

void main()
{
	fruit<apple_t> apple;
}