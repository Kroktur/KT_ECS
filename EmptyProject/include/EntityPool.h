#pragma once
#include <type_traits>

#include "Sparse.h"

namespace KT
{
	namespace ECS
	{
		template<typename type, std::make_unsigned_t<type> allocatorFactor = 1, std::make_unsigned_t<type> offsetSize = 1000> requires (std::is_arithmetic_v<type>)
		class EntityPool
		{
		public:
			static_assert(offsetSize >= 10, "need at least 10");
			using sparse_type = SparseSet<type, allocatorFactor>;
			using base_type = sparse_type::base_type;
			using unsigned_type = sparse_type::unsigned_type;
			using signed_type = sparse_type::signed_type;
			using container_type = std::vector<unsigned_type>;
			static_assert(offsetSize < std::numeric_limits<signed_type>::max(), "offset need to be smaller");
		public:
			EntityPool();
			~EntityPool() = default;
			EntityPool(const EntityPool&) = default;
			EntityPool(EntityPool&&) noexcept = default;
			EntityPool& operator=(const EntityPool&) = default;
			EntityPool& operator=(EntityPool&&) noexcept = default;
		public:
			bool HasEntity(unsigned_type e) const;
			unsigned_type CreateEntity();
			void RemoveEntity(unsigned_type e);
			unsigned_type Size() const;
			const container_type& GetEntities() const;
		private: 
			void FilledWithIds();
			signed_type m_lastEntity;
			sparse_type m_sparse;
			container_type m_freeId;
		};

		template <typename type, std::make_unsigned_t<type> allocatorFactor, std::make_unsigned_t<type> offsetSize> requires (std::is_arithmetic_v<type>)
		EntityPool<type, allocatorFactor,offsetSize>::EntityPool() : m_lastEntity( static_cast<signed_type>(0))
		{

		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor, std::make_unsigned_t<type> offsetSize> requires (std::is_arithmetic_v<type>)
		bool EntityPool<type, allocatorFactor,offsetSize>::HasEntity(unsigned_type e) const
		{
			return m_sparse.Has(e);
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor, std::make_unsigned_t<type> offsetSize> requires (std::is_arithmetic_v<type>)
		typename EntityPool<type, allocatorFactor,offsetSize>::unsigned_type EntityPool<type, allocatorFactor,offsetSize>::CreateEntity()
		{
			if (m_freeId.empty())
				FilledWithIds();
			unsigned_type id = m_freeId.back();
			m_freeId.pop_back();
			m_sparse.Add(id);
			return id;
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor, std::make_unsigned_t<type> offsetSize> requires (std::is_arithmetic_v<type>)
		void EntityPool<type, allocatorFactor,offsetSize>::RemoveEntity(unsigned_type e)
		{

			signed_type index = m_sparse.template At<KT::Storage::SPARSE>(e);
			m_sparse.Remove(e);
			m_freeId.push_back(e);
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor, std::make_unsigned_t<type> offsetSize> requires (std::is_arithmetic_v<type>)
		typename EntityPool<type, allocatorFactor,offsetSize>::unsigned_type EntityPool<type, allocatorFactor,offsetSize>::Size() const
		{
			return m_sparse.Size();
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor, std::make_unsigned_t<type> offsetSize> requires (std::is_arithmetic_v<type>)
		const typename EntityPool<type, allocatorFactor,offsetSize>::container_type& EntityPool<type, allocatorFactor,offsetSize>::
		GetEntities() const
		{
			return m_sparse.Dense();
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor, std::make_unsigned_t<type> offsetSize> requires (std::is_arithmetic_v<type>)
		void EntityPool<type, allocatorFactor,offsetSize>::FilledWithIds()
		{

			if (m_lastEntity >= std::numeric_limits<signed_type>::max())
				throw std::out_of_range("too many entities for type");

			signed_type remaining = std::numeric_limits<signed_type>::max() - static_cast<signed_type>(m_lastEntity);
			signed_type chunkSize = std::min(static_cast<signed_type>(offsetSize), remaining);

			m_freeId.resize(chunkSize);
			m_lastEntity += chunkSize;
			signed_type currentEntity = m_lastEntity;
			for (size_t i = 0; i < m_freeId.size(); ++i)
			{
				m_freeId[i] = --currentEntity;
			}
		}
	}
}
