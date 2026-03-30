#pragma once
#include "ComponentPool.h"
#include "EntityPool.h"


namespace KT
{
	namespace ECS
	{
		template<typename type, std::make_unsigned_t<type> allocatorFactor = 1, std::make_unsigned_t<type> offsetSize = 10 * allocatorFactor> requires (std::is_arithmetic_v<type>)
		class Registry
		{
		public:
		
			using base_type = type;
			using unsigned_type = std::make_unsigned_t<base_type>;
			using signed_type = std::make_signed_t<base_type>;
			using pool_type = ComponentPool<type, allocatorFactor>;
			using entity_pool_type = EntityPool<type, allocatorFactor, offsetSize>;
			using base_pool_type = pool_type::base_type;
			template<typename Component>
			using derived_type = pool_type::template derived_type<Component>;
			using view = std::vector<unsigned_type>;
		public:
			Registry() = default;
			~Registry();
			Registry(const Registry&) = default;
			Registry(Registry&&) noexcept = default;
			Registry& operator=(const Registry&) = default;
			Registry& operator=(Registry&&) noexcept = default;
		public:
			unsigned_type CreateEntity();
			void DestroyEntity(unsigned_type entity);

			template<typename  Component>
			void AddComponent(const type& e);
			template<typename Component>
			void AddComponent(const type& e, Component&& c);

			template<typename... Components>
			void AddComponents(const type& e);
			template<typename... Components>
			void AddComponents(const type& e, Components&&... c);

			template<typename Component>
			void RemoveComponent(const type& e);
			template<typename... Components>
			void RemoveComponents(const type& e);

			template<typename Component>
			bool HasComponent(const type& e) const;
			template<typename... Components>
			bool HasAllComponents(const type& e) const;
			template<typename... Components>
			bool HasAnyComponents(const type& e) const;
			
			template <typename Component>
			derived_type<Component>& Get();

			template<typename Component>
			const view& GetView() const;

			template<typename... Components>
			view GetAllComponentsView();

			template<typename... Components>
			view GetAnyComponentsView();

			bool HasEntity(const type& e) const;
		private:

			template<typename... Component>
			std::vector<base_pool_type*> GetPools();
			entity_pool_type m_entityPool;
			pool_type m_componentPool;

		};

