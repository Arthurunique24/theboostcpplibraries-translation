#pragma once
template<class T>
class RandomAccess
{
	typedef RandomAccess<T> RAit;
public:
	RandomAccess(T *p)
		: _ptr(p)
	{
	}
	RandomAccess(const RAit &copy)
		: _ptr(copy._ptr)
	{
	}

	T *get() {
		return _ptr;
	}

	bool empty() {
		return _ptr == nullptr;
	}

	T &operator*()
	{
		return *_ptr;
	}
	const T &operator*() const
	{
		return *_ptr;
	}
	T &operator[](size_t n)
	{
		return *(_ptr + n);
	}
	const T &operator[](size_t n) const
	{
		return *(_ptr + n);
	}
	T *operator->()
	{
		return &this->operator*();
	}
	const T *operator->() const
	{
		return &this->operator*();
	}

	RAit &operator++()
	{
		++_ptr;
		return *this;
	}
	RAit operator++(int)
	{
		RAit temp(_ptr++);
		return temp;
	}
	RAit &operator--()
	{
		--_ptr;
		return *this;
	}
	RAit operator--(int)
	{
		RAit temp(_ptr--);
		return temp;
	}

	RAit &operator+=(int n)
	{
		_ptr += n;
		return *this;
	}
	//*** implemented with +=
	friend inline RAit &operator-=(RAit left, int n)
	{
		return left += -n;
	}
	friend inline RAit operator+(RAit left, int n)
	{
		RAit temp(left);
		return temp += n;
	}
	friend inline RAit operator-(RAit left, int n)
	{
		RAit temp(left);
		return temp += -n;
	}
	//***
	friend inline int operator-(RAit left, RAit right)
	{
		return left._ptr - right._ptr;
	}

	friend inline bool operator!=(RAit left, RAit right)
	{
		return left._ptr != right._ptr;
	}
	friend inline bool operator<(RAit left, RAit right)
	{
		return left._ptr < right._ptr;
	}
	//*** implemented with != and <
	friend inline bool operator==(RAit left, RAit right)
	{
		return !(left != right);
	}
	friend inline bool operator>(RAit left, RAit right)
	{
		return right < left;
	}
	friend inline bool operator<=(RAit left, RAit right)
	{
		return !(right < left);
	}
	friend inline bool operator>=(RAit left, RAit right)
	{
		return !(left < right);
	}
	//***
	~RandomAccess() = default;
private:
	T *_ptr;
};