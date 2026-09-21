#pragma once
#include <string>
#include <vector>
#include "data/data_registries.h"
#include "world/npc.h"

namespace hl2 {

enum class CombatState { idle, player_turn, enemy_turn, victory, defeat };

struct CombatRound {
    std::string attacker;
    std::string target;
    uint32_t damage = 0;
    std::string action;
    bool crit = false;
};

class CombatEngine {
public:
    CombatEngine(const EnemyRegistry& enemies, const DifficultyManager& diff);

    void engage(const EnemyDef& enemy, float player_hp, float player_max_hp,
                const WeaponDef& weapon);

    std::vector<std::string> attack();
    std::vector<std::string> use_weapon(size_t slot);
    std::vector<std::string> reload();
    std::vector<std::string> heal(uint32_t amount);
    std::vector<std::string> flee();

    bool is_over() const { return m_state == CombatState::victory || m_state == CombatState::defeat; }
    bool player_won() const { return m_state == CombatState::victory; }
    uint32_t enemy_hp() const { return m_enemy_hp; }
    uint32_t enemy_max_hp() const { return m_enemy_max_hp; }
    uint32_t player_hp() const { return m_player_hp; }
    CombatState state() const { return m_state; }

private:
    const EnemyRegistry& m_enemy_db;
    const DifficultyManager& m_diff;
    CombatState m_state = CombatState::idle;
    uint32_t m_enemy_hp = 0;
    uint32_t m_enemy_max_hp = 0;
    uint32_t m_player_hp = 0;
    uint32_t m_player_max_hp = 0;
    const EnemyDef* m_enemy = nullptr;
    const WeaponDef* m_weapon = nullptr;
    uint32_t m_ammo_in_mag = 0;
    uint32_t m_mag_size = 30;
    uint32_t m_ammo_reserve = 90;

    void enemy_attack();
    void check_victory();
};

} // namespace hl2
