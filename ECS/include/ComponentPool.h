#pragma once
#include "ComponentContainer.h"
#include <memory>
namespace KT
{
	namespace ECS
	{
		struct CompId
		{
		public:
			template<typename Component>
			static size_t GetId();
		private:
			static std::atomic_size_t m_id;
		};
		inline std::atomic_size_t CompId::m_id = 0;
		template <typename Component>
		size_t CompId::GetId()
		{
			static size_t id = m_id++;
			return id;
		}


		template<typename type, std::make_unsigned_t<type> allocatorFactor = 1> requires (std::is_arithmetic_v<type>)
		class ComponentPool
		{
		public:
			using base_type = ICompContainer<type, allocatorFactor>;
			using sparse_type = base_type::sparse_type;
			template<typename Component>
			using derived_type = CompContainerDense<Component, type, allocatorFactor>;
			using unsigned_type = base_type::unsigned_type;
			using signed_type = base_type::signed_type;
			using container_components_type = std::vector<std::unique_ptr<base_type>>;
			using const_iterator_components = container_components_type::const_iterator;
			using iterator_components = container_components_type::iterator;
		public:
			ComponentPool() = default;
			~ComponentPool() = default;
			ComponentPool(const ComponentPool&) = default;
			ComponentPool(ComponentPool&&) noexcept = default;
			ComponentPool& operator=(const ComponentPool&) = default;
			ComponentPool& operator=(ComponentPool&&) noexcept = default;
		public:
			template<typename Component>
			bool Has() const;

			template<typename Component>
			void Add();

			template<typename Component>
			void Remove();

			unsigned_type Size() const;

			template<typename Component>
			derived_type<Component>& Get();
			template<typename Component>
			const derived_type<Component>& Get() const;

			template<typename Component>
			derived_type<Component>& GetAndAdd();


			base_type& At(unsigned_type index);
			const base_type& At(unsigned_type index) const;
			
			iterator_components begin();
			iterator_components end();

			const_iterator_components begin() const;
			const_iterator_components end() const;

			const container_components_type& GetPools() const;
		private:
			sparse_type m_sparse;
			container_components_type m_dense;
		};

		template <typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::is_arithmetic_v<type>)
		template <typename Component>
		bool ComponentPool<type, allocatorFactor>::Has() const
		{
			return m_sparse.Has(CompId::GetId<Component>());
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::is_arithmetic_v<type>)
		template <typename Component>
		void ComponentPool<type, allocatorFactor>::Add()
		{
			if (Has<Component>())
				throw std::out_of_range("component already store");

			m_sparse.Add(CompId::GetId<Component>());
			m_dense.push_back(std::make_unique<derived_type<Component>>());
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::is_arithmetic_v<type>)
		template <typename Component>
		void ComponentPool<type, allocatorFactor>::Remove()
		{
			if (!Has<Component>())
				throw std::out_of_range("component not store");

			signed_type index = m_sparse.template At<KT::Storage::SPARSE>(CompId::GetId<Component>());
			m_sparse.Remove(CompId::GetId<Component>());
			m_dense[index] = std::move(m_dense.back());
			m_dense.pop_back();
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::is_arithmetic_v<type>)
		typename ComponentPool<type, allocatorFactor>::unsigned_type ComponentPool<type, allocatorFactor>::Size() const
		{
			return m_dense.size();
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::is_arithmetic_v<type>)
		template <typename Component>
		ComponentPool<type, allocatorFactor>::template derived_type<Component>& ComponentPool<type, allocatorFactor>::Get()
		{
			return static_cast<derived_type<Component>&>(*m_dense[m_sparse.template At<KT::Storage::SPARSE>(CompId::GetId<Component>())].get());
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::is_arithmetic_v<type>)
		template <typename Component>
		const ComponentPool<type, allocatorFactor>::template derived_type<Component>& ComponentPool<type, allocatorFactor>::
		Get() const
		{
			return static_cast<derived_type<Component>&>(*m_dense[m_sparse.template At<KT::Storage::SPARSE>(CompId::GetId<Component>())].get());
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::is_arithmetic_v<type>)
		template <typename Component>
		ComponentPool<type, allocatorFactor>::template derived_type<Component>& ComponentPool<type, allocatorFactor>::GetAndAdd()
		{
			if (!Has<Component>())
				Add<Component>();
			return Get<Component>();
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::is_arithmetic_v<type>)
		typename ComponentPool<type, allocatorFactor>::base_type& ComponentPool<type, allocatorFactor>::At(
			unsigned_type index)
		{
			return m_dense.at(index);
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::is_arithmetic_v<type>)
		const typename ComponentPool<type, allocatorFactor>::base_type& ComponentPool<type, allocatorFactor>::At(
			unsigned_type index) const
		{
			return m_dense.at(index);
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::is_arithmetic_v<type>)
		typename ComponentPool<type, allocatorFactor>::iterator_components ComponentPool<type, allocatorFactor>::begin()
		{
			return m_dense.begin();
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::is_arithmetic_v<type>)
		typename ComponentPool<type, allocatorFactor>::iterator_components ComponentPool<type, allocatorFactor>::end()
		{
			return m_dense.end();
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::is_arithmetic_v<type>)
		typename ComponentPool<type, allocatorFactor>::const_iterator_components ComponentPool<type, allocatorFactor>::
		begin() const
		{
			return m_dense.begin();
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::is_arithmetic_v<type>)
		typename ComponentPool<type, allocatorFactor>::const_iterator_components ComponentPool<type, allocatorFactor>::
		end() const
		{
			return m_dense.end();
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::is_arithmetic_v<type>)
		const typename ComponentPool<type, allocatorFactor>::container_components_type& ComponentPool<type,
		allocatorFactor>::GetPools() const
		{
			return m_dense;
		}
	}
}