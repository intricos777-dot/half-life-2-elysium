#include "combat.h"
#include <cstdio>
#include <cmath>
#include <algorithm>

namespace hl2 {

CombatEngine::CombatEngine(const EnemyRegistry& enemies, const DifficultyManager& diff)
    : m_enemy_db(enemies), m_diff(diff) {}

void CombatEngine::engage(const EnemyDef& enemy, float player_hp, float player_max_hp,
                          const WeaponDef& weapon) {
    m_enemy = &enemy;
    m_weapon = &weapon;
    m_player_hp = (uint32_t)player_hp;
    m_player_max_hp = (uint32_t)player_max_hp;
    const auto& d = m_diff.current();
    m_enemy_max_hp = (uint32_t)(enemy.hp * d.enemy_health_mult);
    m_enemy_hp = m_enemy_max_hp;
    m_state = CombatState::player_turn;
    m_ammo_in_mag = m_mag_size;
    m_ammo_reserve = (uint32_t)(90 * d.ammo_mult);
    std::printf("[Combat] Engaged %s (hp=%u)\n", enemy.name.c_str(), m_enemy_hp);
}

std::vector<std::string> CombatEngine::attack() {
    std::vector<std::string> log;
    if (m_state != CombatState::player_turn || !m_enemy) return log;
    if (m_ammo_in_mag == 0) { log.push_back("Click! Magazine empty."); return log; }

    const auto& d = m_diff.current();
    uint32_t dmg = (uint32_t)(m_weapon->damage * m_weapon->pellets);
    bool crit = (std::rand() % 100) < 12;
    if (crit) dmg = (uint32_t)(dmg * 1.5);

    m_enemy_hp = std::max(0u, (uint32_t)m_enemy_hp - dmg);
    m_ammo_in_mag--;

    log.push_back("You fire the " + m_weapon->name + " at " + m_enemy->name +
                  " for " + std::to_string(dmg) + " damage." + (crit ? " Critical!" : ""));
    log.push_back(m_weapon->name + ": " + std::to_string(m_ammo_in_mag) + "/" +
                 std::to_string(m_mag_size));

    check_victory();
    if (m_state != CombatState::victory) {
        m_state = CombatState::enemy_turn;
        enemy_attack();
    }
    return log;
}

std::vector<std::string> CombatEngine::use_weapon(size_t slot) {
    return attack();
}

std::vector<std::string> CombatEngine::reload() {
    std::vector<std::string> log;
    if (m_ammo_reserve == 0) { log.push_back("No ammo reserve."); return log; }
    uint32_t need = m_mag_size - m_ammo_in_mag;
    uint32_t take = std::min(need, m_ammo_reserve);
    m_ammo_in_mag += take;
    m_ammo_reserve -= take;
    log.push_back("Reloaded. " + std::to_string(m_ammo_in_mag) + "/" + std::to_string(m_mag_size));
    return log;
}

std::vector<std::string> CombatEngine::heal(uint32_t amount) {
    m_player_hp = std::min(m_player_max_hp, m_player_hp + amount);
    return {"Health charger activated. +" + std::to_string(amount) + " HP (" +
            std::to_string(m_player_hp) + "/" + std::to_string(m_player_max_hp) + ")"};
}

std::vector<std::string> CombatEngine::flee() {
    std::vector<std::string> log;
    if ((std::rand() % 100) < 40) {
        m_state = CombatState::idle;
        log.push_back("You slip through a side passage. The enemy loses sight of you.");
    } else {
        log.push_back("Can't escape! " + m_enemy->name + " blocks the exit!");
        enemy_attack();
    }
    return log;
}

void CombatEngine::enemy_attack() {
    if (m_enemy_hp == 0 || m_enemy_hp > m_enemy_max_hp) return;
    const auto& d = m_diff.current();
    if ((std::rand() % 100) / 100.0f > d.enemy_accuracy) {
        std::printf("[Combat] %s misses.\n", m_enemy->name.c_str());
        m_state = CombatState::player_turn;
        return;
    }
    uint32_t dmg = (uint32_t)(m_enemy->damage * d.enemy_damage_mult);
    dmg = std::max(1u, dmg);
    m_player_hp = std::max(0u, m_player_hp - dmg);
    std::printf("[Combat] %s hits Gordon for %u\n", m_enemy->name.c_str(), dmg);
    if (m_player_hp == 0) {
        m_state = CombatState::defeat;
    } else {
        m_state = CombatState::player_turn;
    }
}

void CombatEngine::check_victory() {
    if (m_enemy_hp == 0) {
        m_state = CombatState::victory;
        std::printf("[Combat] %s down.\n", m_enemy->name.c_str());
    }
}

} // namespace hl2
