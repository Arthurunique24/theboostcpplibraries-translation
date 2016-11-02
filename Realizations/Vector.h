#pragma once

#include <initializer_list>
#include "RandomAccessIterator.h"



template<class T, class Alloc = std::allocator<T>>
class Vector
{
public:
	typedef T value_type;
	typedef size_t size_type;
	typedef RandomAccess<value_type> iterator;
	typedef RandomAccess<value_type> const_iterator;
	typedef Alloc allocator_type;
	typedef value_type &reference;
	typedef const value_type &const_reference;

	static allocator_type Allocator() {
		return allocator_type();
	}
	allocator_type &get_allocator() const {
		return al;
	}

	explicit Vector(const allocator_type& alloc = Allocator())
		: al(alloc), _size(0), _capacity(0), _data(nullptr)
	{
	}
	explicit Vector(size_type n)
		: _size(n), _capacity(_size), _data(al.allocate(capacity()))
	{
		for (size_type i = 0; i < size(); ++i)
			al.construct(_data + i);
	}
	explicit Vector(size_type n, const value_type &val, const allocator_type& alloc = Allocator())
		: al(alloc), _size(n), _capacity(_size), _data(al.allocate(capacity()))
	{
		for (size_type i = 0; i < size(); ++i)
			al.construct(_data + i, val);
	}
	Vector(const Vector &copy)
		: _size(copy._size), _capacity(_size), _data(al.allocate(capacity()))
	{
		for (size_type i = 0; i < size(); ++i)
			al.construct(_data + i, copy._data[i]);
	}
	Vector(const Vector &copy, const allocator_type& alloc)
		: al(alloc), _size(copy._size), _capacity(_size), _data(al.allocate(capacity()))
	{
		for (int i = 0; i < size(); ++i)
			al.construct(_data + i, copy._data[i]);
	}
	Vector(std::initializer_list<T> lst, const allocator_type& alloc = Allocator())
		: al(alloc), _size(lst.size()), _capacity(_size), _data(al.allocate(capacity()))
	{
		for (auto it = lst.begin(); it != lst.end(); ++it)
			al.construct(_data + (it - lst.begin()), *it);
	}
	Vector(iterator beg, iterator end, const allocator_type& alloc = Allocator())
		: al(alloc), _size(end - beg), _capacity(_size), _data(al.allocate(capacity()))
	{
		value_type *ptr = _data;
		for (auto it = beg; it != end; ++it)
			al.construct(ptr++, *it);
	}
	Vector(Vector&& other) 
		: _size(other._size), _capacity(_size), _data(other._data)
	{
		other._size = other._capacity = 0;
		other._data = nullptr;
	}
	Vector(Vector&& other, const allocator_type& alloc)
		: al(alloc), _size(other._size), _capacity(_size), _data(other._data)
	{
		auto otherAl = other.get_allocator();
		if (alloc != otherAl) {
			_data = al.allocate(_capacity);
			for (size_type i = 0; i < size(); ++i) {
				al.construct(_data + i, other._data[i]);
				otherAl.destroy(other._data + i);
			}
			other.get_allocator().deallocate(other._data, other._capacity);
		}
		other._size = other._capacity = 0;
		other._data = nullptr;
	}
	~Vector()
	{
		deallocate(_data, size(), capacity());
	}

	Vector &operator=(const Vector &right)
	{
		if (this == &right)
			return *this;
		deallocate(_data, size(), capacity());
		_size = right._size;
		_capacity = right._capacity;
		_data = al.allocate(capacity());
		for (int i = 0; i < size(); ++i)
			al.construct(_data + i, right._data[i]);
		return *this;
	}
	Vector &operator=(Vector &&right) {
		(*this).swap(right);
		return *this;
	}
	void assign(size_type n, const value_type& val)
	{
		*this = Vector(n, val);
	}
	void assign(iterator beg, iterator end)
	{
		*this = Vector(beg, end);
	}

