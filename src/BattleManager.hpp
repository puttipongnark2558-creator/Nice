#pragma once
#include <Geode/Geode.hpp>

class BattleManager {
private:
    int m_elo = 1250;
    int m_tokens = 150;
    int m_winStreak = 0;
    bool m_inMatch = false;

public:
    static BattleManager& get() {
        static BattleManager instance;
        return instance;
    }

    int getElo() const { return m_elo; }
    void addElo(int amount) { m_elo += amount; }
    void subElo(int amount) { m_elo = std::max(0, m_elo - amount); }

    int getTokens() const { return m_tokens; }
    void addTokens(int amount) { m_tokens += amount; }

    int getWinStreak() const { return m_winStreak; }
    void incrementStreak() { m_winStreak++; }
    void resetStreak() { m_winStreak = 0; }

    bool isMatchActive() const { return m_inMatch; }
    void setMatchActive(bool active) { m_inMatch = active; }

    std::string getRankTier() {
        if (m_elo < 1100) return "Bronze Tier";
        if (m_elo < 1300) return "Silver Tier";
        if (m_elo < 1500) return "Gold Tier";
        if (m_elo < 1800) return "Platinum Tier";
        if (m_elo < 2100) return "Diamond Master";
        return "Grandmaster Elite";
    }
};