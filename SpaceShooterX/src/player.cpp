﻿#include "player.h"

#include "base_console.h"
#include "projectile.h"

void Player::OnAttach()
{
    m_color = FG_GREEN;
    AddPixel(L'U', { 1, 0 }, m_color);
    AddPixel(L'║', { 0, 1 }, m_color);
    AddPixel(L'G', { 1, 1 }, m_color);
    AddPixel(L'╵', { 1, 2 }, m_color);
    AddPixel(L'║', { 2, 1 }, m_color);

    m_hitpoints = MAX_PLAYER_HEALTH;
    projectileSpeed = 20;
    rateOfFire = LAJU_TEMBAK;
    m_rect.position.x = LEBAR_TAMPILAN / 2;
    m_rect.position.y = TINGGI_TAMPILAN - 10;
    lockVerticalBounds = true;

    s_shootSound = std::make_shared<AudioPlayer>("laserShoot.wav");
    s_shootSound->SetVolume(0.7f);
    m_game->RegisterObject(s_shootSound);

    s_healthBoostSound = std::make_shared<AudioPlayer>("powerUp.wav");
    s_healthBoostSound->SetVolume(0.65f);
    s_healthBoostSound->m_destroyOnFinish = true;
    m_game->RegisterObject(s_healthBoostSound);

    Spaceship::OnAttach();
}

void Player::OnDetach()
{
    s_shootSound->Invalidate();
    Spaceship::OnDetach();
}

void Player::OnUpdate(float elapsed)
{
    for (int i = 0; i < 10; i++)
        keys[i] = GetAsyncKeyState("\x25\x27\x26\x28\x20\x57\x41\x53\x44\x10"[i]);

    if (m_velocity.x != 0)
        m_velocity.x = 0;

    if (m_velocity.y != 0)
        m_velocity.y = 0;

    if (keys[0] || keys[6])
        m_velocity.x = -13.0f;
    if (keys[1] || keys[8])
        m_velocity.x = 13.0f;
    if (keys[2] || keys[5])
        m_velocity.y = -13.0f;
    if (keys[3] || keys[7])
        m_velocity.y = 13.0f;

    if (keys[9])
        m_velocity *= 2.0f;

    m_isFiring = keys[4];

    Spaceship::OnUpdate(elapsed);
}

void Player::GiveHealth(float p_value)
{
    s_healthBoostSound->Play();

    m_hitpoints += p_value;

    if (m_hitpoints > MAX_PLAYER_HEALTH)
        m_hitpoints = MAX_PLAYER_HEALTH;
}

void Player::Shoot()
{
    s_shootSound->Play();
    Spaceship::Shoot();
}

std::shared_ptr<Projectile> Player::CreateProjectile()
{
    return std::make_shared<Projectile>(std::static_pointer_cast<Player>(weak_from_this().lock()));
}
