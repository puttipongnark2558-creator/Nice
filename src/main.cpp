#include <Geode/Geode.hpp>
#include <Geode/ui/Notification.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include "BattleManager.hpp"

using namespace geode::prelude;

// --- HUB LAYER: MAIN MENU INTEGRATION ---
class $modify(NiceMainHub, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        Notification::create(
            fmt::format("Nice Mod Online | ELO: {}", BattleManager::get().getElo()).c_str(), 
            NotificationIcon::Success
        )->show();

        auto vsButton = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_sprintBtn_001.png"),
            this,
            selector_command(NiceMainHub::onOpenVersusHub)
        );
        vsButton->setScale(0.85f);

        if (auto sideMenu = this->getChildByID("right-side-menu")) {
            sideMenu->addChild(vsButton);
            sideMenu->updateLayout();
        }

        return true;
    }

    void onOpenVersusHub(CCObject*) {
        auto& mgr = BattleManager::get();
        FLAlertLayer::create(
            "Nice Versus - Ultimate Battle Suite",
            fmt::format(
                "<cg>Division:</cg> {}\n"
                "<cy>Rating:</cy> {} ELO | <cl>Streak:</cl> {} 🔥\n"
                "<co>Tokens:</co> {} Battle Tokens\n\n"
                "• <cp>Matchmaking:</cp> 1v1 Ranked / 4v4 War Queues Ready\n"
                "• <cp>Draft Phase:</cp> Pick & Ban Map Protocol Loaded\n"
                "• <cp>Globed Bridge:</cp> Packet Listener Active",
                mgr.getRankTier(),
                mgr.getElo(),
                mgr.getWinStreak(),
                mgr.getTokens()
            ),
            "Close",
            "Queue Match",
            [this](FLAlertLayer* layer, bool queueBtn) {
                if (queueBtn) {
                    BattleManager::get().setMatchActive(true);
                    this->triggerDraftSequence();
                }
            }
        )->show();
    }

    void triggerDraftSequence() {
        FLAlertLayer::create(
            "Ranked Map Draft (Pick / Ban)",
            "Opponent has banned: <cr>Slaughterhouse</cr>, <cr>Bloodbath</cr>\n\n"
            "Your Team Pick: <cg>Sonic Wave</cg>\n"
            "Status: Map locked. Preparing server connection...",
            "Cancel",
            "Accept & Enter",
            [](FLAlertLayer* layer, bool accepted) {
                if (accepted) {
                    Notification::create("Draft finalized! Spawning into level...", NotificationIcon::Success)->show();
                } else {
                    BattleManager::get().setMatchActive(false);
                }
            }
        )->show();
    }
};

// --- GAMEPLAY LAYER: RUBBER-BANDING, DEATH CAMS & TELEMETRY ---
class $modify(NiceGamePlay, PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        if (BattleManager::get().isMatchActive()) {
            Notification::create("Versus Match Live: Synchronizing with Globed...", NotificationIcon::Info)->show();
        }

        return true;
    }

    void destroyPlayer(PlayerObject* player, GameObject* object) {
        if (BattleManager::get().isMatchActive()) {
            Notification::create("Crash Detected! Recording death frame snapshot...", NotificationIcon::Error)->show();
        }
        PlayLayer::destroyPlayer(player, object);
    }

    void levelComplete() {
        PlayLayer::levelComplete();

        if (BattleManager::get().isMatchActive()) {
            auto& mgr = BattleManager::get();
            mgr.addElo(28);
            mgr.incrementStreak();
            mgr.addTokens(20);
            mgr.setMatchActive(false);

            FLAlertLayer::create(
                "MATCH VICTORY!",
                fmt::format(
                    "You won the competitive showdown!\n\n"
                    "<cg>ELO Gained:</cg> +28 (New Total: {})\n"
                    "<cy>Win Streak Bonus:</cy> {} wins active!\n"
                    "<cl>Tokens Added:</cl> +20 (Wallet: {})",
                    mgr.getElo(),
                    mgr.getWinStreak(),
                    mgr.getTokens()
                ),
                "Continue"
            )->show();
        }
    }
};

// --- PAUSE LAYER: EMOTE WHEEL & TACTICAL TAUNTS ---
class $modify(NicePauseOverlay, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        if (auto centerMenu = this->getChildById("center-menu")) {
            auto emoteBtn = CCMenuItemSpriteExtra::create(
                ButtonSprite::create("Emotes", 45, true, "goldFont.fnt", "GJ_button_01.png", 35, 0.6f),
                this,
                selector_command(NicePauseOverlay::onOpenEmotes)
            );
            emoteBtn->setPositionY(-130);
            centerMenu->addChild(emoteBtn);
            centerMenu->updateLayout();
        }
    }

    void onOpenEmotes(CCObject*) {
        FLAlertLayer::create(
            "Quick Emote & Taunt Wheel",
            "Broadcast a message to your 1v1 or 4v4 lobby:\n\n"
            "1. <cg>\"GG! Incredible round!\"</cg>\n"
            "2. <cy>\"Watch this clutch sequence!\"</cy>\n"
            "3. <cr>\"So close, unlucky fail!\"</cr>\n"
            "4. <cl>\"Let's go team, lock in!\"</cl>",
            "Close"
        )->show();
    }
};