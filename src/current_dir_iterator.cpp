#include "current_dir.hpp"

using iterator = current_dir::iterator;

iterator::iterator()
	: m_dirs(), m_regs(), m_pos(0)
{
}

iterator::iterator(const underlying_vec& dirs, const underlying_vec& regs, std::size_t pos)
	: m_dirs(dirs), m_regs(regs), m_pos(pos)
{
}

const file& iterator::operator*() const
{
	auto num_of_dirs = m_dirs.get().size();
	return (m_pos < num_of_dirs) ? m_dirs.get()[m_pos] : m_regs.get()[m_pos-num_of_dirs];
}

const file& iterator::operator[](int offset) const
{
	auto num_of_dirs = m_dirs.get().size();
	return (m_pos+offset < num_of_dirs) ? m_dirs.get()[m_pos+offset] : m_regs.get()[m_pos+offset-num_of_dirs];
}

const file* iterator::operator->() const
{
	auto num_of_dirs = m_dirs.get().size();
	return (m_pos < num_of_dirs) ? &m_dirs.get()[m_pos] : &m_regs.get()[m_pos-num_of_dirs];
}

iterator::operator bool() const
{
	return m_dirs && m_regs;
}

// ++it
iterator& iterator::operator++()
{
	++m_pos;
	return *this;
}

// it++
iterator iterator::operator++(int)
{
	iterator tmp = *this;
	++m_pos;
	return tmp;
}

// --it
iterator& iterator::operator--()
{
	--m_pos;
	return *this;
}

// it--
iterator iterator::operator--(int)
{
	iterator tmp = *this;
	--m_pos;
	return tmp;
}

// it + offset
iterator iterator::operator+(int offset) const
{
	iterator tmp = *this;
	tmp.m_pos += offset;
	return tmp;
}

// it - offset
iterator iterator::operator-(int offset) const
{
	iterator tmp = *this;
	tmp.m_pos -= offset;
	return tmp;
}

iterator& iterator::operator+=(int offset)
{
	m_pos += offset;
	return *this;
}

iterator& iterator::operator-=(int offset)
{
	m_pos -= offset;
	return *this;
}

int iterator::operator-(const iterator& other) const
{
	return m_pos-other.m_pos;
}

bool iterator::operator==(const iterator& other) const
{
	return (m_pos == other.m_pos) && (m_dirs == other.m_dirs) && (m_regs == other.m_regs); 
}

bool iterator::operator!=(const iterator& other) const 
{
	return !(*this == other);
}

bool iterator::operator<(const iterator& other) const
{
	return m_pos < other.m_pos;
}

bool iterator::operator>(const iterator& other) const
{
	return m_pos > other.m_pos;
}

bool iterator::operator>=(const iterator& other) const
{
	return m_pos >= other.m_pos;
}

bool iterator::operator<=(const iterator& other) const
{
	return m_pos <= other.m_pos;
}

