#pragma once
#include <type_traits>
#include "Sparse.h"
#include <stdexcept>

namespace KT
{
	namespace ECS
	{
		template<typename type, std::make_unsigned_t<type> allocatorFactor = 1> requires (std::is_arithmetic_v<type>)
		class ICompContainer
		{
		public:
			using sparse_type = SparseSet<type, allocatorFactor>;
			using base_type = sparse_type::base_type;
			using signed_type = sparse_type::signed_type;
			using unsigned_type = sparse_type::unsigned_type;
			using container_entity_type = sparse_type::container_dense_type;
		public:
			ICompContainer() = default;
			virtual ~ICompContainer() = default;
			ICompContainer(const ICompContainer&) = default;
			ICompContainer(ICompContainer&&) noexcept = default;
			ICompContainer& operator=(const ICompContainer&) = default;
			ICompContainer& operator=(ICompContainer&&) noexcept = default;
		public:
			bool Has(unsigned_type e) const;
			virtual void Remove(unsigned_type e) = 0;
			unsigned_type Size() const;
			const unsigned_type& AtEntity(unsigned_type index) const;
			const container_entity_type& GetEntities() const;
		protected:
			sparse_type m_sparse;
		};

		template <typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::is_arithmetic_v<type>)
		bool ICompContainer<type, allocatorFactor>::Has(unsigned_type e) const
		{
			return m_sparse.Has(e);
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::is_arithmetic_v<type>)
		typename ICompContainer<type, allocatorFactor>::unsigned_type ICompContainer<type, allocatorFactor>::
		Size() const
		{
			return m_sparse.template Size<Storage::DENSE>();
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::is_arithmetic_v<type>)
		const typename ICompContainer<type, allocatorFactor>::unsigned_type& ICompContainer<type, allocatorFactor>::AtEntity(
			unsigned_type index) const
		{
			return m_sparse.template At<Storage::DENSE>(index);
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::is_arithmetic_v<type>)
		const typename ICompContainer<type, allocatorFactor>::container_entity_type& ICompContainer<type,
		allocatorFactor>::GetEntities() const
		{
			return m_sparse.Dense();
		}

		template<typename Component,typename type, std::make_unsigned_t<type> allocatorFactor = 1> requires (std::is_arithmetic_v<type>)
		class CompContainerDense : public ICompContainer<type,allocatorFactor>
		{
		public:
			using base_type = ICompContainer<type, allocatorFactor>::base_type;
			using unsigned_type = ICompContainer<type, allocatorFactor>::unsigned_type;
			using signed_type = ICompContainer<type, allocatorFactor>::signed_type;
			using container_type = std::vector<Component>;
			using component_type = Component;
			using const_iterator_component = container_type::const_iterator;
			using iterator_component = container_type::iterator;
		public:
			CompContainerDense() = default;
			~CompContainerDense() override = default;
			CompContainerDense(const CompContainerDense&) = default;
			CompContainerDense(CompContainerDense&&) noexcept = default;
			CompContainerDense& operator=(const CompContainerDense&) = default;
			CompContainerDense& operator=(CompContainerDense&&) noexcept= default;
		public:
			void Add(unsigned_type e);
			void Add(unsigned_type e, Component&& comp);
			void Remove(unsigned_type e) override;

			component_type& Get(unsigned_type e);
			const component_type& Get(unsigned_type e) const;

			component_type& At(unsigned_type e);
			const component_type& At(unsigned_type e) const;

			iterator_component begin();
			iterator_component end();
			const_iterator_component begin() const;
			const_iterator_component end() const;

			const container_type& Components() const;
			void Reserve(unsigned_type index);
		private:
			container_type m_dense;
		};

		template <typename Component, typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::
			is_arithmetic_v<type>)
		void CompContainerDense<Component, type, allocatorFactor>::Add(unsigned_type e)
		{
			Add(e, std::move(Component{}));
		}

		template <typename Component, typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::
			is_arithmetic_v<type>)
		void CompContainerDense<Component, type, allocatorFactor>::Add(unsigned_type e, Component&& comp)
		{
			if (this->Has(e))
				throw std::out_of_range("component already store");

			this->m_sparse.Add(e);
			
			m_dense.push_back(std::move(comp));
		}

		template <typename Component, typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::
			is_arithmetic_v<type>)
		void CompContainerDense<Component, type, allocatorFactor>::Remove(unsigned_type e)
		{
			if (!this->Has(e))
				throw std::out_of_range("component not store");

			signed_type index = this->m_sparse.template At<KT::Storage::SPARSE>(e);
			this->m_sparse.Remove(e);
			m_dense[index] = std::move(m_dense.back());
			m_dense.pop_back();
		}

		template <typename Component, typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::
			is_arithmetic_v<type>)
		typename CompContainerDense<Component, type, allocatorFactor>::component_type& CompContainerDense<Component,
		type, allocatorFactor>::Get(unsigned_type e)
		{
			return m_dense.at(this->m_sparse.template At<KT::Storage::SPARSE>(e));
		}

		template <typename Component, typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::
			is_arithmetic_v<type>)
		const typename CompContainerDense<Component, type, allocatorFactor>::component_type& CompContainerDense<
		Component, type, allocatorFactor>::Get(unsigned_type e) const
		{
			return m_dense.at(this->m_sparse.template At<KT::Storage::SPARSE>(e));
		}

		template <typename Component, typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::
			is_arithmetic_v<type>)
		typename CompContainerDense<Component, type, allocatorFactor>::component_type& CompContainerDense<Component,
		type, allocatorFactor>::At(unsigned_type e)
		{
			return m_dense.at(e);
		}

		template <typename Component, typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::
			is_arithmetic_v<type>)
		const typename CompContainerDense<Component, type, allocatorFactor>::component_type& CompContainerDense<
		Component, type, allocatorFactor>::At(unsigned_type e) const
		{
			return m_dense.at(e);
		}

		template <typename Component, typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::
			is_arithmetic_v<type>)
		typename CompContainerDense<Component, type, allocatorFactor>::iterator_component CompContainerDense<Component,
		type, allocatorFactor>::begin()
		{
			return m_dense.begin();
		}

		template <typename Component, typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::
			is_arithmetic_v<type>)
		typename CompContainerDense<Component, type, allocatorFactor>::iterator_component CompContainerDense<Component,
		type, allocatorFactor>::end()
		{
			return m_dense.end();
		}

		template <typename Component, typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::
			is_arithmetic_v<type>)
		typename CompContainerDense<Component, type, allocatorFactor>::const_iterator_component CompContainerDense<
		Component, type, allocatorFactor>::begin() const
		{
			return m_dense.begin();
		}

		template <typename Component, typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::
			is_arithmetic_v<type>)
		typename CompContainerDense<Component, type, allocatorFactor>::const_iterator_component CompContainerDense<
		Component, type, allocatorFactor>::end() const
		{
			return m_dense.end();
		}

		template <typename Component, typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::
			is_arithmetic_v<type>)
		const typename CompContainerDense<Component, type, allocatorFactor>::container_type& CompContainerDense<
		Component, type, allocatorFactor>::Components() const
		{
			return m_dense;
		}

		template <typename Component, typename type, std::make_unsigned_t<type> allocatorFactor> requires (std::
			is_arithmetic_v<type>)
		void CompContainerDense<Component, type, allocatorFactor>::Reserve(unsigned_type index)
		{
			m_dense.reserve(index);
			this->m_sparse.template Reserve<KT::Storage::DENSE>(index);
		}
	}
}

