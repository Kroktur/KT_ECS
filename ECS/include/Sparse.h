#pragma once
#include <type_traits>
#include <vector>
namespace KT
{
	enum class Storage : std::uint8_t
	{
		SPARSE,
		DENSE
	};
	template<typename type, std::make_unsigned_t<type> allocatorFactor = 1> requires (std::is_arithmetic_v<type>)
	class SparseSet
	{
	public:
		static_assert(allocatorFactor != 0, "need at least 1");
		using base_type = type;
		using signed_type = std::make_signed_t<base_type>;
		using unsigned_type = std::make_unsigned_t<base_type>;
		using container_sparse_type =  std::vector<signed_type>;
		using container_dense_type = std::vector<unsigned_type>;
		using const_iterator_type =  container_dense_type::const_iterator;
		static constexpr signed_type invalidValue = static_cast<signed_type>(-1);
	public:
		SparseSet() = default;
		~SparseSet() = default;
		SparseSet(const SparseSet&) = default;
		SparseSet(SparseSet&&) noexcept = default;
		SparseSet& operator=(const SparseSet&) = default;
		SparseSet& operator=(SparseSet&&) noexcept = default;
	public:
		bool Has(unsigned_type e) const ;
		void Add(unsigned_type e);
		void Remove(unsigned_type e);

		template<Storage storage>
		unsigned_type Size() const;
		template<Storage storage>
		unsigned_type  Capacity() const;
		template<Storage storage>
		void Reserve(unsigned_type size);

		template<Storage storage>requires (storage == Storage::DENSE)
		const unsigned_type& At(unsigned_type index) const;
		template<Storage storage>requires (storage == Storage::SPARSE)
		const signed_type& At(unsigned_type index) const;

	
		const container_dense_type& Dense() const;
		const_iterator_type begin() const;
		const_iterator_type end() const;
	private:
		void ResizeSparse(unsigned_type size);
		container_sparse_type m_sparse;
		container_dense_type m_dense;
	};

	template <typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::is_arithmetic_v<type>)
	bool SparseSet<type, allocatorFactor>::Has(unsigned_type e) const 
	{
		return m_sparse.size() > e && m_sparse[e] != invalidValue;

	}

	template <typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::is_arithmetic_v<type>)
	void SparseSet<type, allocatorFactor>::Add(unsigned_type e)
	{
		if (m_sparse.size() <= e)
		{
			if (e > std::numeric_limits<signed_type>::max() / allocatorFactor)
				ResizeSparse(std::numeric_limits<signed_type>::max());
			else
				ResizeSparse((e + 1) * allocatorFactor);
		}
		
		m_sparse[e] = static_cast<signed_type>(m_dense.size());
		m_dense.push_back(e);
	}

	template <typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::is_arithmetic_v<type>)
	void SparseSet<type, allocatorFactor>::Remove(unsigned_type e)
	{
		signed_type index = m_sparse[e];
		unsigned_type lastEntity = m_dense.back();

		m_dense[index] = lastEntity;
		m_dense.pop_back();

		m_sparse[lastEntity] = index;
		m_sparse[e] = invalidValue;
	}

	

	template <typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::is_arithmetic_v<type>)
	const typename SparseSet<type, allocatorFactor>::container_dense_type& SparseSet<type, allocatorFactor>::
	Dense() const
	{
		return m_dense;
	}

	template <typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::is_arithmetic_v<type>)
	typename SparseSet<type, allocatorFactor>::const_iterator_type SparseSet<type, allocatorFactor>::begin() const
	{
		return m_dense.begin();
	}

	template <typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::is_arithmetic_v<type>)
	typename SparseSet<type, allocatorFactor>::const_iterator_type SparseSet<type, allocatorFactor>::end() const
	{
		return m_dense.end();
	}

	template <typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::is_arithmetic_v<type>)
	void SparseSet<type, allocatorFactor>::ResizeSparse(unsigned_type size)
	{
		m_sparse.resize(size, invalidValue);
	}

	template <typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::is_arithmetic_v<type>)
	template <typename ::KT::Storage storage>
	void SparseSet<type, allocatorFactor>::Reserve(
		unsigned_type size)
	{
		if constexpr (storage == Storage::DENSE)
		{
			m_dense.reserve(size);
		}
		else
		{
			m_sparse.reserve(size);
		}

	}

	template <typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::is_arithmetic_v<type>)
	template <Storage storage> requires (storage == Storage::DENSE)
	const typename SparseSet<type, allocatorFactor>::unsigned_type& SparseSet<type, allocatorFactor>::At(
		unsigned_type index) const
	{
		return m_dense.at(index);
	}

	template <typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::is_arithmetic_v<type>)
	template <Storage storage> requires (storage == Storage::SPARSE)
	const typename SparseSet<type, allocatorFactor>::signed_type& SparseSet<type, allocatorFactor>::At(
		unsigned_type index) const
	{
		return m_sparse.at(index);
	}

	template <typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::is_arithmetic_v<type>)
	template <typename ::KT::Storage storage>
	typename SparseSet<type, allocatorFactor>::unsigned_type SparseSet<type, allocatorFactor>::Capacity() const
	{
		if constexpr (storage == Storage::DENSE)
			return static_cast<unsigned_type>(m_dense.capacity());
		else
			return static_cast<unsigned_type>(m_sparse.capacity());
	}

	template <typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::is_arithmetic_v<type>)
	template <typename ::KT::Storage storage>
	typename SparseSet<type, allocatorFactor>::unsigned_type SparseSet<type, allocatorFactor>::Size() const
	{
		if constexpr (storage == Storage::DENSE)
			return static_cast<unsigned_type>(m_dense.size());
		else
			return static_cast<unsigned_type>(m_sparse.size());
	}
}



