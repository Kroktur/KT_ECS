#include <iostream>
#include <vector>
#include <chrono>

#include "Entities.h"
#include "Registry.h" // Ton ECS

using Clock = std::chrono::high_resolution_clock;

// Structure pour le test "vector AoS"
struct Position { float x = 0.f, y = 0.f; };
struct Velocity { float vx = 0.f, vy = 0.f; };
struct Velocity2 { float vx = 0.f, vy = 0.f; };
struct Velocity3 { float vx = 0.f, vy = 0.f; };
struct Velocity4 { float vx = 0.f, vy = 0.f; };
struct Velocity5 { float vx = 0.f, vy = 0.f; };
struct Velocity6 { float vx = 0.f, vy = 0.f; };
// Pour ton ECS
using ecsType = KT::ECS::Registry<KT::ECS::Entity::_64, 100, 100* 10>;

struct toto
{
    Position pos; Velocity vel;
};
int main() {
    const size_t N = 1'000'000;
    const int STEPS = 100;
    // ========================= ECS =========================
    ecsType ecs;

    auto t1 = Clock::now();
    for (auto i = 0; i < N; ++i) {
        auto e = ecs.CreateEntity();
        ecs.AddComponents<Position, Velocity>(e);
    }

   
    
    auto t2 = Clock::now();
    for (int step = 0; step < STEPS; ++step) {
        auto entities = ecs.GetAllComponentsView<Position, Velocity>();
        auto& posComp = ecs.Get<Position>();
        auto& velComp = ecs.Get<Velocity>();
        for (auto& e : entities) {
            auto& pos = posComp.Get(e);
            auto& vel = velComp.Get(e);
            pos.x += vel.vx;
            pos.y += vel.vy;
        }
    }
    auto t3 = Clock::now();

    double ecs_creation = std::chrono::duration<double>(t2 - t1).count();
    double ecs_update = std::chrono::duration<double>(t3 - t2).count();

    // ========================= VECTOR AoS =========================
    std::vector< toto> vec;

    vec.reserve(N);

    auto t4 = Clock::now();
    for (size_t i = 0; i < N; ++i)
        vec.push_back({ {0.f,0.f}, {1.f,1.f} });
    auto t5 = Clock::now();

    for (int step = 0; step < STEPS; ++step) {
        for (auto& e : vec) {
            e.pos.x += e.vel.vx;
            e.pos.y += e.vel.vy;


        }
    }
    auto t6 = Clock::now();

    double vec_creation = std::chrono::duration<double>(t5 - t4).count();
    double vec_update = std::chrono::duration<double>(t6 - t5).count();

    // ========================= RESULTATS =========================
    std::cout << "===== ECS =====\n";
    std::cout << "Creation: " << ecs_creation << " s\n";
    std::cout << "Update  : " << ecs_update << " s\n\n";

    std::cout << "===== VECTOR (AoS) =====\n";
    std::cout << "Creation: " << vec_creation << " s\n";
    std::cout << "Update  : " << vec_update << " s\n\n";

    double ecs_ops = N * STEPS / ecs_update;
    double vec_ops = N * STEPS / vec_update;
    std::cout << "===== PERF =====\n";
    std::cout << "ECS Ops/sec: " << ecs_ops << "\n";
    std::cout << "Vector Ops/sec: " << vec_ops << "\n";
    std::cout << "ECS / Vector Ratio: " << ecs_update / vec_update << "\n";

    return 0;
}