#pragma once

#include <stdint.h>

namespace std
{
	typedef	uint8_t	  flag8_t;
	typedef	uint16_t  flag16_t;
	typedef	uint32_t  flag32_t;
	typedef	uint64_t  flag64_t;



	// ������� ���� �� ���� ���������
	template<typename flag_t>
	inline void clear_flag(flag_t& flag) { flag = (flag_t)0; }

	// ��������� ��������� � ���� flag_t
	template<typename type_flag_t, typename flag_t>
	inline void add_flag(type_flag_t& flag, const flag_t& add) { flag |= (type_flag_t)add; }

	// ������� ��������� �� �����
	template<typename type_flag_t, typename flag_t>
	inline void del_flag(type_flag_t& flag, const flag_t& del) { flag &= ~(type_flag_t)del; }

	// ��������� ��������� � �����, ���� ��� ����������, ���������� true
	template<typename type_flag_t, typename flag_t>
	inline bool check_flag(const type_flag_t& flag, const flag_t& check_flag) { return (flag & (type_flag_t)check_flag); }

	// ��������� ��� ������ ���� ���� � ������ ������ ����������
	template<typename type_flag_t, typename flag_t>
	inline bool only_flag(const type_flag_t& flag, const flag_t& check_flag) {
		type_flag_t tmp_flag = 0;
		add_flag(tmp_flag, check_flag);
		return tmp_flag == flag;
	}

	// ������� ������� ���������� �����
	// ��� ����� ��������� �������� � enum � ��������� ��������� max
	// ����� �������� ��� � ���������� max - 1 � �������� ����� � �� ������ �������
	// ������ � ������������
	template<typename flag_t>
	void max_flag(const flag_t& flag, int& position) {
		flag64_t max = (flag64_t)flag;
		for (position = position; position < 64; position++)
			if (max == ((flag64_t)1 << (flag64_t)position))
				return;
		position = 0;
	}

	inline const flag64_t number_flag(const int& position) {
		return ((flag64_t)1 << (flag64_t)position);
	}
}