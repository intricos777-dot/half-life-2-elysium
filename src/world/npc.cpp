#include "npc.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <cstdio>

namespace hl2 {

bool NPCDb::load(const std::string& path) {
    std::ifstream f(path);
    if (!f) { std::fprintf(stderr, "[NPC] missing: %s\n", path.c_str()); return false; }
    nlohmann::json j;
    try { f >> j; } catch (const std::exception& e) {
        std::fprintf(stderr, "[NPC] parse error: %s\n", e.what()); return false;
    }
    for (const auto& n : j.value("npcs", nlohmann::json::array())) {
        NPCDef def;
        def.id = n.value("id", "");
        def.name = n.value("name", def.id);
        def.role = n.value("role", "citizen");
        def.affiliation = n.value("affiliation", "citizen");
        def.current_world = n.value("world", "city17");
        if (n.contains("schedules")) {
            for (const auto& s : n["schedules"]) {
                NPCSchedule sched;
                sched.location = s.value("location", "");
                for (auto& [h, text] : s.value("lines", nlohmann::json::object()).items())
                    sched.lines[std::stoi(h)] = text.get<std::string>();
                def.schedules.push_back(sched);
            }
        }
        m_npcs.push_back(std::move(def));
    }
    std::printf("[NPC] %zu loaded\n", m_npcs.size());
    return true;
}

const NPCDef* NPCDb::find(const std::string& id) const {
    for (const auto& n : m_npcs) if (n.id == id) return &n;
    return nullptr;
}

std::vector<const NPCDef*> NPCDb::in_world(const std::string& world) const {
    std::vector<const NPCDef*> out;
    for (const auto& n : m_npcs)
        if (n.current_world == world || n.current_world == "any") out.push_back(&n);
    return out;
}

void register_hl2_npcs(NPCDb& db) {
    NPCDef alyx;
    alyx.id = "alyx"; alyx.name = "Alyx Vance"; alyx.role = "ally";
    alyx.affiliation = "resistance";
    alyx.current_world = "city17";
    {
        NPCSchedule sched;
        sched.location = "hideout";
        sched.lines = {
            {2, "Gordon! You came back. I knew you would."},
            {8, "Dad's been working on something. He won't tell me what."},
            {14, "Combine scanners are everywhere. Watch the skies."},
            {20, "We're going to Nova Prospekt. For Eli."},
        };
        alyx.schedules.push_back(sched);
    }
    db.npcs().push_back(alyx);

    NPCDef eli;
    eli.id = "eli"; eli.name = "Dr. Eli Vance"; eli.role = "scientist";
    eli.affiliation = "resistance";
    eli.current_world = "city17";
    {
        NPCSchedule sched;
        sched.location = "lab";
        sched.lines = {
            {3, "Gordon, my boy. The Combine are not invincible."},
            {9, "We've been working on a way to close the superportal."},
            {15, "Alyx is the best thing that ever happened to me."},
            {21, "The Borealis... it must never fall into Combine hands."},
        };
        eli.schedules.push_back(sched);
    }
    db.npcs().push_back(eli);

    NPCDef mossman;
    mossman.id = "mossman"; mossman.name = "Dr. Judith Mossman"; mossman.role = "scientist";
    mossman.affiliation = "black_mesa_east";
    mossman.current_world = "nova_prospekt";
    {
        NPCSchedule sched;
        sched.location = "control";
        sched.lines = {
            {4, "Gordon, I... I didn't have a choice. Breen forced my hand."},
            {11, "I transmitted the data to the resistance. It's done."},
            {17, "Don't trust anyone at Black Mesa East."},
        };
        mossman.schedules.push_back(sched);
    }
    db.npcs().push_back(mossman);

    NPCDef vort;
    vort.id = "vortigaunt"; vort.name = "Vortigaunt Ally"; vort.role = "ally";
    vort.affiliation = "xen";
    vort.current_world = "highway17";
    {
        NPCSchedule sched;
        sched.location = "roadside";
        sched.lines = {
            {5, "We remember the Freeman. We owe a debt that spans galaxies."},
            {12, "The Combine enslaved our kind once. Not again."},
            {18, "Come. Nova Prospekt awaits."},
        };
        vort.schedules.push_back(sched);
    }
    db.npcs().push_back(vort);

    NPCDef soldier;
    soldier.id = "combine_soldier"; soldier.name = "Combine Soldier"; soldier.role = "enemy";
    soldier.affiliation = "combine";
    soldier.current_world = "city17";
    {
        NPCSchedule sched;
        sched.location = "checkpoint";
        sched.lines = {
            {7, "Pick up that can."},
            {13, "Citizen, present your identification."},
            {19, "Code: sacrifice. Anticitizen reported."},
        };
        soldier.schedules.push_back(sched);
    }
    db.npcs().push_back(soldier);

    std::printf("[NPC] %zu HL2 roster registered\n", db.npcs().size());
}

} // namespace hl2