		template <typename type, std::make_unsigned_t<type> allocatorFactor, std::make_unsigned_t<type> offsetSize>
			requires (std::is_arithmetic_v<type>)
		Registry<type, allocatorFactor, offsetSize>::~Registry()
		{
			for (auto& e : m_entityPool.GetEntities())
				DestroyEntity(e);
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor, std::make_unsigned_t<type> offsetSize>
			requires (std::is_arithmetic_v<type>)
		typename Registry<type, allocatorFactor, offsetSize>::unsigned_type Registry<type, allocatorFactor, offsetSize>
		::CreateEntity()
		{
			return m_entityPool.CreateEntity();
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor, std::make_unsigned_t<type> offsetSize>
			requires (std::is_arithmetic_v<type>)
		void Registry<type, allocatorFactor, offsetSize>::DestroyEntity(unsigned_type entity)
		{
			m_entityPool.RemoveEntity(entity);
			const std::vector<std::unique_ptr<base_pool_type>>* pool = &m_componentPool.GetPools();
			for (auto& it : *pool)
			{
				if (it->Has(entity))
					it->Remove(entity);
			}
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor, std::make_unsigned_t<type> offsetSize>
			requires (std::is_arithmetic_v<type>)
		template <typename Component>
		void Registry<type, allocatorFactor, offsetSize>::AddComponent(const type& e)
		{
			m_componentPool.template GetAndAdd<Component>().Add(e);
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor, std::make_unsigned_t<type> offsetSize>
			requires (std::is_arithmetic_v<type>)
		template <typename Component>
		void Registry<type, allocatorFactor, offsetSize>::AddComponent(const type& e, Component&& c)
		{
			m_componentPool.template GetAndAdd<Component>().Add(e, std::move(c));
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor, std::make_unsigned_t<type> offsetSize>
			requires (std::is_arithmetic_v<type>)
		template <typename ... Components>
		void Registry<type, allocatorFactor, offsetSize>::AddComponents(const type& e)
		{
			(..., m_componentPool.template GetAndAdd<Components>().Add(e));
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor, std::make_unsigned_t<type> offsetSize>
			requires (std::is_arithmetic_v<type>)
		template <typename ... Components>
		void Registry<type, allocatorFactor, offsetSize>::AddComponents(const type& e, Components&&... c)
		{
			(..., m_componentPool.template GetAndAdd<Components>().Add(e, std::move(c)));
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor, std::make_unsigned_t<type> offsetSize>
			requires (std::is_arithmetic_v<type>)
		template <typename Component>
		void Registry<type, allocatorFactor, offsetSize>::RemoveComponent(const type& e)
		{
			m_componentPool.template Get<Component>().Remove(e);
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor, std::make_unsigned_t<type> offsetSize>
			requires (std::is_arithmetic_v<type>)
		template <typename ... Components>
		void Registry<type, allocatorFactor, offsetSize>::RemoveComponents(const type& e)
		{
			(..., m_componentPool.template Get<Components>().Remove(e));
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor, std::make_unsigned_t<type> offsetSize>
			requires (std::is_arithmetic_v<type>)
		template <typename Component>
		bool Registry<type, allocatorFactor, offsetSize>::HasComponent(const type& e) const
		{
			if ((!m_componentPool.template Has<Component>() ? false : !m_componentPool.template Get<Component>().Has(e)))
				return false;
			return true;
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor, std::make_unsigned_t<type> offsetSize>
			requires (std::is_arithmetic_v<type>)
		template <typename ... Components>
		bool Registry<type, allocatorFactor, offsetSize>::HasAllComponents(const type& e) const
		{
			if ((... || (!m_componentPool.template Has<Components>() ? false : !m_componentPool.template Get<Components>().Has(e))))
				return false;
			return true;
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor, std::make_unsigned_t<type> offsetSize>
			requires (std::is_arithmetic_v<type>)
		template <typename ... Components>
		bool Registry<type, allocatorFactor, offsetSize>::HasAnyComponents(const type& e) const
		{

			if ((... || (m_componentPool.template Has<Components>() == true ? m_componentPool.template Get<Components>().Has(e) : false)))
				return true;
			return false;
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor, std::make_unsigned_t<type> offsetSize>
			requires (std::is_arithmetic_v<type>)
		template <typename Component>
		Registry<type, allocatorFactor, offsetSize>::template derived_type<Component>& Registry<type, allocatorFactor, offsetSize
		>::Get()
		{
			return m_componentPool.template Get<Component>();
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor, std::make_unsigned_t<type> offsetSize>
			requires (std::is_arithmetic_v<type>)
		template <typename Component>
		const typename Registry<type, allocatorFactor, offsetSize>::view& Registry<type, allocatorFactor, offsetSize>::
		GetView() const
		{
			return m_componentPool.template Get<Component>().GetEntities();
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor, std::make_unsigned_t<type> offsetSize>
			requires (std::is_arithmetic_v<type>)
		template <typename ... Components>
		typename Registry<type, allocatorFactor, offsetSize>::view Registry<type, allocatorFactor, offsetSize>::
		GetAllComponentsView()
		{
			std::vector<base_pool_type*> pools = GetPools<Components...>();

			const std::vector<unsigned_type>* smallestEntity = nullptr;
			for (base_pool_type* p : pools)
			{
				if (!smallestEntity || smallestEntity->size() > p->Size())
					smallestEntity = &p->GetEntities();
			}

			view result;
			result.reserve(smallestEntity->size());
			for (type e : *smallestEntity)
			{
				bool valid = true;
				for (base_pool_type* p : pools)
				{
					if (!p->Has(e))
					{
						valid = false;
						break;
					}
				}
				if (valid)
					result.push_back(e);
			}
			return result;
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor, std::make_unsigned_t<type> offsetSize>
			requires (std::is_arithmetic_v<type>)
		template <typename ... Components>
		typename Registry<type, allocatorFactor, offsetSize>::view Registry<type, allocatorFactor, offsetSize>::
		GetAnyComponentsView()
		{
			std::vector<base_pool_type*> pools = GetPools<Components...>();

			const std::vector<unsigned_type>* biggestEntity = nullptr;
			for (base_pool_type* p : pools)
			{
				if (!biggestEntity || biggestEntity->size() < p->Size())
					biggestEntity = &p->GetEntities();
			}
			return *biggestEntity;
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor, std::make_unsigned_t<type> offsetSize>
			requires (std::is_arithmetic_v<type>)
		bool Registry<type, allocatorFactor, offsetSize>::HasEntity(const type& e) const
		{
			return m_entityPool.HasEntity(e);
		}

		template <typename type, std::make_unsigned_t<type> allocatorFactor, std::make_unsigned_t<type> offsetSize>
			requires (std::is_arithmetic_v<type>)
		template <typename ... Component>
		std::vector<typename Registry<type, allocatorFactor, offsetSize>::base_pool_type*> Registry<type,
		allocatorFactor, offsetSize>::GetPools()
		{
			std::vector<base_pool_type*> pools;
			pools.reserve(m_componentPool.Size());
			(..., pools.push_back(&m_componentPool.template Get<Component>()));
			return pools;
		}
	}
}