	reference at(size_type index)
	{
		return const_cast<reference>(static_cast<const Vector*>(this)->at(index));
	}
	const_reference at(size_type index) const
	{
		if (index >= size() || index < 0)
			throw std::out_of_range("Invalid index");
		return _data[index];
	}
	reference operator[](size_type index)
	{
		return const_cast<reference>(static_cast<const Vector*>(this)->operator[](index));
	}
	const_reference operator[](size_type index) const
	{
		return _data[index];
	}
	reference front()
	{
		return const_cast<reference>(static_cast<const Vector*>(this)->front());
	}
	const_reference front() const
	{
		return _data[0];
	}
	reference back()
	{
		return const_cast<reference>(static_cast<const Vector*>(this)->back());
	}
	const_reference back() const
	{
		return _data[_size - 1];
	}
	value_type *data() noexcept
	{
		return const_cast<value_type *>(static_cast<const Vector*>(this)->data());
	}
	const value_type *data() const noexcept
	{
		return _data;
	}

	iterator begin() noexcept
	{
		return iterator(_data);
	}
	const_iterator begin() const noexcept
	{
		return const_iterator(_data);
	}
	const_iterator cbegin() const noexcept
	{
		return const_iterator(_data);
	}
	iterator end() noexcept
	{
		return iterator(_data + _size);
	}
	const_iterator end() const noexcept
	{
		return const_iterator(_data + _size);
	}
	const_iterator cend() const noexcept
	{
		return const_iterator(_data + _size);
	}

	bool empty() const noexcept
	{
		return size() == 0;
	}
	size_type size() const noexcept
	{
		return _size;
	}
	size_type max_size() const noexcept
	{
		return std::numeric_limits<size_type>::max();
	}
	void reserve(size_type capacity)
	{
		if (capacity > max_size())
			throw std::length_error("Vector can't be so big");
		reallocate(capacity);
	}
	size_type capacity() const noexcept
	{
		return _capacity;
	}
	void shrink_to_fit()
	{
		if (size() != capacity())
		{
			value_type *temp = _data;
			_capacity = size();
			_data = al.allocate(capacity());
			for (size_type i = 0; i < size(); ++i)
				al.construct(_data + i, temp[i]);
			deallocate(temp, size(), capacity());
		}
	}

