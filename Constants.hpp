#pragma once
#include <string>

// ── Window ───────────────────────────────────────────────────────────────────
constexpr unsigned int WIN_WIDTH  = 1280;
constexpr unsigned int WIN_HEIGHT = 720;

// ── Physics ──────────────────────────────────────────────────────────────────
constexpr float WALK_SPEED    = 220.f;
constexpr float RUN_SPEED     = 380.f;
constexpr float JUMP_FORCE    = -580.f;
constexpr float GRAVITY       = 1400.f;
constexpr float LADDER_SPEED  = 150.f;

// ── Gun ──────────────────────────────────────────────────────────────────────
constexpr int   MAX_AMMO           = 7;
constexpr float RELOAD_TIME        = 5.0f;
constexpr float BULLET_SPEED       = 950.f;
constexpr float GUN_SNAP_SPEED     = 600.f;   // deg/sec snapping UP to sky
constexpr float GUN_SWEEP_SPEED    = 45.f;    // deg/sec sweeping DOWN to opponent
constexpr float GUN_LOCK_THRESHOLD = 3.f;     // degrees — close enough to fire

// ── Game flow ─────────────────────────────────────────────────────────────────
constexpr int   ROUNDS_TO_WIN   = 3;
constexpr float COUNTDOWN_DELAY = 1.0f;
constexpr float SCREENSHAKE_TIME = 0.4f;
constexpr float SCREENSHAKE_MAG  = 8.f;
constexpr float HIT_FLASH_TIME   = 0.15f;

// ── Names ─────────────────────────────────────────────────────────────────────
constexpr int MAX_NAME_LEN = 8;
const std::string DEFAULT_P1_NAME = "Player 1";
const std::string DEFAULT_P2_NAME = "Player 2";