	void clear() noexcept
	{
		for (size_type i = 0; i < size(); ++i)
			al.destroy(ptr + i);
		_size = 0;
	}
	iterator insert(iterator pos, size_type count, const T &val)
	{
		if (count <= 0)
			return pos;
		value_type *temp = _data;
		while (size() + count > capacity())
		{
			_data = new value_type[_capacity *= 2];
			for (iterator it1(temp), it2(_data); it1 != pos;)
				*it2++ = *it1++;
		}
		iterator itData(_data + (_size - 1 + count)), itTemp(temp + (_size - 1));
		_size += count;
		while ((itTemp + 1) != pos)
			*itData-- = *itTemp--;
		for (size_type i = 0; i < count; ++i)
			*itData-- = val;
		return ++itData;
	}
	iterator insert(iterator pos, const T &val)
	{
		return insert(pos, 1, val);
	}
	iterator insert(iterator pos, iterator beg, iterator end)
	{
		value_type *temp = _data;
		while (size() + (end - beg) > capacity())
		{
			_data = new value_type[_capacity *= 2];
			for (iterator it1(temp), it2(_data); it1 != pos;)
				*it2++ = *it1++;
		}
		iterator itData(_data + (_size - 1 + (end - beg))), itTemp(temp + (_size - 1));
		_size += end - beg;
		while ((itTemp + 1) != pos)
			*itData-- = *itTemp--;
		for (auto it = end; it != beg; --it)
			*itData-- = *(it - 1);
		return ++itData;
	}
	iterator insert(iterator pos, std::initializer_list<value_type> lst)
	{
		value_type *temp = _data;
		while (size() + lst.size() > capacity())
		{
			_data = new value_type[_capacity *= 2];
			for (iterator it1(temp), it2(_data); it1 != pos;)
				*it2++ = *it1++;
		}
		iterator itData(_data + (_size - 1 + lst.size())), itTemp(temp + (_size - 1));
		_size += lst.size();
		while ((itTemp + 1) != pos)
			*itData-- = *itTemp--;
		for (auto it = lst.end(); it != lst.begin(); --it)
			*itData-- = *(it - 1);
		return ++itData;
	}
	iterator erase(const_iterator pos)
	{
		return erase(pos, pos + 1);
	}
	iterator erase(const_iterator first, const_iterator last) {
		auto res = first;
		auto difference = last - first;
		while (last != end())
			*first++ = *last++;
		while (first != last)
			al.destroy((first++).get());
		_size -= difference;
		return res;
	}
	template< class... Args >
	void emplace_back(Args&&... args) {
		reallocate(size() + 1);
		al.construct(_data + _size++, args...);
	}
	void resize(size_type count) {
		resize(count, value_type());
	}
	void resize(size_type count, const value_type &value) {
		if (size() >= count) {
			for (size_type i = count; i < size(); ++i)
				al.destroy(_data + i);
		}
		else {
			reallocate(count);
			for (size_type i = size(); i < count; ++i)
				al.construct(_data + i, value);
		}
		_size = count;
	}
	void push_back(const value_type &val)
	{
		reallocate(size() + 1);
		al.construct(_data + _size++, val);
	}
	void push_back(value_type &&val)
	{
		reallocate(size() + 1);
		al.construct(_data + _size++, std::move(val));
	}
	void pop_back()
	{
		if (size() > 0)
			al.destroy(_data + --_size);
	}
	void swap(Vector &other) noexcept {
		using std::swap;
		swap(_size, other._size);
		swap(_capacity, other._capacity);
		swap(_data, other._data);
	}
	friend void swap(Vector &lhs, Vector &rhs) noexcept {
		lhs.swap(rhs);
	}

	friend inline bool operator!=(const Vector &lhs, const Vector &rhs)
	{
		if (lhs.size() != rhs.size())
			return true;
		for (size_type i = 0; i < lhs.size(); ++i)
			if (lhs[i] != rhs[i])
				return true;
		return false;
	}
	friend inline bool operator<(const Vector &lhs, const Vector &rhs)
	{
		size_type lsize = lhs.size(), rsize = rhs.size();
		size_type minSize = lsize < rsize ? lsize : rsize;
		for (size_type i = 0; i < minSize; ++i)
			if (lhs[i] >= rhs[i])
				return false;
		if (lsize < rsize)
			return true;
		else
			return false;
	}
	friend inline bool operator==(const Vector &lhs, const Vector &rhs)
	{
		return !(lhs != rhs);
	}
	friend inline bool operator>(const Vector &lhs, const Vector &rhs)
	{
		return rhs < lhs;
	}
	friend inline bool operator<=(const Vector &lhs, const Vector &rhs)
	{
		return !(rhs < lhs);
	}
	friend inline bool operator>=(const Vector &lhs, const Vector &rhs)
	{
		return !(lhs < rhs);
	}
	
	
private:
	allocator_type al;
	size_type _size;
	size_type _capacity;
	value_type *_data;

	size_type growTo(size_type count) const
	{
		size_type cap = capacity();
		cap = max_size() - cap / 2 < cap
			? 0 : cap + cap / 2;
		if (cap < count)
			cap = count;
		return cap;
	}
	void reallocate(size_type newCapacity) {
		size_type oldCap = capacity();
		if (newCapacity <= oldCap)
			return;
		value_type *temp = _data;
		_capacity = growTo(newCapacity);
		_data = al.allocate(capacity());
		for (size_type i = 0; i < size(); ++i)
			al.construct(_data + i, temp[i]);
		deallocate(temp, size(), oldCap);
	}
	void deallocate(value_type *ptr, size_type size, size_type capacity) {
		for (size_type i = 0; i < size; ++i)
			al.destroy(ptr + i);
		al.deallocate(ptr, capacity);
	}
};